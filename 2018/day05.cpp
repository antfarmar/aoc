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
    // *top != *cur && (toupper(*top) == toupper(*cur))) toupper(c) == c & 0xDF
    auto react = [](polymer& poly) {  // uses reverse iterator for efficiency
        size_t size = poly.size();    // start size (original)
        poly.push_back('^');          // top of the stack sentinel (temporary)
        auto top = std::next(poly.rbegin());  // ie. '^' pushed on stack
        for (auto cur = std::next(top); cur != poly.rend(); cur++)
            if ((*top ^ *cur) == 32)  // opposite cases of same letter
                --top, size -= 2;     // pop, dec size by match size of 2
            else
                std::iter_swap(++top, cur);  // push, and advance
        poly.pop_back();  // restore the polymer back to its original state
        return size;
    };
    size_t part1_size = react(the_polymer);

    // Part 2
    // What is the length of the shortest polymer you can produce by removing
    // all units of exactly one type and fully reacting the result?
    // Solution: 5124

    // Remove all units of exactly one type and react the result, once per unit
    static char units[26];                               // lowercase alphabet
    std::iota(std::begin(units), std::end(units), 'a');  // a-z

    // for (char unit = 'a'; unit <= 'z'; unit++) {
    size_t part2_size = SIZE_MAX;
    for (const char& unit : units) {
        polymer poly = the_polymer;  // work on a copy since we mutate
        auto eq_un = [&](const char& c) { return unit == tolower(c); };
        // char zip[2] = {unit, char(toupper(unit))};  // e.g. {a,A}
        // for (char& cC : zip)
        poly.erase(std::remove_if(poly.begin(), poly.end(), eq_un), poly.end());
        part2_size = std::min(part2_size, react(poly));
    }

    // output the solutions
    std::cout << "Part 1: " << part1_size << "\n"   // 11546
              << "Part 2: " << part2_size << "\n";  // 5124
}