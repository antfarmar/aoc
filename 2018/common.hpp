/******************************************************************************
note to self: try to keep this file simple
******************************************************************************/
#pragma once

// #include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <vector>

/*****************************************************************************/

// housekeeping: unsync io (gotta go fast)
int _before_main() {
    std::cerr << "Unsyncing/untieing io streams...\n";
    std::ios_base::sync_with_stdio(0);  // unsync c++ streams (from c stdio)
    std::cin.tie(0);                    // unsync cin (from cout)
    return 0;
}
int __bm = _before_main();

/*****************************************************************************/

// parse input stream into a vector of type T
template <typename T>
std::vector<T> parse(std::istream& is = std::cin) {
    return std::vector<T>{std::istream_iterator<T>{is}, {}};
}

// print a vector of type T
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    std::copy(v.cbegin(), v.cend(), std::ostream_iterator<T>(os, " "));
    return os;
}

/*****************************************************************************/

// for output reporting
int _run_calls{0};
int _test_calls{0};
double _total_runtime{0};

// run or test a given part# of the solution
template <typename F, typename I, typename O>
struct runner {
    // runs & times the function on an input
    static void run(F& partf, const I& input) {
        _run_calls++;
        auto t1{std::chrono::high_resolution_clock::now()};
        auto&& solution{partf(input)};
        auto t2{std::chrono::high_resolution_clock::now()};
        std::chrono::duration<double, std::milli> ms{t2 - t1};
        report(solution, ms.count());
    }

    // reports the solutions and runtimes
    static void report(O solution, double runtime) {
        _total_runtime += runtime;
        if (_run_calls == 1)
            std::cout << "\n---------- Solutions ----------\n";
        std::cout << "Part " << _run_calls << ": " << solution;
        std::cout << " (" << runtime << "ms)" << std::endl;
        if (_run_calls >= 2)
            std::cout << "\nTotal time: " << _total_runtime << "ms\n";
    }

    // test the function on a vector of input/output pairs
    using test_case = std::pair<I, O>;
    using test_suite = std::vector<test_case>;
    static void test(F& partf, const test_suite& tsuite, bool verbose = true) {
        _test_calls++;
        auto report_test{[&](const auto& tcase, const auto& got) {
            const auto& [input, output]{tcase};
            std::cerr << "Testing part " << _test_calls << "...\n";
            std::cerr << "For: " << input << std::endl;
            std::cerr << "Exp: " << output << std::endl;
            std::cerr << "Got: " << got << std::endl << std::endl;
        }};
        auto run_test{[&](const auto& tcase) {
            const auto& [input, output]{tcase};
            const auto& got = partf(input);
            if (got != output)
                report_test(tcase, got), abort();
            if (verbose)
                report_test(tcase, got);
        }};
        // for_each(suite.cbegin(), suite.cend(), run_test);
        for (const auto& tcase : tsuite)
            run_test(tcase);
    }
};