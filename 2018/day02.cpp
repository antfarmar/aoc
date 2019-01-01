// Advent of Code 2018
// Day 2: Inventory Management System

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <unordered_set>
#include <vector>

// Count box id's with 2 and 3 of any letter.
int part1(std::vector<std::string> &data) {
    int dubs{0}, trip{0};
    for (std::string &line : data) {
        int cnt[26]{0};
        std::for_each(line.begin(), line.end(),
                      [&cnt](char &c) { ++cnt[c - 'a']; });
        dubs += std::any_of(std::begin(cnt), std::end(cnt),
                            [](int n) { return n == 2; })
                    ? 1
                    : 0;
        trip += std::any_of(std::begin(cnt), std::end(cnt),
                            [](int n) { return n == 3; })
                    ? 1
                    : 0;
    }
    return dubs * trip;
}

// Find the 2 box id's that differ by 1 character.
// Compare every id pair: O(n^2)
std::string part21(std::vector<std::string> &data) {
    for (auto line1 = data.begin(); line1 < data.end() - 1; ++line1) {
        for (auto line2 = line1 + 1; line2 < data.end(); ++line2) {
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
std::string part22(std::vector<std::string> &data) {
    std::sort(data.begin(), data.end());
    for (auto line1 = data.begin(); line1 < data.end() - 1; ++line1) {
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
std::string part23(std::vector<std::string> &data) {
    for (size_t i = 0; i < data.size(); i++) {
        std::unordered_set<std::string> set;
        for (auto &line : data) {
            std::string sub = line.substr(0, i) + line.substr(i + 1);
            if (!set.insert(sub).second)
                return sub;
        }
    }
    return "No solution.";
}

// Parse the box id's and solve.
int main() {
    std::vector<std::string> data{std::istream_iterator<std::string>{std::cin},
                                  {}};
    std::cout << "Part 1: Count = " << part1(data) << std::endl; // 6916
    std::cout << "Part 2: Match = " << part23(data)
              << std::endl; // oeylbtcxjqnzhgyylfapviusr
}