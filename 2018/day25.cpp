// Advent of Code 2018
// Day 25: Four-Dimensional Adventure
// https://adventofcode.com/2018/day/25

#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <vector>

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
int mhdist(const Point& p, const Point& q) {
    return abs(p.x - q.x) + abs(p.y - q.y) + abs(p.z - q.z) + abs(p.t - q.t);
}

// dfs on graph of adjacency lists of points within distance 3 of each other
void solve() {
    std::vector<Point> points{std::istream_iterator<Point>{std::cin}, {}};
    int pointCount = points.size();

    std::vector<std::vector<int>> within3(pointCount);
    std::vector<bool> visited(pointCount);
    int constellations = 0;

    // for each point pair, build an adjacency list of points within dist 3
    for (int p = 0; p < pointCount; ++p)
        for (int q = p + 1; q < pointCount; ++q)
            if (mhdist(points[p], points[q]) <= 3)
                within3[p].push_back(q), within3[q].push_back(p);

    // recursive lambda: depth-first search to mark nodes as seen
    std::function<void(int)> visit_dfs = [&](int node) {
        visited[node] = true;
        for (int edge : within3[node])
            if (not visited[edge])
                visit_dfs(edge);
    };

    // count constellations of points and mark them as counted
    for (int point = 0; point < pointCount; ++point)
        if (not visited[point])
            ++constellations, visit_dfs(point);

    // Part 1
    // How many constellations are formed by the fixed points in spacetime?
    // Your puzzle answer was 324
    std::cout << "Part 1: " << constellations << std::endl;
}

// Main: Time the solver.
int main() {
    // Speed up C++ io by unsyncing with C stdio and untie cin/cout.
    std::ios_base::sync_with_stdio(false);
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