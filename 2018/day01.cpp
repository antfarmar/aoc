// Advent of Code 2018
// Day 1: Chronal Calibration
// https://adventofcode.com/2018/day/1

#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <numeric>
#include <unordered_set>
#include <vector>

// common code (to avoid duplicate code in each solution)
#include "common.hpp"

/*****************************************************************************/

// type aliases for convenience and readability
using frequencies = std::vector<int>;

// function definitions of the solutions
int part1(const frequencies& freqs);
int part2(const frequencies& freqs);

// more type aliases (for runner template)
using runner1 = runner<decltype(part1), frequencies, int>;

/*****************************************************************************/
// testsuite of the given examples for part 1
const runner1::test_suite suite1{{{+1, -2, +3, +1}, 3},
                                 {{+1, +1, +1}, 3},
                                 {{+1, +1, -2}, 0},
                                 {{-1, -2, -3}, -6}};

// testsuite of the given examples for part 2
const runner1::test_suite suite2{{suite1[0].first, 2},
                                 {{+1, -1}, 0},
                                 {{+3, +3, +4, -2, -4}, 10},
                                 {{-6, +3, +8, +5, -6}, 5},
                                 {{+7, +7, -2, -7, -4}, 14}};
/*****************************************************************************/

// main: test the examples, parse the input data, solve both parts, then report
int main() {
    // run example tests
    std::cerr << "Running the tests...\n";
    runner1::test(part1, suite1);
    runner1::test(part2, suite2);

    // parse the input data
    std::cerr << "Parsing the input...\n";
    const frequencies ids{std::istream_iterator<int>{std::cin}, {}};

    // run, time, and output the solutions
    std::cerr << "Solving the challenge...\n";
    runner1::run(part1, ids);
    runner1::run(part2, ids);
}

/*****************************************************************************/

// Part 1
// Starting with a frequency of zero, what is the resulting frequency
// after all of the changes in frequency have been applied?
// Solution: 520
//
// just a simple fully accumulated sum of the frequency data
int part1(const frequencies& freqs) {
    return std::accumulate(freqs.cbegin(), freqs.cend(), 0);
}

// Part 2
// What is the first frequency your device reaches twice?
// Solution: 394
//
// cycle through the partial sum frequencies until we get a recurrence
int part2(const frequencies& freqs) {
    int index{0}, sum{0};
    std::unordered_set<int> seen{sum};  // first sum is 0

    auto cycle{[&](const auto& v) { return v[index++ % v.size()]; }};
    auto partial_sums{[&](const auto& f) { return sum += f; }};
    auto unique{[&](const auto& s) { return std::get<bool>(seen.insert(s)); }};

    while (unique(partial_sums(cycle(freqs))))
        ;
    return sum;
}