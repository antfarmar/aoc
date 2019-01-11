// Advent of Code 2018
// Day 13: Mine Cart Madness
// https://adventofcode.com/2018/day/13

#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>
using namespace std;

// Global constants.
// Parallel arrays of velocities & direction state.
const string cartChars = "v>^<";  // the cart dir states
const int dx[] = {0, 1, 0, -1};   // x velocities
const int dy[] = {1, 0, -1, 0};   // y velocities

// Helper struct.
struct Cart {
    int x, y;     // x,y location in track grid. (0,0) is top left
    int dir, dd;  // direction (idx into dx,dy) and delta (for intersections)
    bool crash;   // did this cart crash?

    // Constructor.
    Cart(int x, int y, int d) : x(x), y(y), dir(d), dd(1), crash(false) {}

    // For sorting.
    // The furthest left column is X=0 and the furthest top row is Y=0.
    bool operator<(const Cart& other) {
        return make_pair(y, x) < make_pair(other.y, other.x);
    }

    // Moves this cart on the track for one tick, and checks for crashes.
    bool move(vector<string>& trackGrid, vector<Cart>& carts) {
        x += dx[dir], y += dy[dir];  // move to new location based on state

        // Update cart's direction based on the track piece at the new location.
        // Each time a cart has the option to turn (at a + intersection), it
        // turns left the first time, goes straight the second time, turns right
        // the third time, and then repeats those directions again.
        char track = trackGrid[y][x];
        switch (track) {
            case '\\':  // up(2)->left(3), right(1)->down(0)
                dir ^= 1;
                break;
            case '/':  // up(2)->right(1), left(3)->down(0)
                dir = ((dir + 2) ^ 1) & 3;
                break;
            case '+':  // 4-way intersection
                dir = (dir + dd + 4) & 3;
                dd = (--dd < -1) ? 1 : dd;
                break;
        }

        // If this cart moved to a location with another cart, then they
        // crash and we update them accordingly, informing the caller.
        bool moveOK = true;
        for (Cart& c : carts)
            if (c.x == x && c.y == y && &c != this)
                crash = true, c.crash = true, moveOK = false;

        return moveOK;
    }
};

// Simulate the carts on the track.
void solve() {
    vector<string> trackGrid;                // the track
    vector<Cart> carts;                      // the carts
    int firstCrashX = -1, firstCrashY = -1;  // part 1 memo

    // Parse the track into a string vector.
    for (string line; getline(cin, line); trackGrid.push_back(line))
        ;

    // Find all the carts and their initial state on the track.
    // Initially, the track under each cart is a straight path matching the
    // direction the cart is facing.
    int rows = trackGrid.size(), cols = trackGrid[0].size();
    for (int i = 0; i < rows; i++)        // strings
        for (int j = 0; j < cols; j++) {  // substring chars
            int pos = cartChars.find(trackGrid[i][j]);
            if (size_t(pos) != string::npos) {  // found a cart
                carts.push_back(Cart(j, i, pos));
                trackGrid[i][j] = ((pos & 1) == 0) ? '|' : '-';
            }
        }

    // Move all the carts one tick (until only one is left still moving).
    // Carts all move at the same speed; they take turns moving a single step at
    // a time. They do this based on their current location: carts on the top
    // row move first (from left to right), then carts on the second row move.
    while (carts.size() > 1) {  // part 2
        // Move each cart in sorted order and remember the first crash location.
        sort(carts.begin(), carts.end());
        for (Cart& c : carts)
            if (!c.move(trackGrid, carts))  // cart crashed
                if (firstCrashX < 0)        // part 1
                    firstCrashX = c.x, firstCrashY = c.y;

        // Discard carts that crashed.
        vector<Cart> okCarts;
        for (Cart& c : carts)
            if (!c.crash)
                okCarts.push_back(c);
        carts = okCarts;
    }

    // PART 1: What is the location of the first crash?  // 103,85
    cout << "[PART 1] = " << firstCrashX << "," << firstCrashY << endl;

    // PART 2: What is the location of the last cart at the end of the first
    // tick where it is the only cart left?  // 88,64
    cout << "[PART 2] = " << carts[0].x << "," << carts[0].y << endl;
}

// Main: Time the solver.
int main() {
    // Speed up C++ io by unsyncing with C stdio and untie cin/cout.
    ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    // Time the solver.
    auto start_time = std::chrono::high_resolution_clock::now();
    solve();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto ms_count = std::chrono::duration_cast<std::chrono::milliseconds>(
                        end_time - start_time)
                        .count();
    std::clog << "[RUN TIME: " << ms_count << "ms]" << std::endl;
}