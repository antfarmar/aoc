// Advent of Code 2018
// Day 17: Reservoir Research
// https://adventofcode.com/2018/day/17

#include <algorithm>
#include <chrono>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
using namespace std;

// Coordinates: helper struct
struct coord {
    int x, y;
    coord dx(int d) { return {x + d, y}; }
    coord dy(int d) { return {x, y + d}; }
};

// Vein: a clay vein on the ground
struct Vein {
    int x0, x1, y0, y1;  // coords of the vein "line"

    // Default constructor: largest bounding box
    Vein() : x0(INT_MAX), x1(INT_MIN), y0(INT_MAX), y1(INT_MIN) {}

    // To iteratively compute a bounding box
    void minmax(const Vein& other) {
        x0 = min(x0, other.x0 - 1);  // margin left
        x1 = max(x1, other.x1 + 1);  // margin right
        y0 = min(y0, other.y0 - 1);  // margin top
        y1 = max(y1, other.y1);      // just bottom
    }

    // Move coords relative to a (0,0) origin
    void translate(const Vein& bbox) {
        x0 -= bbox.x0, x1 -= bbox.x0;
        y0 -= bbox.y0, y1 -= bbox.y0;
    }

    // Parse the vein input
    friend std::istream& operator>>(std::istream& is, Vein& v) {
        char c;
        string line;
        getline(cin, line);
        // Construct a vein by parsing raw input string data
        sscanf(line.c_str(), "%c=%d, %*c=%d..%d", &c, &v.x0, &v.y0, &v.y1);
        v.x1 = v.x0 + 1, v.y1++;  // half-open intervals: [x0,x1) [y0,y1)
        if (c == 'y')             // input varied x or y first
            swap(v.x0, v.y0), swap(v.x1, v.y1);
        return is;
    }
};

// Ground grid : 2D-Array of Elements
enum Element { SAND, CLAY, WATER, FLOW, VOID };  // ground grid tile types
struct Ground {
    int sizeX, sizeY;              // ground dimensions
    Vein bbox;                     // bounding box of veins
    vector<vector<Element>> grid;  // 2D array[y][x] of Elements

    // "Constructor"
    void initialize(Vein& vbox) {
        bbox = vbox;
        sizeX = bbox.x1 - bbox.x0;                         // x=cols
        sizeY = bbox.y1 - bbox.y0;                         // y=rows
        grid.resize(sizeY, vector<Element>(sizeX, SAND));  // [y][x]
    }

    // Set ground vein tiles to CLAY type
    void addVein(Vein v) {
        for (int x = v.x0; x < v.x1; x++)
            for (int y = v.y0; y < v.y1; y++)
                set({x, y}, CLAY);
    }

    // Grid bounds-checking helper function
    bool inBounds(coord co) const {
        return (co.x >= 0) && (co.x < sizeX) && (co.y >= 0) && (co.y < sizeY);
    }

    // Get the element type at tile (x,y): grid[y][x]
    auto get(coord co) const { return inBounds(co) ? grid[co.y][co.x] : VOID; }

    // Set the element at tile (x,y) to e: grid[y][x] = e
    void set(coord co, Element e) {
        if (inBounds(co))
            grid[co.y][co.x] = e;
    }

    // Getter/Setter alternate: overloaded [] operator
    // const Element &operator[](coord co) const { return grid[co.y][co.x]; }
    Element OOB = VOID;  // out-of-bounds dummy reference
    Element& operator[](coord co) {
        return inBounds(co) ? grid[co.y][co.x] : OOB;
    }

    // Debug draw the entire ground grid
    void draw() const {
        for (auto& row : grid) {
            for (auto& e : row)
                cerr << ".#~|"[e];
            cerr << endl;
        }
    }

    // Count all the water elements on the ground (parts 1 & 2)
    int countWater() const {
        int cnt = 0;
        for (auto& row : grid)
            cnt += count(row.begin(), row.end(), WATER);
        return cnt;
    }

    // Count all the flow elements on the ground (part 1)
    int countFlow() const {
        int cnt = -1;  // minus the topmost spring flow
        for (auto& row : grid)
            cnt += count(row.begin(), row.end(), FLOW);
        return cnt;
    }

    // Parse the clay vein data and construct the Ground grid
    friend std::istream& operator>>(std::istream& is, Ground& G) {
        // Parse all the clay vein input data.
        vector<Vein> VeinList{istream_iterator<Vein>{cin}, {}};

        // Get bounding box of all veins
        Vein bbox;
        for (auto&& v : VeinList)
            bbox.minmax(v);

        // Shift all veins to (0,0) array base
        for (auto& v : VeinList)
            v.translate(bbox);

        // Initialize the ground grid
        G.initialize(bbox);
        for (auto&& v : VeinList)
            G.addVein(v);

        return is;
    }
};  // end Ground

// Recursively fill ground areas with water
void fill(Ground& G, coord co, int dir) {
    // if (G.get(co) == FLOW) {
    if (G[co] == FLOW) {
        fill(G, co.dx(dir), dir);
        // G.set(co, WATER);
        G[co] = WATER;
    }
}

// Recursively flow water down or laterally on the ground
bool flow(Ground& G, coord co, int dir = 0) {  // 0=down
    // Flowable directions
    int left = -1, down = 0, right = 1;

    // Get the current tile's element type // Element e = G.get(co);
    Element e = G[co];

    // Check if the current element tile is flowable
    if (e != SAND)
        return (e != CLAY && e != WATER);  // (FLOW || VOID)

    // At a flowable tile // G.set(co, FLOW);
    G[co] = FLOW;

    // Try to flow down (increasing y)
    bool flowable = flow(G, co.dy(1), down);
    if (flowable)
        return true;

    // Down is not flowing, so flow laterally
    flowable = (dir <= down) && flow(G, co.dx(left), left);
    flowable |= (dir >= down) && flow(G, co.dx(right), right);
    if (flowable)
        return true;

    // In a watertight vein, so fill it up
    if (dir == down) {
        fill(G, co, left);
        fill(G, co.dx(right), right);
    }

    // Can't flow anymore
    return false;
}

// Recursive solution.
void solve() {
    Ground G;  // the ground grid of veins where the water flow takes place
    cin >> G;  // 2D array[][] grid of tiles of element types

    // Start the recursive water flow sim, emanating from the (shifted) spring
    coord spring{500, 0};   // original input water spring coords
    spring.x -= G.bbox.x0;  // shift towards origin in the translated bbox
    spring.y = max(0, spring.y - G.bbox.y0);
    flow(G, spring);

    // Debug draw
    // G.draw();

    // Count the water & flow elements for the solutions
    int waterCount = G.countWater();
    int flowCount = G.countFlow();

    // Part 1: How many tiles can the water reach within the range of y values
    // in your scan? (ignore tiles with a y coord smaller than the min y coord)
    cout << "[Part 01] = " << waterCount + flowCount << endl;  // 33242

    // Part 2: How many water tiles are left after the water spring stops
    // producing water and all remaining water not at rest has drained?
    cout << "[Part 02] = " << waterCount << endl;  // 27256
}

// Main: Time the solver.
int main() {
    // Speed up C++ io by unsyncing with C stdio and untie cin/cout.
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    // Time the solve() function in milliseconds.
    auto start_time = chrono::high_resolution_clock::now();
    solve();
    auto end_time = chrono::high_resolution_clock::now();
    auto ms_count =
        chrono::duration_cast<chrono::milliseconds>(end_time - start_time)
            .count();
    clog << "[Runtime] = " << ms_count << "ms" << endl;
}