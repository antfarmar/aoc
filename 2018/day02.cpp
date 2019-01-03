// Advent of Code 2018
// Day 2: Inventory Management System
// https://adventofcode.com/2018/day/2

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <unordered_set>
#include <vector>

// type definitions for convenience and readability
typedef std::string box_id;
typedef std::vector<box_id> box_ids;
typedef std::pair<int, box_id> solutions;

// print a vector of type T
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    std::copy(v.cbegin(), v.cend(), std::ostream_iterator<T>(os, " "));
    return os;
}

// Part 1
// What is the checksum for your list of box IDs?
// checksum(ids) = #(letter_repeat(id)==2) x #(letter_repeat(id)==3)
//
// count box IDs that contain exactly 2 or 3 repeats of any letter
// product(count_if any_of count_of any_letter in id is in {2,3})
int part1(const box_ids& ids) {
    const box_id letters{"abcdefghijklmnopqrstuvwxyz"};
    const auto letter_repeat_counts_to_match = {2, 3};

    auto count_of_any_letter{[&](const auto& id, const auto& cnt) {
        return any_of(letters.cbegin(), letters.cend(), [&](const char& c) {
            return (cnt == std::count(id.cbegin(), id.cend(), c));
        });
    }};
    auto ids_with_exactly_n{[&](const int& n) {
        return std::count_if(ids.cbegin(), ids.cend(), [&](const auto& id) {
            return count_of_any_letter(id, n);
        });
    }};
    auto compute_checksum_from{[&](const auto& repeats_counts) {
        return std::accumulate(repeats_counts.begin(), repeats_counts.end(), 1,
                               [&](const int& product, const int& repeats) {
                                   return product * ids_with_exactly_n(repeats);
                               });
    }};
    return compute_checksum_from(letter_repeat_counts_to_match);
}

// Part 2
// What letters are common between the two correct box IDs?
// (the boxes will have IDs which differ by exactly one character at the same
// position in both strings)
//
// Find the 2 box ids that differ by 1 character at the same position
// Hash every box id substring in to a set and check for a match
// O(n*m) where m = string length of the ids, space complexity = O(n)
box_id part2(const box_ids& ids) {
    box_ids id_subs(ids.size());   // box id substrings to test
    box_id result{"No solution"};  // solution memo

    // duplicate id substring checking
    std::unordered_set<box_id> seen;
    auto unique{[&](const auto& s) {
        auto [itr, unique] = seen.insert(s);
        if (not unique)
            result = s;  // side-effect to get result from all_of
        return unique;
    }};

    // for every possible id character position
    // remove exactly 1 character at the same position and check for a match
    const size_t end_pos{ids[0].size()};
    for (size_t char_pos{0}; char_pos < end_pos; char_pos++) {
        // transformation lambda to remove a character at current position
        auto remove_char{[&](auto s) {
            return s.erase(char_pos, 1);
            // return s.substr(0, char_pos) + s.substr(char_pos + 1);
        }};
        std::transform(ids.cbegin(), ids.cend(), id_subs.begin(), remove_char);

        // check for an id substring match in this new id substring set
        seen.clear();
        if (not std::all_of(id_subs.cbegin(), id_subs.cend(), unique))
            break;
    }
    // non stl/algorithm version
    // for (size_t i = 0; i < ids[0].size(); i++) {
    //     std::unordered_set<box_id> set;
    //     for (auto& id : ids) {
    //         box_id sub = id.substr(0, i) + id.substr(i + 1);
    //         if (!set.insert(sub).second)
    //             return sub;
    //     }
    // }
    return result;
}

// return the solution pair to main
solutions solve(const box_ids& ids) {
    return {part1(ids), part2(ids)};
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
const tester<box_ids, int>::test_suite suite1{
    {{"abcdef", "bababc", "abbcde", "abcccd", "aabcdd", "abcdee", "ababab"},
     12}};

// part 2 testsuite of the given examples
const tester<box_ids, box_id>::test_suite suite2{
    {{"abcde", "fghij", "klmno", "pqrst", "fguij", "axcye", "wvxyz"}, "fgij"}};

// main
// test the examples, parse the input data, solve both parts, then report
int main() {
    // housekeeping: speed up io (gotta go fast)
    std::ios_base::sync_with_stdio(0);  // unsync c++ streams (from c stdio)
    std::cin.tie(0);                    // unsync cin (from cout)

    // run some tests
    std::cerr << "Testing part 1 examples...\n";
    tester<box_ids, int>::test<decltype(part1)>(part1, suite1);
    std::cerr << "Testing part 2 examples...\n";
    tester<box_ids, box_id>::test<decltype(part2)>(part2, suite2);

    // parse the real input data
    std::cerr << "Parsing the input...\n";
    const box_ids ids{std::istream_iterator<box_id>{std::cin}, {}};

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