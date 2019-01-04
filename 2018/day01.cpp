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

// type definitions for convenience and readability
typedef std::vector<int> frequencies;
typedef std::pair<int, int> solutions;

// print a vector of type T
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    std::copy(v.cbegin(), v.cend(), std::ostream_iterator<T>(os, " "));
    return os;
}

// Part 1
// Starting with a frequency of zero, what is the resulting frequency
// after all of the changes in frequency have been applied?
//
// just a simple fully accumulated sum of the frequency data
int part1(const frequencies& freqs) {
    return std::accumulate(freqs.cbegin(), freqs.cend(), 0);
}

// Part 2
// What is the first frequency your device reaches twice?
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

// return the solution pair to main
solutions solve(frequencies& freqs) {
    return {part1(freqs), part2(freqs)};
}

// testing helper
template <typename I, typename O>
struct tester {
    typedef std::pair<I, O> test_case;
    typedef std::vector<test_case> test_suite;
    template <typename F>
    static void test(F partf, test_suite suite, bool verbose = true) {
        auto report{[&](const auto& tcase, const auto& got) {
            const auto& [input, output]{tcase};
            std::cerr << "For: " << input << std::endl;
            std::cerr << "Exp: " << output << std::endl;
            std::cerr << "Got: " << got << std::endl << std::endl;
        }};
        auto run_test{[&](const auto& tcase) {
            const auto& [input, output]{tcase};
            const auto& got = partf(input);
            if (got != output)
                report(tcase, got), abort();
            if (verbose)
                report(tcase, got);
        }};
        for_each(suite.cbegin(), suite.cend(), run_test);
    }
};

// part 1 testsuite of the given examples
const tester<frequencies, int>::test_suite suite1{{{+1, -2, +3, +1}, 3},
                                                  {{+1, +1, +1}, 3},
                                                  {{+1, +1, -2}, 0},
                                                  {{-1, -2, -3}, -6}};

// part 2 testsuite of the given examples
const tester<frequencies, int>::test_suite suite2{{suite1[0].first, 2},
                                                  {{+1, -1}, 0},
                                                  {{+3, +3, +4, -2, -4}, 10},
                                                  {{-6, +3, +8, +5, -6}, 5},
                                                  {{+7, +7, -2, -7, -4}, 14}};

// test the examples, parse the input data, then solve
int main() {
    // housekeeping: speed up io (gotta go fast)
    std::ios_base::sync_with_stdio(0);  // unsync c++ streams (from c stdio)
    std::cin.tie(0);                    // unsync cin (from cout)

    // run some tests
    std::cerr << "Testing part 1 examples...\n";
    tester<frequencies, int>::test<decltype(part1)>(part1, suite1);
    std::cerr << "Testing part 2 examples...\n";
    tester<frequencies, int>::test<decltype(part2)>(part2, suite2);

    // parse the real input data
    std::cerr << "Parsing the input...\n";
    frequencies freqs{std::istream_iterator<int>{std::cin}, {}};

    // time the solver
    std::cerr << "Solving the challenge...\n\n";
    auto start_time{std::chrono::high_resolution_clock::now()};
    auto [part1, part2] = solve(freqs);
    auto end_time{std::chrono::high_resolution_clock::now()};

    // convert duration timescale from nanoseconds to milliseconds
    auto ms_count{std::chrono::duration_cast<std::chrono::milliseconds>(
                      end_time - start_time)
                      .count()};

    // output the runtime
    std::cerr << "After " << ms_count << "ms, we get..." << std::endl;

    // output the solutions
    std::cout << "\n--- Solutions ---\n";
    std::cout << "Part 1: " << part1 << std::endl;  // 520
    std::cout << "Part 2: " << part2 << std::endl;  // 394
}
