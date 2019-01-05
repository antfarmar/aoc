#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <vector>

// print a vector of type T
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    std::copy(v.cbegin(), v.cend(), std::ostream_iterator<T>(os, " "));
    return os;
}

// runs or tests a given part of the solution
template <typename F, typename I, typename O>
struct runner {
    // runs & times the function on an input
    static void run(F& partf, const I& input) {
        auto t1{std::chrono::high_resolution_clock::now()};
        auto&& solution{partf(input)};
        auto t2{std::chrono::high_resolution_clock::now()};
        std::chrono::duration<double, std::milli> ms{t2 - t1};
        std::cout << "Part 1: " << solution;
        std::cerr << " (" << ms.count() << "ms)" << std::endl;
    }

    // test the function on a vector of input/output pairs
    using test_case = std::pair<I, O>;
    using test_suite = std::vector<test_case>;
    static void test(F& partf, const test_suite& suite, bool verbose = true) {
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