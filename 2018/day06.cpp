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

// an (x,y) point/coordinate in 2D space
// area = # of (x,y) locations that are closest to this point (and aren't tied
// in distance to any other point)
struct Point {
    int x, y;
    int area{0};  // the area around this point

    // manhattan distance (L1-norm)
    int distance(int xx, int yy) { return abs(x - xx) + abs(y - yy); }

    // parses a point from the input
    friend std::istream& operator>>(std::istream& is, Point& p) {
        char comma;
        is >> p.x >> comma >> p.y;
        return is;
    }
};

int main() {
    // parse the input points
    std::vector<Point> points{std::istream_iterator<Point>{std::cin}, {}};

    // compute bounding box of points
    auto x_bounds =
        std::minmax_element(points.begin(), points.end(),
                            [](Point& p1, Point& p2) { return p1.x < p2.x; });
    auto y_bounds =
        std::minmax_element(points.begin(), points.end(),
                            [](Point& p1, Point& p2) { return p1.y < p2.y; });

    int x_min = x_bounds.first->x;
    int y_min = y_bounds.first->y;
    int x_max = x_bounds.second->x;
    int y_max = y_bounds.second->y;

    // for each (x,y) grid location, find the closest input point
    int region_size = 0;
    int cutoff = 10000;
    for (int y = y_min; y <= y_max; y++)        // for each row
        for (int x = x_min; x <= x_max; x++) {  // for each col
            int min_dst = inf;
            int tot_dst = 0;
            int idx = 0;
            bool tied = false;
            for (size_t k = 0; k < points.size(); k++) {  // for each point
                int cur_dst = points[k].distance(x, y);
                tot_dst += cur_dst;     // part 2
                if (cur_dst < min_dst)  // part 1
                    min_dst = cur_dst, idx = k, tied = false;
                else if (cur_dst == min_dst)
                    tied = true;
            }
            if (not tied)  // part 1
                ++points[idx].area;
            if ((x == x_min || x == x_max || y == y_min || y == y_max))
                points[idx].area = -inf;  // set border points to -infinity
            if (tot_dst < cutoff)         // part 2
                ++region_size;
        }

    int largestArea =
        std::max_element(points.begin(), points.end(),
                         [](Point& p1, Point& p2) { return p1.area < p2.area; })
            ->area;
    std::cout << "[Part 1] Largest Area = " << largestArea
              << std::endl;  // 3722
    std::cout << "[Part 2] Region Size  = " << region_size
              << std::endl;  // 44634
}