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
    std::unordered_set<int> seen{sum};  // first sum is 0

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
void test_part(int part, bool verbose = false) {
    auto partf{(part == 1) ? part1 : part2};
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
        std::cerr << "Part " << part << std::endl;
        std::cerr << "For: " << input << std::endl;
        std::cerr << "Exp: " << output << std::endl;
        std::cerr << "Got: " << got << std::endl << std::endl;
    }};
    auto run_test{[&](const auto& tcase) {
        const auto& [input, output]{tcase};
        auto got = partf(input);
        if (got != output)
            report(tcase, got), abort();
        if (verbose)
            report(tcase, got);
    }};
    auto& examples{(partf == part1) ? part1_examples : part2_examples};
    for_each(examples.cbegin(), examples.cend(), run_test);
}

// test the examples, parse the input data, then solve
int main() {
    // housekeeping: speed up io (gotta go fast)
    std::ios_base::sync_with_stdio(0);  // unsync c++ streams (from c stdio)
    std::cin.tie(0);                    // unsync cin (from cout)

    // run some tests
    std::cerr << "Testing the examples...\n";
    test_part(1), test_part(2);

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
