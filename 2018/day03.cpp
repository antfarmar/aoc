// Advent of Code 2018
// Day 3: No Matter How You Slice It
// https://adventofcode.com/2018/day/3

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <unordered_map>
#include <vector>

struct Claim {
    int id, x, y, w, h;  // Claim id, x-offset, y-offset, width, height.

    // Mark a fabric grid location by adding to its mark count.
    // Key: bitwise-or of x,y.
    // Value: mark count.
    void mark(std::unordered_map<int, int>& grid) const {
        for (int i = x; i < x + w; ++i)
            for (int j = y; j < y + h; ++j)
                ++grid[(i << 15) | j];
    }

    // Check if a claim is safe on a fabric grid by counting marks.
    // A safe claim has all of its marks == 1. i.e. no overlappin claims.
    bool safe(std::unordered_map<int, int> const& grid) const {
        for (int i = x; i < x + w; ++i)
            for (int j = y; j < y + h; ++j)
                if (grid.at((i << 15) | j) > 1)
                    return false;
        return true;
    }

    // Parse claims appropriately.
    friend std::istream& operator>>(std::istream& is, Claim& c) {
        char skip;           // #id @ x,y: wxh
        is >> skip           // #
            >> c.id >> skip  // @
            >> c.x >> skip   // ,
            >> c.y >> skip   // :
            >> c.w >> skip   // x
            >> c.h;
        return is;
    }
    // for (std::string line; std::getline(is, line);)
    //     sscanf(line.c_str(), "#%d @ %d,%d: %dx%d", &id, &x, &y, &w, &h)
};

// Count overlapping fabric claims.
int part1(std::vector<Claim>& claims, std::unordered_map<int, int>& fabric) {
    std::for_each(claims.begin(), claims.end(),
                  [&fabric](Claim& claim) { claim.mark(fabric); });
    int overlaps =
        std::count_if(fabric.begin(), fabric.end(),
                      [](auto& mark_cnt) { return mark_cnt.second > 1; });
    return overlaps;
}

// Find the fabric claim that doesn't overlap any other claim.
int part2(std::vector<Claim>& claims, std::unordered_map<int, int>& fabric) {
    auto safe_claim =
        std::find_if(claims.begin(), claims.end(),
                     [&fabric](Claim& claim) { return claim.safe(fabric); });
    return safe_claim->id;
}

// Parse stream input into a vector of type T.
template <class T>
std::vector<T> parse(std::istream& is = std::cin) {
    return std::vector<T>{std::istream_iterator<T>{is}, {}};
}

// Solve the problem.
int main() {
    std::vector<Claim> claims = parse<Claim>();
    std::unordered_map<int, int> fabric;
    std::cout << "Part 1: Overlaps = " << part1(claims, fabric)
              << std::endl;  // 115304
    std::cout << "Part 2: Safe Id# = " << part2(claims, fabric)
              << std::endl;  // 275
}
