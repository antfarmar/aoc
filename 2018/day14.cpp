// Advent of Code 2018
// Day 14: Chocolate Charts
// https://adventofcode.com/2018/day/14

#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>
using namespace std;

// Straightforward scoreboard vector implementation.
void solve() {
    size_t elf0{0}, elf1{1};    // elf bakers/index into recipe scoreboard
    uint32_t scoreCnt{554401};  // input: number of recipes
    vector<uint8_t> targetScores{5, 5, 4, 4, 0, 1};  // target score sequence
    vector<uint8_t> scoreboard{3, 7};                // elf recipe scoreboard

    // Parse the input: number of recipes. Digits are target score sequence.
    cin >> scoreCnt, targetScores.clear();
    for (char& c : to_string(scoreCnt))
        targetScores.push_back(uint8_t(c - '0'));

    // Writes a score digit to the board and checks for a target sequence match.
    auto writeScoreCheck = [&](uint8_t digit) -> bool {
        scoreboard.push_back(digit);
        return equal(targetScores.crbegin(), targetScores.crend(),
                     scoreboard.crbegin());
    };

    // Score the recipes according to the elves criteria until we target match.
    for (bool match{false}; !match;) {
        uint8_t newScore = scoreboard[elf0] + scoreboard[elf1];
        match |= (newScore >= 10) ? writeScoreCheck(newScore / 10) : match;
        match |= (match) ? match : writeScoreCheck(newScore % 10);
        elf0 += scoreboard[elf0], ++elf0 %= scoreboard.size();
        elf1 += scoreboard[elf1], ++elf1 %= scoreboard.size();
    }

    // Part 1: What are the scores of the ten recipes immediately after the
    // number of recipes in your puzzle input? // 3610281143
    cout << "[Part 01] = ";
    for_each(scoreboard.begin() + scoreCnt, scoreboard.begin() + scoreCnt + 10,
             [](uint8_t& score) { cout << uint16_t(score); });
    cout << endl;

    // Part 2: How many recipes appear on the scoreboard to the left of the
    // score sequence in your puzzle input? // 20211326
    cout << "[Part 02] = " << scoreboard.size() - targetScores.size() << endl;
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