// Advent of Code 2018
// Day 15: Beverage Bandits
// https://adventofcode.com/2018/day/15

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
using namespace std;

typedef pair<int, int> pii;

// Elves || Goblins
struct Unit {
    int x, y;              // location in cave
    int hp{200};           // hit points
    int ap{3};             // attack power
    char type;             // 'E'lf or 'G'oblin
    bool tookTurn{false};  // flag to indicate the unit moved in a round

    // constructor
    Unit(int x, int y, int t) : x(x), y(y), type(t){};

    // comparisons in reading order
    bool operator<(const Unit& other) {
        return make_pair(y, x) < make_pair(other.y, other.x);
    }
};

// GLOBAL CONSTANTS ////////////////////////////////////
queue<pii> Q;        // queue for bfs
vector<Unit> units;  // the elves/goblins

// The cave map data.
vector<string> cave{istream_iterator<string>{cin}, {}};
size_t rows{cave.size()};
size_t cols{cave[0].size()};

// Pathfinding data structures.
const int MAXN{32};
int DST[MAXN][MAXN];          // distance memos
bool VST[MAXN][MAXN];         // visited? for bfs
const int dx[]{0, 1, 0, -1};  // x/col adjacency
const int dy[]{-1, 0, 1, 0};  // y/row adjacency
////////////////////////////////////////////////////////

// BFS Path Reconstruction.
void reconstruct_path(pii cur, pii prev, pii goal, vector<pii>& result) {
    if (cur == goal) {
        result.push_back(prev);
        return;
    }
    int dist = DST[cur.first][cur.second];
    for (size_t d = 0; d < 4; d++) {  // check all adjacent locations
        pii ncur = make_pair(cur.first + dy[d], cur.second + dx[d]);
        if (ncur.first >= 0 && ncur.first < int(rows) && ncur.second >= 0 &&
            ncur.second < int(cols) && DST[ncur.first][ncur.second] == dist - 1)
            reconstruct_path(ncur, cur, goal, result);
    }
    return;
}

// BFS: Breadth First Search of the open cave locations.
// Returns a location adjacent to the weakest, nearest enemy (in reading order).
pii BFS(Unit& unit) {
    memset(DST, 0x3f3f3f, sizeof(DST));  // prepare for BFS
    memset(VST, 0, sizeof(VST));         // prepare for BFS

    // BFS: Breadth First Search the entire cave, record distances in DST.
    Q.push(make_pair(unit.y, unit.x));
    DST[unit.y][unit.x] = 0;
    while (!Q.empty()) {
        pii cur = Q.front();
        Q.pop();
        if (VST[cur.first][cur.second])
            continue;
        VST[cur.first][cur.second] = true;
        for (size_t d = 0; d < 4; d++) {  // check all adjacent locations
            pii ncur = make_pair(cur.first + dy[d], cur.second + dx[d]);
            if (ncur.first >= 0 && ncur.first < int(rows) && ncur.second >= 0 &&
                ncur.second < int(cols) &&
                cave[ncur.first][ncur.second] == '.' &&
                !VST[ncur.first][ncur.second]) {
                DST[ncur.first][ncur.second] = DST[cur.first][cur.second] + 1;
                Q.push(ncur);
            }
        }
    }

    // Find the shortest distance to the next closest enemy (in reading order).
    int minDst = 4 * MAXN;
    pii target = make_pair(MAXN, MAXN);
    for (Unit& u : units) {
        if (u.type == unit.type)  // skip allies
            continue;
        for (size_t d = 0; d < 4; d++) {  // check all adjacent locations
            pii cur = make_pair(u.y + dy[d], u.x + dx[d]);
            if (cur.first >= 0 && cur.first < int(rows) && cur.second >= 0 &&
                cur.second < int(cols) && VST[cur.first][cur.second]) {
                if (DST[cur.first][cur.second] < minDst) {
                    minDst = DST[cur.first][cur.second];
                    target = cur;
                } else if (DST[cur.first][cur.second] == minDst && cur < target)
                    target = cur;  // tie, select first in reading order
            }
        }
    }

    // If the Unit cannot reach (find an open path to) any of the squares that
    // are in range, it ends its turn. Return an invalid location to inform.
    if (minDst == 4 * MAXN)
        return pii{-1, -1};

    // Success. Return the location adjacent to the nearest enemy.
    vector<pii> minPath;
    reconstruct_path(target, make_pair(-1, -1), make_pair(unit.y, unit.x),
                     minPath);
    std::sort(minPath.begin(), minPath.end());
    return minPath[0];
}

// Return pointer to weakest,closest adjacent enemy to attack, if it exists.
Unit* adjEnemy(Unit& unit) {
    int minHP = 201;
    Unit* target = nullptr;
    // For each other Unit...
    for (Unit& u : units) {
        // skip ourself and already dead units.
        if (&u == &unit || u.hp < 0)
            continue;
        // ...find the weakest, closest adjacent enemy in reading order.
        for (size_t d = 0; d < 4; d++)  // check all adjacent locations
            if (unit.x + dx[d] == u.x && unit.y + dy[d] == u.y &&
                unit.type != u.type)
                if (u.hp < minHP || (u.hp == minHP && u < *target))
                    target = &u, minHP = u.hp;
    }
    return target;
}

// Take Unit U's turn for this round.
// 1. Try to move into range of an enemy (if it isn't already)
// 2. Attack the enemy (if it is in range/adjacent).
void takeTurn(Unit& unit) {
    unit.tookTurn = true;

    // Are we already adjacent to an enemy to attack?
    Unit* enemyToAttack = adjEnemy(unit);

    // Nope. We have to find & move towards the closest enemy.
    if (enemyToAttack == nullptr) {
        pii newLoc = BFS(unit);

        // If the Unit cannot reach (find an open path to) any of the squares
        // that are in range, it ends its turn.
        if (newLoc.first == -1)
            return;

        // Path found. Move to our new location.
        cave[newLoc.first][newLoc.second] = unit.type;
        cave[unit.y][unit.x] = '.';
        unit.y = newLoc.first, unit.x = newLoc.second;
        enemyToAttack = adjEnemy(unit);
    }

    // Attack the closest enemy we were at or just moved to.
    if (enemyToAttack != nullptr)
        enemyToAttack->hp -= unit.ap;
}

// Parse the units initial state in the input cave map. Return a copy.
vector<Unit> parseCaveMap() {
    for (size_t y = 0; y < rows; y++)
        for (size_t x = 0; x < cols; x++)
            if (cave[y][x] == 'G' || cave[y][x] == 'E') {
                Unit tmp(x, y, cave[y][x]);
                units.push_back(tmp);  // global scope
            }
    return units;
}

// Debug drawing of the entire cave map.
void drawCave() {
    string draw{string(32, '\n')};
    for (string& s : cave)
        draw += s + "\n";
    cerr << draw;
    this_thread::sleep_for(100ms);
}

// Straightforward battle simulations according to the specs.
void solve() {
    vector<string> initCave{cave};  // fresh copy of initial cave map
    vector<Unit> initUnits{
        parseCaveMap()};     // fresh copy of initial units state
    bool someElfDied{true};  // part 2: no elf deaths
    int elfAP{2};            // part 2: elf attack power
    int sumHPatAP3{-1};      // part 1: solution memo
    int round;               // multiplier for solutions

    // Simulate entire battles with increasingly stronger elves until none die.
    while (someElfDied) {
        // reinitialize state from initial copies
        cave = initCave;
        units = initUnits;
        someElfDied = false;
        elfAP++;
        // cerr << "elfAP:" << elfAP << endl;

        // Make the elves STRONGER!
        for (Unit& u : units)
            u.ap = (u.type == 'E') ? elfAP : u.ap;

        // Simulate a full battle at the current elf attack power.
        bool battleOver = false;
        round = -1;  // number of full rounds that were completed
                     // (not counting the round in which combat ends)
        while (!battleOver) {
            round++;  // new round

            //  No unit moved yet. Reset moved flags.
            for (Unit& unit : units)
                unit.tookTurn = false;

            // Units take turns in reading order. Top->Bot,Lft->Rgt
            std::sort(units.begin(), units.end());

        // Goto label: to reset the range-based for loop on surviving units.
        unitsTurnsBegin:
            // Simulate a full battle round. Each unit takes a turn in order.
            for (Unit& unit : units) {
                // Skip units that already took a turn this round.
                if (unit.tookTurn)
                    continue;

                // Are any enemies left to attack/move to?
                bool enemyLeft = false;
                for (Unit& u : units)
                    enemyLeft |= (u.type != unit.type);

                battleOver = (!enemyLeft || (someElfDied && elfAP > 3));
                if (battleOver)
                    break;

                drawCave();
                takeTurn(unit);  // move and attack if possible

                // Remove dead units from the list & cave. Check if an elf died.
                vector<Unit> survivors;
                for (Unit& u : units)
                    if (u.hp > 0)
                        survivors.push_back(u);
                    else {                     // unit died
                        cave[u.y][u.x] = '.';  // remove corpse from map
                        someElfDied =
                            (u.type == 'E');  // part 2: no elf deaths!
                    }

                units = survivors;     // copy surviving units
                goto unitsTurnsBegin;  // to reset range-based units iterator

            }  // end units turns

        }  // end round

        // Part 1 answer memo for first battle at elf attack power 3.
        if (elfAP == 3)
            sumHPatAP3 =
                round * accumulate(units.begin(), units.end(), 0,
                                   [](int sum, Unit& u) { return sum + u.hp; });
    }  // end battle

    // Part 1: Number of full rounds that were completed multiplied by the sum
    // of the hit points of all remaining units at the moment combat ends.
    cout << "[Part 01] = " << sumHPatAP3 << endl;  // 189910

    // Part 2: Find the outcome of the battle in which the Elves have the lowest
    // attack power (>3) that allows them to win without a single death.
    cout << "[Part 02] = ";  // 57820
    int sumHP = accumulate(units.begin(), units.end(), 0,
                           [](int sum, Unit& u) { return sum + u.hp; });
    cout << round * sumHP << endl;
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