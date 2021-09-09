// Advent of Code 2018
// Day 22: Mode Maze
// #include <bits/stdc++.h>
// #include <array>
// #include <bitset>
// #include <cassert>
// #include <deque>
// #include <list>
// #include <map>
// #include <numeric>
// #include <regex>
// #include <sstream>
// #include <string>
// #include <tuple>
// #include <unordered_map>
// #include <unordered_set>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <istream>
#include <iterator>
#include <queue>
#include <set>
#include <tuple>
#include <vector>
using namespace std;

// 4-tuple with less-than comparison operator (for the priority_queue)
// Returns the reversed comparison since priority_queue returns the
// maximal element and Dijkstra uses the minimal
struct Node {
    int x, y, tool, cost; // (x,y) coord, tool-type, path-time cost
    bool operator<(const Node &rhs) const { return cost > rhs.cost; }
};

// Dijkstra's shortest path cost on weighted graph
// Edge cost is the time it takes to move to another location in the cave
void solve() {
    // Example // solution: 114, 45
    // depth: 510
    // target: 10,10
    // int depth = 510;
    // int tgtX = 10;
    // int tgtY = 10;
    // int maxX = 20;
    // int maxY = 20;

    // Input // solution: 6256, 973
    // depth: 5913
    // target: 8,701
    int depth = 5913; // cave depth (input)
    int tgtX = 8;     // target location (input)
    int tgtY = 710;   // target location (input)
    int maxX = 100;   // cave upper bound
    int maxY = 1000;  // cave upper bound

    // parse input via scanf
    // scanf("depth: %d\ntarget:%d,%d", &depth, &tgtX, &tgtY);
    // depth = 510, tgtX = 10, tgtY = 10; // ans: 114, 45

    // parse input via cin
    string skip;
    char comma;
    cin >> skip >> depth >> skip >> tgtX >> comma >> tgtY;
    // depth = 510, tgtX = 10, tgtY = 10; // ans: 114, 45

    // extend cave bounds beyond target a reasonable amount
    maxX = tgtX + 100;
    maxY = tgtY + 100;

    // init vectors
    vector<vector<int>> geology(maxY, vector<int>(maxX, 0));
    vector<vector<int>> erosion(maxY, vector<int>(maxX, 0));

    // compute geologic indices and erosion levels
    const int MOD = 20183;
    for (int y = 0; y < maxY; y++)
        for (int x = 0; x < maxX; x++) {

            if (y == 0 && x == 0)
                geology[y][x] = 0;
            else if (y == tgtY && x == tgtX)
                geology[y][x] = 0;
            else if (y == 0)
                geology[y][x] = x * 16807;
            else if (x == 0)
                geology[y][x] = y * 48271;
            else
                geology[y][x] = erosion[y - 1][x] * erosion[y][x - 1];

            // compute erosion level from geological index
            erosion[y][x] = (geology[y][x] + depth) % MOD;
        }

    // Part 1: Sum the risk levels from the cave mouth to the target location
    int totalRiskLevel = 0;
    for (int y = 0; y <= tgtY; y++)
        for (int x = 0; x <= tgtX; x++)
            totalRiskLevel += (erosion[y][x] % 3);

    // Part 2: Dijkstra's shortest path on weighted graph
    // Edge cost: time to move to next node/location

    // rocky, wet, narrow = {0,1,2} cave type (erosion mod 3)
    // none, torch, gear  = {0,1,2} tool type not usable in cave type of same #
    enum { none, torch, gear }; // tool types
    vector<int> tools{none, torch, gear};

    // vector<int> dY{-1, 0, 1, 0}; // up, down
    // vector<int> dX{0, 1, 0, -1}; // right, left
    vector<pair<int, int>> nsew{{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

    // Dijkstra Shortest Path Algorithm (on weighted graph)
    int fewestMinutes{0};              // part 2 memo (lowest path cost)
    priority_queue<Node> PQ;           // {x,y,tool,cost}
    set<tuple<int, int, int>> visited; // {x,y,tool}
    PQ.push({0, 0, torch, 0});         // start at cave mouth with torch
    while (!PQ.empty()) {
        auto [x, y, tool, cost] = PQ.top();
        PQ.pop();

        // reached our target with the torch equipped, done!
        if ((y == tgtY) && (x == tgtX) && (tool == torch)) {
            fewestMinutes = cost;
            break;
        }

        // skip visited nodes, memo new ones
        if (visited.count({x, y, tool}))
            continue;
        else
            visited.insert({x, y, tool});

        // Try each tool type: time cost +7 minutes
        int caveType = erosion[y][x] % 3;
        for (int const &toolType : tools)
            if (caveType != toolType)
                PQ.push({x, y, toolType, (cost + 7)});

        // Try each direction: time cost +1 minute
        for (pair<int, int> const &dx_dy : nsew) {
            auto [dx, dy] = dx_dy;
            dx += x, dy += y;
            int caveType = erosion[dy][dx] % 3;
            bool inCave{(0 <= dx) && (dx < maxX) && (0 <= dy) && (dy < maxY)};
            if (inCave && (caveType != tool))
                PQ.push({dx, dy, tool, (cost + 1)});
        }
    }

    // Part 1: What is the total risk level for the smallest rectangle that
    // includes the cave mouth (0,0) and the target's coordinates?
    cout << "[Part 01] = " << totalRiskLevel << endl; // 6256

    // Part 2: What is the fewest # of minutes you can take to reach the target?
    cout << "[Part 02] = " << fewestMinutes << endl; // 973
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
