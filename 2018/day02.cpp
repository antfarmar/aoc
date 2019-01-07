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

// common code (to avoid duplicate code in each solution)
#include "common.hpp"

/*****************************************************************************/

// type aliases for convenience and readability
using box_id = std::string;
using box_ids = std::vector<box_id>;

// function definitions of the solutions
int part1(const box_ids& ids);
box_id part2(const box_ids& ids);

// more type aliases (for runner template)
using runner1 = runner<decltype(part1), box_ids, int>;
using runner2 = runner<decltype(part2), box_ids, box_id>;

/*****************************************************************************/

// testsuite of the given examples for part 1
const runner1::test_suite suite1{
    {{"abcdef", "bababc", "abbcde", "abcccd", "aabcdd", "abcdee", "ababab"},
     12}};

// testsuite of the given examples for part 2
const runner2::test_suite suite2{
    {{"abcde", "fghij", "klmno", "pqrst", "fguij", "axcye", "wvxyz"}, "fgij"}};

/*****************************************************************************/

// main: test the examples, parse the input data, solve both parts, then report
int main() {
    // run example tests
    std::cerr << "Running the tests...\n";
    runner1::test(part1, suite1);
    runner2::test(part2, suite2);

    // parse the input data
    std::cerr << "Parsing the input...\n";
    const box_ids ids{std::istream_iterator<box_id>{std::cin}, {}};

    // run, time, and output the solutions
    std::cerr << "Solving the challenge...\n";
    runner1::run(part1, ids);
    runner2::run(part2, ids);
}

/*****************************************************************************/

// Part 1
// What is the checksum for your list of box IDs?
// Solution: 6916
//
// checksum(ids) = #(letter_repeat(id)==2) x #(letter_repeat(id)==3)

// count box IDs that contain exactly 2 or 3 repeats of any letter
// product(count_if any_of count_of any_letter in id is in {2,3})
int part1(const box_ids& ids) {
    // const box_id alpha{"abcdefghijklmnopqrstuvwxyz"};
    const auto letter_repeat_counts_to_match = {2, 3};
    box_id alpha(26, '?');
    std::iota(alpha.begin(), alpha.end(), 'a');

    auto count_of_any_letter{[&](const auto& id, const auto& cnt) {
        return any_of(alpha.cbegin(), alpha.cend(), [&](const char& c) {
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

/*****************************************************************************/

// Part 2
// What letters are common between the two correct box IDs?
// Solution: oeylbtcxjqnzhgyylfapviusr
//
// (the boxes will have IDs which differ by exactly one character at the same
// position in both strings)

// Find the 2 box ids that differ by 1 character at the same position by
// using the inner_product (ordered map/reduce) of id 2-combination pairs
// (!=) not_eq map, (+) sum reduce
box_id part2(const box_ids& ids) {
    box_id common_chars{"No solution"};  // solution memo

    // for each id 2-combination (pair)...
    for (const box_id& id1 : ids)
        for (const box_id& id2 : ids) {
            if (id1 == id2)
                continue;  // skip same

            // (+) reduce on (!=) map b/w each (id1[i], id2[i]) char pair
            int diff_cnt =
                std::inner_product(id1.begin(), id1.end(), id2.begin(), 0,
                                   std::plus<>{}, std::not_equal_to<>{});

            // if only 1 character difference, done
            if (diff_cnt == 1) {
                common_chars = id1;
                auto [itr1, itr2] = std::mismatch(
                    common_chars.begin(), common_chars.end(), id2.begin());
                common_chars.erase(itr1);
                return common_chars;
            }
        }
    return common_chars;
}

// Part 2_2 (using a hash set)
// Hash every box id substring (of size-1) in to a set and check for a match
// O(n*m) where m = string length of the ids, space complexity = O(n)
box_id part2_2(const box_ids& ids) {
    box_ids id_subs(ids.size());         // box id substrings to test
    box_id common_chars{"No solution"};  // solution memo

    // duplicate id substring checking
    std::unordered_set<box_id> seen;
    auto unique{[&](const auto& s) {
        auto [itr, unique] = seen.insert(s);
        if (not unique)
            common_chars = s;  // side-effect to get result from all_of
        return unique;
    }};

    // for every possible id character position in all ids...
    // remove exactly 1 character at the same positions and check for a match
    const size_t end_pos{ids[0].size()};
    for (size_t char_pos{0}; char_pos < end_pos; char_pos++) {
        // transformation lambda to remove a character at current position
        auto remove_char{[&](auto s) {
            s.erase(char_pos, 1);
            return s;
            // return s.substr(0, char_pos) + s.substr(char_pos + 1);
        }};
        std::transform(ids.cbegin(), ids.cend(), id_subs.begin(), remove_char);

        // check for an id substring match in this new id substring set
        seen.clear();
        if (not std::all_of(id_subs.cbegin(), id_subs.cend(), unique))
            break;
    }
    return common_chars;
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
