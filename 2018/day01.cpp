// Advent of Code 2018
// Day 1: Chronal Calibration
// https://adventofcode.com/2018/day/1
#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <iterator>
#include <numeric>
#include <unordered_set>
#include <vector>

// type definitions for convenience and readability
typedef std::vector<int> frequencies;
typedef std::pair<int, int> solutions;

// print a vector of ints (frequencies)
std::ostream& operator<<(std::ostream& os, const frequencies& f) {
    std::copy(f.cbegin(), f.cend(), std::ostream_iterator<int>(os, " "));
    return os;
}

// part 1
// starting with a frequency of zero, what is the resulting frequency
// after all of the changes in frequency have been applied?
//
// just a simple fully accumulated sum of the frequency data
int part1(const frequencies& freqs) {
    return std::accumulate(freqs.cbegin(), freqs.cend(), 0);
}

// part 2
// what is the first frequency your device reaches twice?
//
// cycle through the partial sum frequencies until we get a recurrence
int part2(const frequencies& freqs) {
    int idx{0}, sum{0};
    std::unordered_set<int> seen{0};

    auto cycle{[&](auto& v) { return v[idx++ % v.size()]; }};
    auto partial_sums{[&](auto f) { return sum += f; }};
    auto unique{[&](auto s) { return std::get<bool>(seen.insert(s)); }};

    while (unique(partial_sums(cycle(freqs))))
        ;
    return sum;
}

// return the solution pair to main
solutions solve(frequencies& freqs) {
    return {part1(freqs), part2(freqs)};
}

// test suite of given examples
void test(int (*partf)(const frequencies&), bool verbose = true) {
    typedef std::pair<frequencies, int> test_case;
    typedef std::vector<test_case> test_suite;
    const test_suite part1_examples{{{+1, -2, +3, +1}, 3},
                                    {{+1, +1, +1}, 3},
                                    {{+1, +1, -2}, 0},
                                    {{-1, -2, -3}, -6}};
    const test_suite part2_examples{{part1_examples[0].first, 2},
                                    {{+1, -1}, 0},
                                    {{+3, +3, +4, -2, -4}, 10},
                                    {{-6, +3, +8, +5, -6}, 5},
                                    {{+7, +7, -2, -7, -4}, 14}};
    auto report{[&](const auto& tcase, const auto& got) {
        const auto& [input, output]{tcase};
        std::cerr << "For: " << input << std::endl;
        std::cerr << "Exp: " << output << std::endl;
        std::cerr << "Got: " << got << std::endl << std::endl;
    }};
    auto run_test{[&](const auto& tcase) {
        const auto& [input, output] = tcase;
        auto got = partf(input);
        if (verbose)
            report(tcase, got);
        assert(got == output);
    }};
    auto& examples{(partf == part1) ? part1_examples : part2_examples};
    if (verbose)
        std::cerr << "Testing part " << (partf != part1) + 1 << std::endl;
    for_each(examples.cbegin(), examples.cend(), run_test);
}

// parse the frequency data and solve
int main() {
    // speed up io (gotta go fast)
    std::ios_base::sync_with_stdio(0);  // unsync c++ streams (from c stdio)
    std::cin.tie(0);                    // unsync cin (from cout)

    // parse the data
    frequencies freqs{std::istream_iterator<int>{std::cin}, {}};

    // run some tests
    test(part1), test(part2);

    // time the solver
    auto start_time{std::chrono::high_resolution_clock::now()};
    auto [part1, part2] = solve(freqs);
    auto end_time{std::chrono::high_resolution_clock::now()};

    // convert time to milliseconds
    auto ms_count{std::chrono::duration_cast<std::chrono::milliseconds>(
                      end_time - start_time)
                      .count()};

    // output the solutions
    std::cout << "--- Solutions ---" << std::endl;
    std::cout << "Part 1: " << part1 << std::endl;  // 520
    std::cout << "Part 2: " << part2 << std::endl;  // 394

    // output the runtime
    std::cerr << "Runtime: " << ms_count << "ms" << std::endl;
}
