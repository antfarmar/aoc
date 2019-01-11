// Advent of Code 2018
// Day 12: Subterranean Sustainability
// https://adventofcode.com/2018/day/12

// #include <array>
// #include <deque>
// #include <list>
// #include <map>
// #include <regex>
// #include <set>
// #include <sstream>
// #include <string>
// #include <tuple>
// #include <unordered_map>
#include <algorithm>
#include <bitset>
#include <chrono>
#include <iostream>
#include <numeric>
#include <unordered_set>
#include <vector>
using namespace std;

// Global constants.
const int initSize = 100;
const int ruleSize = 5;
const char potChar = '.';
const char plantChar = '#';

// Parse the input: Initial plant locations & evolution ruleset.
void parseInput(unordered_set<int>& plants,
                unordered_set<unsigned long>& rules) {
    // Parse the initial plant locations state into a bitset.
    string line;
    getline(cin, line);
    bitset<initSize> initialState(line, 15, 15 + initSize, potChar, plantChar);

    // Build set of plant locations from initial state.
    for (int loc = 0; loc < initSize; loc++)
        if (initialState[initSize - loc - 1])  // bitset indices are reversed
            plants.insert(loc);

    getline(cin, line);  // skip empty line

    // Parse the ruleset. Only keep rules that produce a new plant.
    while (getline(cin, line)) {
        bitset<ruleSize> rule(line.substr(0, ruleSize), 0, ruleSize, potChar,
                              plantChar);
        if (line[line.length() - 1] == plantChar)  // only keep birthing rules
            rules.insert(rule.to_ulong());
    }
}

// Cellular automaton of plants.
void solve() {
    // Set of plant locations and the evolution ruleset.
    unordered_set<int> plants;           // key is plant location
    unordered_set<int> newPlants;        // for the next generation
    unordered_set<unsigned long> rules;  // key is rule bitset as an int
    parseInput(plants, rules);

    long sum = 0, prevSum;        // for detecting growth stability
    long gen20Sum, gen50Sum;      // memo part1, part2 answers
    const int generations = 100;  // iteration maximum

    // Evolve the plant cellular automata.
    for (int gen = 1; gen <= generations; gen++) {
        // Find the plant location bounds.
        auto [minLoc, maxLoc] = minmax_element(plants.begin(), plants.end());

        // Debug: Draw the plants.
        bitset<initSize> draw;
        for (int loc = *minLoc; loc < initSize - *minLoc; loc++)
            draw[loc - *minLoc] = plants.count(loc);
        cerr << draw.to_string('.', '#');

        // Apply the evolution rules.
        newPlants.clear();        // clear previous generation
        bitset<ruleSize> region;  // to inspect plant locations for rule matches
        for (int loc = *minLoc - ruleSize; loc <= *maxLoc + ruleSize; loc++) {
            region <<= 1;
            region[0] = plants.count(loc);
            if (rules.count(region.to_ulong()))
                newPlants.insert(loc - (ruleSize / 2));
        }
        plants = newPlants;  // copy new generation

        // Part 1
        // After 20 generations, what is the sum of the numbers of
        // all pots which contain a plant?
        // Your puzzle answer was 2911
        if (gen == 20)
            gen20Sum = accumulate(plants.begin(), plants.end(), 0);

        // Debug: After 90 generations, the plant growth is constant = 50.
        prevSum = sum;
        sum = accumulate(plants.begin(), plants.end(), 0);
        cerr << "[Gen_" << gen << "] Size: " << plants.size() << " Sum: " << sum
             << " Delta: " << sum - prevSum << endl;
    }

    // Part 2
    // After fifty billion (50'000'000'000) generations, what is the sum of the
    // numbers of all pots which contain a plant?
    // Your puzzle answer was 2500000000695
    gen50Sum = accumulate(plants.begin(), plants.end(), 0);
    gen50Sum += (50'000'000'000 - generations) * (sum - prevSum);

    // Output the solutions.
    cout << "[Part 1]  = " << gen20Sum << endl;  // 2911
    cout << "[Part 2]  = " << gen50Sum << endl;  // 2500000000695
}

// Main: Time the solver.
int main() {
    // Speed up C++ io by unsyncing with C stdio and untie cin/cout.
    ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    auto start_time = chrono::high_resolution_clock::now();
    solve();
    auto end_time = chrono::high_resolution_clock::now();
    auto ms_count =
        chrono::duration_cast<chrono::milliseconds>(end_time - start_time)
            .count();
    clog << "[Run Time] = " << ms_count << "ms" << endl;
}