// Advent of Code 2018
// Day 18: Settlers of The North Pole
// https://adventofcode.com/2018/day/18

#include <algorithm>
#include <chrono>
#include <iostream>
#include <istream>
#include <iterator>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

typedef unordered_map<uint16_t, uint8_t> umap;

// Coordinates: helper struct
struct coord {
    uint8_t x, y;  // coords
    uint16_t key;  // hash key

    // Constructor
    coord(uint8_t x, uint8_t y) : x(x), y(y) { keyify(); };

    // Computes the key: Two 8-bit ints packed into one 16-bit int
    void keyify() { key = (x << 8) | y; }

    // Displacement helpers
    coord dx(int8_t d) { return {uint8_t(x + d), y}; }
    coord dy(int8_t d) { return {x, uint8_t(y + d)}; }
};

// The Lumberyard: Simulates the lumberyard growth patterns
struct Yard {
    const uint8_t open = '.';  // cell types
    const uint8_t tree = '|';
    const uint8_t lumb = '#';
    umap grid, grid2;  // hash map of coords to cell type
    size_t rows;       // yard size: 50 rows
    size_t cols;       // yard size: 50 cols

    // Count neighbours of a given grid cell coordinate
    int cntNbor(coord co, uint8_t type) {
        int cnt = 0;
        for (int8_t dy = -1; dy <= 1; dy++) {
            for (int8_t dx = -1; dx <= 1; dx++) {
                coord nbor{co.dx(dx).dy(dy)};
                if (grid[nbor.key] == type)
                    cnt++;
            }
        }
        return cnt;
    }

    // Simulates one tick of lumberyard growth
    void simulate() {
        grid2 = grid;  // copy for double-buffering
        for (uint8_t y = 1; y < rows; y++)
            for (uint8_t x = 1; x < cols; x++) {
                coord cur{x, y};
                uint8_t cell = grid[cur.key];

                // The growth rules
                if (cell == open && cntNbor(cur, tree) >= 3)
                    grid2[cur.key] = tree;
                else if (cell == tree && cntNbor(cur, lumb) >= 3)
                    grid2[cur.key] = lumb;
                else if (cell == lumb)
                    if (!(cntNbor(cur, lumb) > 1 && cntNbor(cur, tree) > 0))
                        grid2[cur.key] = open;
            }
        swap(grid, grid2);  // swap to new grid
    }

    // Debug draw the lumberyard
    void draw() {
        string ascii{string(rows, '\n')};  // clear screen
        for (uint8_t y = 1; y < rows; y++) {
            for (uint8_t x = 1; x < cols; x++)
                ascii += grid[coord{x, y}.key];
            ascii += '\n';
        }
        cerr << ascii;
        this_thread::sleep_for(100ms);
    }

    // Count all the cells of some type
    int countType(uint8_t type) const {
        return count_if(grid.begin(), grid.end(),
                        [&](auto& e) { return e.second == type; });
    }

    // Parse the lumberyard data
    friend std::istream& operator>>(std::istream& is, Yard& Y) {
        uint8_t x = 1, y = 1;  // margins @ top & left (prevent negatives)
        for (string line; getline(cin, line);) {
            x = 1;
            for (uint8_t c : line)
                Y.grid.emplace(coord{x++, y}.key, c);
            y++;
        }
        // Compute rows/cols
        Y.rows = y, Y.cols = x;
        cerr << "Yard Size: " << Y.rows << "x" << Y.cols << endl;
        return is;
    }
};  // end Yard

// Cellular automaton simulation.
void solve() {
    uint32_t part1{0}, part2{0};      // solution memos
    uint32_t treeCnt{0}, lumbCnt{0};  // resource counts

    // Cycle detection variables
    unordered_map<uint32_t, uint32_t> yardHashCnt;
    size_t cycleStart{0}, billion{1000000000};
    uint32_t thresh{4}, target{0};

    // Simulate the lumberyard growth
    Yard yard;
    cin >> yard;
    for (size_t curTime = 1; curTime <= billion; curTime++) {
        yard.simulate();
        // yard.draw();
        // this_thread::sleep_for(100ms);

        // Count the resources at this timestep
        treeCnt = yard.countType(yard.tree);
        lumbCnt = yard.countType(yard.lumb);

        // Use the resource counts as a hash/checksum to detect cycles
        uint32_t resProd = treeCnt * lumbCnt;      // resources product (hash)
        uint32_t rptCnt = ++yardHashCnt[resProd];  // how many times it occured

        // Look for repeating resource products after a reasonable threshold cnt
        if (rptCnt >= thresh && !target)
            target = resProd, cycleStart = curTime;

        // cerr << "Time: " << curTime << " Prod: " << resProd
        //      << " Reps: " << rptCnt << endl;

        // Look for the repeating resource product hash value again
        if ((target == resProd) && (curTime > cycleStart)) {
            size_t period = (curTime - cycleStart);
            size_t deltaTime = (billion - cycleStart) % period;
            cycleStart = billion;           // prevent entering this block again
            billion = curTime + deltaTime;  // timestep to stop at (~1000000000)

            cerr << "Period: " << period << endl;
            cerr << "Time: " << curTime << endl;
            cerr << "Delta: " << deltaTime << endl;
            cerr << "Stop: " << billion << endl;
        }

        // At 10 iterations, we have the part 1 solution
        if (curTime == 10)
            part1 = resProd;

        // At a time equivalent to a "billion", we have the part 2 solution
        part2 = resProd;
    }

    // Part 1: What will the total resource value of the lumber collection area
    // be after 10 minutes/iterations?
    cout << "[Part 01] = " << part1 << endl;  // 506160

    // Part 2: What will the total resource value of the lumber collection area
    // be after 1,000,000,000 (1 billion) minutes/iterations? 1000000000
    cout << "[Part 02] = " << part2 << endl;  // 189168 // cycle size: 28
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