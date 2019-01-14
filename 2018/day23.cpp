// Advent of Code 2018
// Day 23: Experimental Emergency Teleportation
// https://adventofcode.com/2018/day/23

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <set>
#include <vector>
using namespace std;

// nanobot: location (x,y,z) and signal radius (r)
struct Bot {
    int64_t x, y, z, r{0};

    // for set comparisons
    bool operator<(const Bot& rhs) const {
        // return r > rhs.r;
        if (x < rhs.x)
            return true;
        else if (x > rhs.x)
            return false;
        if (y < rhs.y)
            return true;
        else if (y > rhs.y)
            return false;
        if (z < rhs.z)
            return true;
        else
            return z < rhs.z;
    }

    // for set comparisons
    // bool operator==(const Bot &p) const {
    //     return x == p.x && y == p.y && z == p.z;
    // }
};

// same thing, really
typedef Bot Point;

// output a bot
ostream& operator<<(ostream& os, const Bot& b) {
    os << b.x << "," << b.y << "," << b.z << "," << b.r;
    return os;
}

// parse the input bots
istream& operator>>(istream& is, vector<Bot>& bots) {
    int64_t x, y, z, r;
    while (scanf("pos=<%ld,%ld,%ld>, r=%ld\n", &x, &y, &z, &r) == 4)
        bots.push_back({x, y, z, r});
    return is;
}

// manhattan distance
int64_t mhDist(const Point& a, const Point& b) {
    return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
}

// is a bot/point within range of another bot's signal radius?
bool inRange(const Bot& bot, const Point& point, const int64_t& buffer) {
    return (mhDist(bot, point) <= bot.r + buffer);
}

//  Progressive refinement, as seen in other solutions.
//  Not guaranteed to always work.
void solve() {
    vector<Bot> bots;
    cin >> bots;

    // part 1
    // find the bot with the strongest signal radius
    sort(bots.begin(), bots.end(),  // sort by greater signal strength radius
         [](const Bot& lhs, const Bot& rhs) { return lhs.r > rhs.r; });

    // count how many bots are within the strongest bot's signal radius
    Bot strongestBot = bots[0];
    int64_t numBotsInRange =  // part 1 memo
        count_if(bots.begin(), bots.end(),
                 [&](const Bot& bot) { return inRange(strongestBot, bot, 0); });

    // part 2
    // first find the points that are in range of the largest number of bots
    // compute the bounding box of the bot's locations + signal radii
    int64_t x_min{0}, y_min{0}, z_min{0}, x_max{0}, y_max{0}, z_max{0};
    for (auto& b : bots) {
        x_min = min(x_min, b.x - b.r), x_max = max(x_max, b.x + b.r + 1);
        y_min = min(y_min, b.y - b.r), y_max = max(y_max, b.y + b.r + 1);
        z_min = min(z_min, b.z - b.r), z_max = max(z_max, b.z + b.r + 1);
    }

    // range is the bbox size, is a power of 2, and is halved each iteration
    int64_t x_size{x_max - x_min}, y_size{y_max - y_min}, z_size{z_max - z_min};
    int64_t range{int64_t(2) << int(log2(x_size + y_size + z_size))};  // perim

    // starting points: 8 corners of the bbox
    set<Point> tryPoints, bestPoints;
    for (int64_t dx = 0; dx <= 1; ++dx)
        for (int64_t dy = 0; dy <= 1; ++dy)
            for (int64_t dz = 0; dz <= 1; ++dz) {
                tryPoints.insert({x_min + (dx * range), y_min + (dy * range),
                                  z_min + (dz * range), 0});
            }

    // progressively refine the best points as the range is halved to 0
    while (1) {
        int64_t bestCount{0};
        for (auto& point : tryPoints) {
            int64_t numBotsInRange = count_if(
                bots.begin(), bots.end(),
                [&](const Bot& bot) { return inRange(bot, point, range); });

            // keep the best points: most bots in range of it
            if (numBotsInRange >= bestCount) {
                if (numBotsInRange > bestCount) {
                    bestCount = numBotsInRange;
                    bestPoints.clear();
                }
                bestPoints.insert(point);
            }
        }

        // range is 0, done!
        if (!range)
            break;

        // setup next halving iteration
        range >>= 1;
        tryPoints.clear();
        if (!range)  // last iteration
            swap(tryPoints, bestPoints);
        else {
            for (auto& point : bestPoints) {
                for (int64_t dx = -range; dx <= range; dx += range)
                    for (int64_t dy = -range; dy <= range; dy += range)
                        for (int64_t dz = -range; dz <= range; dz += range)
                            tryPoints.insert(
                                {point.x + dx, point.y + dy, point.z + dz});
            }
        }
    }

    // finally, find the closest point to the origin in the best point set
    // int64_t closestToOrigin{numeric_limits<int64_t>::max()}; // part2 memo
    int64_t closestToOrigin{INT64_MAX};  // part2 memo
    for (auto& point : bestPoints)
        closestToOrigin = min(closestToOrigin, mhDist(point, {0, 0, 0}));

    // Part 1: Find the nanobot with the largest signal radius. How many
    // nanobots are in range of its signals?
    cout << "[Part 01] = " << numBotsInRange << endl;  // 408

    // Part 2: Find the coordinates that are in range of the largest number of
    // nanobots. What is the shortest manhattan distance between any of those
    // points and (0,0,0)?
    cout << "[Part 02] = " << closestToOrigin << endl;  // 121167568
}

// Main: Time the solver.
int main() {
    // Speed up C++ io by unsyncing with C stdio and untie cin/cout.
    ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    // Time the solve() function in milliseconds.
    auto start_time = std::chrono::high_resolution_clock::now();
    solve();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto ms_count = std::chrono::duration_cast<std::chrono::milliseconds>(
                        end_time - start_time)
                        .count();
    std::clog << "[Runtime] = " << ms_count << "ms" << std::endl;
}