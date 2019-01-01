// Advent of Code 2018
// Day 1: Chronal Calibration
// https://adventofcode.com/2018/day/1
#include <chrono>
#include <iostream>
#include <iterator>
#include <numeric>
#include <unordered_set>
#include <vector>

// type definitions for convenience and readability
typedef std::vector<int> frequencies;
typedef std::pair<int, int> solution;

// part 1
// starting with a frequency of zero, what is the resulting frequency
// after all of the changes in frequency have been applied?
//
// just a simple fully accumulated sum of the frequency data
int part1(frequencies& freqs) {
    return std::accumulate(freqs.cbegin(), freqs.cend(), 0);
}

// part 2
// what is the first frequency your device reaches twice?
//
// cycle through the partial sum frequencies until we get a recurrence
int part2(frequencies& freqs) {
    std::unordered_set<int> seen{};
    int idx{0}, sum{0};

    auto unique{[&](auto s) { return std::get<bool>(seen.insert(s)); }};
    auto partial_sums{[&](auto f) { return sum += f; }};
    auto cycle{[&](auto& v) { return v[idx++ % v.size()]; }};

    while (unique(partial_sums(cycle(freqs))))
        ;
    return sum;
}

// return the solution pair to main
solution solve(frequencies& freqs) {
    return {part1(freqs), part2(freqs)};
}

// parse the frequency data and solve
int main() {
    // speed up io (gotta go fast)
    std::ios_base::sync_with_stdio(false);  // unsync streams from stdio
    std::cin.tie(0);                        // untie cin from cout

    // parse the data
    frequencies freqs{std::istream_iterator<int>{std::cin}, {}};

    // time the solver
    auto start_time{std::chrono::high_resolution_clock::now()};
    auto [part1, part2] = solve(freqs);
    auto end_time{std::chrono::high_resolution_clock::now()};

    // convert time to milliseconds
    auto ms_count{std::chrono::duration_cast<std::chrono::milliseconds>(
                      end_time - start_time)
                      .count()};

    // output the solutions and runtime
    std::cout << "Part 1: " << part1 << std::endl;  // 520
    std::cout << "Part 2: " << part2 << std::endl;  // 394
    std::cerr << "Runtime: " << ms_count << "ms" << std::endl;
}
