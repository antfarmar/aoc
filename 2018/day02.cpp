// Advent of Code 2018
// Day 2: Inventory Management System
// https://adventofcode.com/2018/day/2

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <unordered_set>
#include <variant>
#include <vector>

// type definitions for convenience and readability
typedef std::string box_id;
typedef std::vector<box_id> box_ids;
typedef std::variant<int, box_id> result;
typedef std::pair<int, box_id> solutions;

// print a vector of type T
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    std::copy(v.cbegin(), v.cend(), std::ostream_iterator<T>(os, " "));
    return os;
}

// part 1
// what is the checksum for your list of box IDs?
//
// checksum = (#letters==2) x (#letters==3)
// count box IDs with exactly 2 or 3 of any letter
int part1(const box_ids& ids) {
    auto has_n_of_any_char{[&](const auto& s, const auto& n) {
        for (auto c = 'a'; c <= 'z'; c++)
            if (n == std::count(s.cbegin(), s.cend(), c))
                return true;
        return false;
    }};
    auto ids_with_exactly{[&](const int& n) {
        return std::count_if(ids.begin(), ids.end(), [&](auto& id) {
            return has_n_of_any_char(id, n);
        });
    }};
    return ids_with_exactly(2) * ids_with_exactly(3);
}
// int part1(box_ids& ids) {
//     int dubs{0}, trip{0};
//     for (box_id& id : ids) {
//         int cnt[26]{0};
//         std::for_each(id.begin(), id.end(),
//                       [&cnt](char& c) { ++cnt[c - 'a']; });
//         dubs += std::any_of(std::begin(cnt), std::end(cnt),
//                             [](int n) { return n == 2; })
//                     ? 1
//                     : 0;
//         trip += std::any_of(std::begin(cnt), std::end(cnt),
//                             [](int n) { return n == 3; })
//                     ? 1
//                     : 0;
//     }
//     return dubs * trip;
// }

// Find the 2 box id's that differ by 1 character.
// Compare every id pair: O(n^2)
box_id part21(box_ids& ids) {
    for (auto line1 = ids.begin(); line1 < ids.end() - 1; ++line1) {
        for (auto line2 = line1 + 1; line2 < ids.end(); ++line2) {
            if ([&line1, &line2](int diff) {
                    for (size_t i = 0; i < line1->length(); ++i)
                        if (line1->at(i) != line2->at(i) && ++diff > 1)
                            break;
                    return diff == 1;
                }(0))
                return *line1 + "\n" + *line2;
        }
    }
    return "No solution.";
}

// Find the 2 box id's that differ by 1 character.
// Sort id's and hope they are adjacent: O(n) + O(n log n)
box_id part22(box_ids& ids) {
    std::sort(ids.begin(), ids.end());
    for (auto line1 = ids.begin(); line1 < ids.end() - 1; ++line1) {
        if ([&line1](int diff) {
                auto line2 = line1 + 1;
                for (size_t i = 0; i < line1->length(); ++i)
                    if (line1->at(i) != line2->at(i) && ++diff > 1)
                        break;
                return diff == 1;
            }(0))
            return *line1 + "\n" + *++line1;
    }
    return "No solution.";
}

// Find the 2 box id's that differ by 1 character.
// Hash every box id substring and wait for a match.
// O(n*m) where m = string length of the ids, space complexity = O(n)
box_id part23(box_ids& ids) {
    for (size_t i = 0; i < ids.size(); i++) {
        std::unordered_set<box_id> set;
        for (auto& id : ids) {
            box_id sub = id.substr(0, i) + id.substr(i + 1);
            if (!set.insert(sub).second)
                return sub;
        }
    }
    return "No solution.";
}

// return the solution pair to main
solutions solve(box_ids& ids) {
    return {part1(ids), part23(ids)};
}

// test suite of given examples
void test_part(int part, bool verbose = true) {
    auto partf{(part == 1) ? part1 : part1};
    typedef std::pair<box_ids, int> test_case;
    typedef std::vector<test_case> test_suite;
    const test_suite part1_examples{
        {{"abcdef", "bababc", "abbcde", "abcccd", "aabcdd", "abcdee", "ababab"},
         12}};
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
    auto& examples{(partf == part1) ? part1_examples : part1_examples};
    for_each(examples.cbegin(), examples.cend(), run_test);
}

// test the examples, parse the input data, then solve
int main() {
    // housekeeping: speed up io (gotta go fast)
    std::ios_base::sync_with_stdio(0);  // unsync c++ streams (from c stdio)
    std::cin.tie(0);                    // unsync cin (from cout)

    // run some tests
    std::cerr << "Testing the examples...\n";
    test_part(1);

    // parse the real input data
    std::cerr << "Parsing the input...\n";
    box_ids ids{std::istream_iterator<box_id>{std::cin}, {}};

    // time the solver
    std::cerr << "Solving the challenge...\n\n";
    auto start_time{std::chrono::high_resolution_clock::now()};
    auto [part1, part2] = solve(ids);
    auto end_time{std::chrono::high_resolution_clock::now()};

    // convert duration timescale from nanoseconds to milliseconds
    auto ms_count{std::chrono::duration_cast<std::chrono::milliseconds>(
                      end_time - start_time)
                      .count()};

    // output the runtime
    std::cerr << "After " << ms_count << "ms, we get..." << std::endl;

    // output the solutions
    std::cout << "\n--- Solutions ---\n";
    std::cout << "Part 1: " << part1 << std::endl;  // 6916
    std::cout << "Part 2: " << part2 << std::endl;  // oeylbtcxjqnzhgyylfapviusr
}