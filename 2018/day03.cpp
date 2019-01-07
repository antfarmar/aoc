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
// #include "common.hpp"

/*****************************************************************************/

// a rectangular claim on the fabric
struct Claim {
    int id, x, y, w, h;  // id, x-offset, y-offset, width, height
};

/*****************************************************************************/

// parse claims appropriately via an istream
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

// parse via sscanf
// for (std::string line; std::getline(is, line);)
//     sscanf(line.c_str(), "#%d @ %d,%d: %dx%d", &id, &x, &y, &w, &h)

/*****************************************************************************/

// type aliases for convenience and readability
using Claims = std::vector<Claim>;
using LocationToClaimCount = std::unordered_map<int, int>;

/*****************************************************************************/

// Fabric with claims and claim location count records
struct Fabric {
    // all the elves claims to rectangular fabric locations
    Claims claims;

    // efficiently maps fabric locations to the # of claims there
    // Key: bitwise-or of (x,y) location
    // Value: # of claims on (x,y)
    // ((x<<15)|y) -> (# of claims on (x,y))
    LocationToClaimCount claims_on_location;

    // ctor
    Fabric(std::istream& is) {
        claims = {std::istream_iterator<Claim>{is}, {}};
        claim_the_fabric();
    };

    // add 1 to a fabric location's claim count for each location in each claim
    void claim_the_fabric() {
        auto claim_fabric_area = [&](const Claim& claim) {
            for (int row = claim.y; row < (claim.y + claim.h); row++)
                for (int col = claim.x; col < (claim.x + claim.w); col++)
                    claims_on_location[Fabric::loc2key(col, row)] += 1;
        };
        std::for_each(claims.cbegin(), claims.cend(), claim_fabric_area);
    };

    // combines (x,y) claim locations into a single int key (x[31:16] | y[15:0])
    static int loc2key(int x, int y) { return ((x << 15) bitor (y)); };
};

/*****************************************************************************/

// Part 1
// How many square inches of fabric are within two or more claims?
// Solution: 115304
//
// Count fabric locations with claim counts greater than or equal to 2
int part1(const Fabric& fabric) {
    auto gte_2 = [](const auto& loc_count) { return loc_count.second >= 2; };
    return std::count_if(fabric.claims_on_location.cbegin(),
                         fabric.claims_on_location.cend(), gte_2);
}

// Part 2
// What is the ID of the only claim that doesn't overlap?
// Solution: 275
//
// Find the only claim whose fabric location counts are all equal to 1
int part2(const Fabric& fabric) {
    auto no_overlaps = [&](const Claim& claim) {
        for (int row = claim.y; row < (claim.y + claim.h); row++)
            for (int col = claim.x; col < (claim.x + claim.w); col++)
                if (fabric.claims_on_location.at(Fabric::loc2key(col, row)) > 1)
                    return false;
        return true;
    };
    auto&& safe_claim =
        std::find_if(fabric.claims.cbegin(), fabric.claims.cend(), no_overlaps);
    return safe_claim->id;
}

/*****************************************************************************/

int main() {
    Fabric fabric(std::cin);
    std::cout << "Part 1: Overlaps = " << part1(fabric) << std::endl;  // 115304
    std::cout << "Part 2: Safe Id# = " << part2(fabric) << std::endl;  // 275
}
