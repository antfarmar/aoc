// Advent of Code 2018
// Day 11: Chronal Charge
// https://adventofcode.com/2018/day/11

#include <chrono>
#include <iostream>
using namespace std;

// O(n^2) using 2D partial sums in a Summed-Area Table.
// https://en.wikipedia.org/wiki/Summed-area_table
void solve() {
    int grid[301][301];                       // the summmed-area table
    int gridSerial = 9110;                    // given input
    int bestX, bestY, bestSize, best = -1e9;  // memo variables
    int best3X, best3Y;                       // part 1 answer

    // Build the 300x300 summed-area table using fuel cell power levels.
    for (int y = 1; y <= 300; y++)
        for (int x = 1; x <= 300; x++) {
            int rackID = x + 10;
            int powerLevel = rackID * y + gridSerial;
            powerLevel = ((powerLevel * rackID / 100) % 10) - 5;
            grid[y][x] = powerLevel + grid[y - 1][x] + grid[y][x - 1] -
                         grid[y - 1][x - 1];
        }

    // Find the largest total power of all square sizes using the summed-area
    // table of fuel cell power level sums.
    for (int s = 1; s <= 300; s++)
        for (int y = s; y <= 300; y++)
            for (int x = s; x <= 300; x++) {
                int totalPower = grid[y][x] - grid[y - s][x] - grid[y][x - s] +
                                 grid[y - s][x - s];
                if (totalPower > best) {
                    best = totalPower, bestX = x, bestY = y, bestSize = s;
                    if (bestSize == 3)
                        best3X = bestX, best3Y = bestY;
                }
            }

    // Part 1
    // What is the X,Y coordinate of the top-left fuel cell of the 3x3 square
    // with the largest total power?
    // Your puzzle answer was 21,13
    cout << "[Part 1]  = " << best3X - 2 << "," << best3Y - 2 << endl;  // 21,13

    // Part 2
    // What is the X,Y,SIZE identifier of the square with the largest total
    // power?
    // Your puzzle answer was 235,268,13
    cout << "[Part 2]  = " << (bestX - bestSize) + 1 << ","
         << (bestY - bestSize) + 1 << "," << bestSize << endl;  // 235,268,13
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