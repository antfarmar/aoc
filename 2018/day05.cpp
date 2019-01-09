// Advent of Code 2018
// Day 5: Alchemical Reduction
// https://adventofcode.com/2018/day/5

#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <string>

// type aliases for convenience and readability
using polymer = std::string;

int main() {
    polymer the_polymer;
    std::getline(std::cin, the_polymer);  // std::cin >> the_polymer;

    // Part 1
    // How many units remain after fully reacting the polymer you scanned?
    // Solution: 11546

    // Fully react the polymer: remove adjacent pairs from aA to zZ
    // Simulate a stack with pointers and swap units in place
    // abs(A-a) == xor(A,a) == A^a == 0x20 == 32
    // *top != *cur && (toupper(*top) == toupper(*cur))) toupper(x) == c & 0xDF
    auto react = [](auto&& poly) {
        auto total = poly.size();
        for (auto top = poly.begin(), cur = top + 1; cur != poly.end();)
            if ((*top ^ *cur) == 0x20)
                cur++, (top == poly.begin()) ? top : top--, total -= 2;
            else
                std::swap(*(++top), *(cur++));
        return total;
    };
    size_t size1 = react(the_polymer);

    // Part 2
    // What is the length of the shortest polymer you can produce by removing
    // all units of exactly one type and fully reacting the result?
    // Solution: 5124

    // Remove all units of exactly one type and react the result, once per unit
    char units[26];
    std::iota(std::begin(units), std::end(units), 'a');  // a-z
    size_t size2 = SIZE_MAX;
    for (char& unit : units) {  // for (char unit = 'a'; unit <= 'z'; unit++) {
        auto poly = the_polymer;  // work on a copy since we mutate the polymer
        auto zip = std::array{unit, char(toupper(unit))};  // {a,A}
        for (char& c : zip)
            poly.erase(std::remove(poly.begin(), poly.end(), c), poly.end());
        size2 = std::min(size2, react(poly));
    }

    // Output the solutions.
    std::cout << "Part 1: " << size1 << "\n"   // 11546
              << "Part 2: " << size2 << "\n";  // 5124
}