// Advent of Code 2018
// Day 20: A Regular Map
// https://adventofcode.com/2018/day/20

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>
using namespace std;

typedef pair<int, int> Room;        // coordinates (x,y)
map<Room, vector<Room>> RoomGraph;  // adjacency list

// Update room adjaceny lists and move to the new room
void connectRoomsAndMoveToNewRoom(Room& atRoom, const Room& newRoom) {
    RoomGraph[atRoom].push_back(newRoom);  // (atRoom) -> (newRoom)
    RoomGraph[newRoom].push_back(atRoom);  // (newRoom) -> (atRoom)
    atRoom = newRoom;                      // move to the new room
}

// Recursively parses the directions to build the rooms graph
void parseBuildGraph(string_view directions, size_t& dirIdx, Room atRoom) {
    auto b4Room = atRoom;   // memo for branching
    auto& [x, y] = atRoom;  // reference coordinates of this room

    // Iterate through the directions, recurse on branches (|)
    while (1) {
        switch (directions[dirIdx++]) {
            case 'N':
                connectRoomsAndMoveToNewRoom(atRoom, {x, y - 1});
                break;
            case 'E':
                connectRoomsAndMoveToNewRoom(atRoom, {x + 1, y});
                break;
            case 'S':
                connectRoomsAndMoveToNewRoom(atRoom, {x, y + 1});
                break;
            case 'W':
                connectRoomsAndMoveToNewRoom(atRoom, {x - 1, y});
                break;
            case '(':  // branching -> recursion
                parseBuildGraph(directions, dirIdx, atRoom);
                break;
            case '|':             // another possible branch
                atRoom = b4Room;  // go back to room before branch
                break;
            case ')':    // end branching recursion
                return;  // from last '('
            case '$':
                return;  // done (~EOF)
        }
    }
}

// Breadth-first search the rooms graph after parsing the directions
typedef pair<uintmax_t, uintmax_t> lengthPair;
lengthPair bfs(string_view directions, uintmax_t lengthLimit = 0) {
    RoomGraph.clear();  // multiple runs; global variable

    uintmax_t maxPathLength{0};  // part 1
    uintmax_t atLeastLength{0};  // part 2

    size_t dirIdx{1};                             // skip ^ char
    parseBuildGraph(directions, dirIdx, {0, 0});  // build adj list

    // bfs the rooms graph (edge adjaceny list)
    set<Room> visited;
    typedef pair<Room, uintmax_t> RoomPathLengths;
    queue<RoomPathLengths> Q;
    Q.push({{0, 0}, 0});  // starting location
    while (!Q.empty()) {
        auto [roomNode, pathLength] = Q.front();
        Q.pop();
        if (visited.count(roomNode))
            continue;
        if (pathLength >= lengthLimit)
            ++atLeastLength;
        visited.insert(roomNode);
        maxPathLength = max(pathLength, maxPathLength);
        for (auto roomEdge : RoomGraph[roomNode])
            Q.push({roomEdge, pathLength + 1});
    }
    return {maxPathLength, atLeastLength};
}

// Test suite of examples
void test(bool doTest) {
    if (!doTest)
        return;

    // suite of given sample examples
    typedef pair<string, uintmax_t> in_out;
    vector<in_out> suite = {
        {"^WNE$", 3},
        {"^ENWWW(NEEE|SSE(EE|N))$", 10},
        {"^ENNWSWW(NEWS|)SSSEEN(WNSE|)EE(SWEN|)NNN$", 18},
        {"^ESSWWN(E|NNENN(EESS(WNSE|)SSS|WWWSSSSE(SW|NNNE)))$", 23},
        {"^WSSEESWWWNW(S|NENNEEEENN(ESSSSW(NWSW|SSEN)|WSWWN(E|WWS(E|SS))))$",
         31}};

    // test the testsuite
    for (auto const& [input, output] : suite) {
        auto [got, _] = bfs(input);
        if (got != output) {
            cerr << "For: " << input << endl;
            cerr << "Exp: " << output << endl;
            cerr << "Got: " << got << endl;
            abort();
        }
    }
}

// Recursively parse the elf's "regex" directions
// Build an adjacency list of the graph as a map: (Room -> Room list)
// Do the breadth-first search (bfs) of the graph to get the solutions
void solve() {
    test(false);

    const uintmax_t lengthLimit{1000};  // part 2 path length min limit
    string directions;
    cin >> directions;
    auto [maxPathLength, atLeastLength] = bfs(directions, lengthLimit);

    // Part 1: What is the largest number of rooms you would be required to
    // pass through to reach a room? That is, find the room for which the
    // shortest path from your starting location to that room would require
    // passing through the most rooms; what is the fewest rooms you can
    // pass through to reach it?
    cout << "[Part 01] = " << maxPathLength << endl;  // 3958

    // Part 2: How many rooms have a shortest path from your current
    // location that pass through at least 1000 rooms?
    cout << "[Part 02] = " << atLeastLength << endl;  // 8566
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