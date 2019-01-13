// Advent of Code 2018
// Day 25: Four-Dimensional Adventure
// https://adventofcode.com/2018/day/25

#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <vector>
using namespace std;

// a 4D point in spacetime
struct Point {
    int x, y, z, t;
};

// parse the points via an istream
std::istream& operator>>(std::istream& is, Point& p) {
    char comma;  // x,y,z,t
    is >> p.x >> comma >> p.y >> comma >> p.z >> comma >> p.t;
    return is;
}

// manhattan distance b/w two 4D points
int mhdist(const Point& a, const Point& b) {
    return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z) + abs(a.t - b.t);
}

// dfs on graph of adjacency lists of points within distance 3 of each other
void solve() {
    vector<Point> points{istream_iterator<Point>{cin}, {}};
    int pointCount = points.size();

    vector<vector<int>> within3(pointCount);
    vector<bool> visited(pointCount);
    int constellations = 0;

    // for each point pair, build an adjacency list of points within dist 3
    for (auto pntA = 0; pntA < pointCount; ++pntA)
        for (auto pntB = pntA + 1; pntB < pointCount; ++pntB) {
            int dist = mhdist(points[pntA], points[pntB]);
            if (dist <= 3) {
                within3[pntA].push_back(pntB);
                within3[pntB].push_back(pntA);
            }
        }

    // recursive lambda: depth-first search to mark nodes as seen
    std::function<void(int)> visit_dfs = [&](int node) {
        visited[node] = true;
        for (int edge : within3[node])
            if (not visited[edge])
                visit_dfs(edge);
    };

    // visit constellations of points and count them
    for (int point = 0; point < pointCount; point++) {
        if (not visited[point]) {
            constellations++;
            visit_dfs(point);
        }
    }

    // Part 1
    // How many constellations are formed by the fixed points in spacetime?
    // Your puzzle answer was 324
    cout << "Part 1: " << constellations << endl;
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