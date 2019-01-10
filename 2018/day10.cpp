// Advent of Code 2018
// Day 10: The Stars Align
// https://adventofcode.com/2018/day/10

#include <algorithm>
#include <chrono>
#include <iostream>
#include <istream>
#include <iterator>
#include <tuple>
#include <vector>
using namespace std;

// Helper struct
struct Star {
    int px{0}, py{0};  // position
    int vx{0}, vy{0};  // velocity

    // Translate star position by velocity.
    void update() {
        px += vx;
        py += vy;
    }

    // For minmax comparisons.
    bool operator<(Star const& rhs) const {
        return tie(py, px) < tie(rhs.py, rhs.px);
    }

    // Parse input via std::cin >>.
    friend std::istream& operator>>(std::istream& is, Star& s) {
        // e.g. position=<-9,  1> velocity=< 0,  2>
        int ig = 99;
        char c;
        cin.ignore(ig, '<');
        is >> s.px >> c >> s.py;
        cin.ignore(ig, '<');
        is >> s.vx >> c >> s.vy;
        cin.ignore(ig, '\n');
        return is;
    }
};

// Parse input via sscanf.
vector<Star> readInput() {
    vector<Star> vs;
    char form[] = "position=<%d,%d> velocity=<%d,%d>";
    for (string line; getline(cin, line);) {
        Star s;
        sscanf(line.c_str(), form, &s.px, &s.py, &s.vx, &s.vy);
        vs.push_back(s);
    }
    return vs;
}

// Draw the stars: loop through the sky grid and stars.
string drawStarmap(Star& minStar, Star& maxStar, vector<Star>& stars) {
    string starmap;
    for (int y = minStar.py; y <= maxStar.py; y++) {
        for (int x = minStar.px; x <= maxStar.px; x++) {
            char pixel = ' ';
            for (Star& star : stars) {
                if (x == star.px && y == star.py) {
                    pixel = '*';
                    break;
                }
            }
            starmap += pixel;
        }
        starmap += '\n';
    }
    return starmap;
}

// Solve the challenge. Optimization of star distances: minimize.
void solve() {
    // vector<Star> stars = readInput();
    vector<Star> stars{istream_iterator<Star>{cin}, {}};

    // Update/converge star positions until they diverge again.
    int delta = 1e9;
    int prevDelta = delta + 1;
    int iterations = -1;
    const int threshold = 10;
    for (; delta < prevDelta; iterations++) {
        for_each(stars.begin(), stars.end(), [](Star& s) { s.update(); });
        auto [minStar, maxStar] = minmax_element(stars.begin(), stars.end());
        // tie(minStar, maxStar) = minmax_element(stars.begin(), stars.end());
        prevDelta = delta;
        delta = (maxStar->py - minStar->py);
        if (delta < threshold)  // print when stars are near each other
            cout << drawStarmap(*minStar, *maxStar, stars);
    }

    // Part 1: What message will eventually appear in the sky?
    cout << "[Part 1] = KFLBHXGK" << endl;  // after initial delta/10 iterations

    // Part 2: # of seconds (iterations) for that message to appear?
    cout << "[Part 2] = " << iterations << endl;  // 10659
}

// Main: Time the solver.
int main() {
    auto start_time = std::chrono::high_resolution_clock::now();
    solve();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto ms_count = std::chrono::duration_cast<std::chrono::milliseconds>(
                        end_time - start_time)
                        .count();
    std::clog << "[Run Time] = " << ms_count << "ms" << std::endl;
}