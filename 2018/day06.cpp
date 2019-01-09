// Advent of Code 2018
// Day 6: Chronal Coordinates
// https://adventofcode.com/2018/day/6

#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>

// int infinity = INT_MAX = 2147483647 = 0x7fffffff
static const int inf = std::numeric_limits<int>::max();

// an (x,y) coordinate in 2D space (from the device)
// area: # of (x,y) locations that are closest to this coordinate (and aren't
// tied in distance to any other coordinate)
struct Coord {
    int x, y;
    int area{0};

    // manhattan distance (L1-norm)
    int distance(int xx, int yy) { return abs(x - xx) + abs(y - yy); }

    // parses a coordinate from the input stream
    friend std::istream& operator>>(std::istream& is, Coord& p) {
        char comma;
        is >> p.x >> comma >> p.y;
        return is;
    }
};

int main() {
    // parse all the input coordinates
    std::vector<Coord> coords{std::istream_iterator<Coord>{std::cin}, {}};

    // coordinate comparison lambdas
    auto x_cmp = [](Coord& p1, Coord& p2) { return p1.x < p2.x; };
    auto y_cmp = [](Coord& p1, Coord& p2) { return p1.y < p2.y; };
    auto a_cmp = [](Coord& p1, Coord& p2) { return p1.area < p2.area; };

    // determine the bounding box of the coordinates
    auto x_bounds = std::minmax_element(coords.begin(), coords.end(), x_cmp);
    auto y_bounds = std::minmax_element(coords.begin(), coords.end(), y_cmp);

    // extract the min coordinates
    int x_min = x_bounds.first->x;
    int y_min = y_bounds.first->y;

    // extract the max coordinates
    int x_max = x_bounds.second->x;
    int y_max = y_bounds.second->y;

    // for each (x,y) bounding box location, find the closest input coordinate
    const int cutoff = 10'000;  // part 2 condition
    int region_size = 0;        // part 2 solution memo

    // for each row and column of the bounding box
    for (int y = y_min; y <= y_max; y++)
        for (int x = x_min; x <= x_max; x++) {
            bool tied = false;
            int min_dst = inf;
            int tot_dst = 0;
            int idx = 0;

            // for each input coordinate
            for (size_t k = 0; k < coords.size(); k++) {
                int cur_dst = coords[k].distance(x, y);
                tot_dst += cur_dst;

                if (cur_dst > min_dst)
                    continue;

                // for part 1 solution
                tied = (cur_dst == min_dst);
                if (cur_dst < min_dst)
                    min_dst = cur_dst, idx = k;
            }

            // for part 1 solution
            if (not tied)
                coords[idx].area++;

            // set bordering coordinates' areas to -infinity (invalid)
            if (x == x_min || x == x_max || y == y_min || y == y_max)
                coords[idx].area = -inf;

            // for part 2 solution
            if (tot_dst < cutoff)
                region_size++;
        }

    // Part 1
    // What is the size of the largest area that isn't infinite?
    // Your puzzle answer was 3722
    int max_area = std::max_element(coords.begin(), coords.end(), a_cmp)->area;

    // Part 2
    // What is the size of the region containing all locations which have a
    // total distance to all given coordinates of less than 10000?
    // Your puzzle answer was 44634

    // output solutions
    std::cout << "Part 1: Largest Area = " << max_area << std::endl;   // 3722
    std::cout << "Part 2: Regn Size  = " << region_size << std::endl;  // 44634
}