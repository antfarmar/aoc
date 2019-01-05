// Advent of Code 2018
// Day 3: No Matter How You Slice It
// https://adventofcode.com/2018/day/3

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <unordered_map>
#include <vector>

// common code (to avoid duplicate code in each solution)
#include "common.hpp"

/*****************************************************************************/

// Claim id, x-offset, y-offset, width, height
struct Claim {
    int id, x, y, w, h;
    void mark() const;
    bool safe() const;
    static int to_key(int x, int y);
};

/*****************************************************************************/

// type aliases for convenience and readability
using Claims = std::vector<Claim>;
using FabricMap = std::unordered_map<int, int>;

/*****************************************************************************/

// global variable
// efficiently maps a fabric grid location to its "claim mark" count
// Key: bitwise-or of (x,y)
// Value: claim mark count
// ie. (x, y)(x+w, y+h) -> (# of overlapping claims)
FabricMap mark_count;

/*****************************************************************************/

// combine (x,y) locations into only 1 int (16-bits each)
int Claim::to_key(int x, int y) {
    return ((x << 15) | y);
}

// Mark a fabric grid location by adding to its "mark count"
void Claim::mark() const {
    for (int row = y; row < (y + h); row++)
        for (int col = x; col < (x + w); col++)
            mark_count[Claim::to_key(col, row)] += 1;
}

// Check if a claim is safe on a fabric grid by counting marks
// A safe claim has all of its marks == 1 (ie. no overlapping claims)
bool Claim::safe() const {
    for (int row = y; row < (y + h); ++row)
        for (int col = x; col < (x + w); ++col)
            if (mark_count.at(Claim::to_key(col, row)) > 1)
                return false;
    return true;
}

/*****************************************************************************/

// Parse claims appropriately
// for (std::string line; std::getline(is, line);)
//     sscanf(line.c_str(), "#%d @ %d,%d: %dx%d", &id, &x, &y, &w, &h)
std::istream& operator>>(std::istream& is, Claim& c) {
    char skip;           // #id @ x,y: wxh
    is >> skip           // #
        >> c.id >> skip  // @
        >> c.x >> skip   // ,
        >> c.y >> skip   // :
        >> c.w >> skip   // x
        >> c.h;
    return is;
}

/*****************************************************************************/

// Part 1
// How many square inches of fabric are within two or more claims?
// Solution: 115304
//
// Count overlapping fabric claims
int part1(const Claims& claims) {
    auto mark_claim = [](const Claim& claim) { claim.mark(); };
    auto overlaps = [](const auto& mark_cnt) { return mark_cnt.second > 1; };
    std::for_each(claims.cbegin(), claims.cend(), mark_claim);
    return std::count_if(mark_count.cbegin(), mark_count.cend(), overlaps);
}

// Part 2
// What is the ID of the only claim that doesn't overlap?
// Solution: 275
//
// Find the fabric claim location that doesn't overlap any other claim
int part2(const Claims& claims) {
    auto is_safe = [](const Claim& claim) { return claim.safe(); };
    auto&& safe_claim = std::find_if(claims.cbegin(), claims.cend(), is_safe);
    return safe_claim->id;
}

/*****************************************************************************/

int main() {
    const Claims claims = parse<Claim>();
    std::cout << "Part 1: Overlaps = " << part1(claims) << std::endl;  // 115304
    std::cout << "Part 2: Safe Id# = " << part2(claims) << std::endl;  // 275
}
