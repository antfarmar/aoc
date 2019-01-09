// Advent of Code 2018
// Day 6: Chronal Coordinates
// https://adventofcode.com/2018/day/6

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
using namespace std;

struct Point {
    int x{0}, y{0}, area{0};
    int distance(int xx, int yy) { return abs(x - xx) + abs(y - yy); }
    friend std::istream& operator>>(std::istream& is, Point& p) {
        char comma;
        is >> p.x >> comma >> p.y;
        return is;
    }
};

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    // Input points
    vector<Point> pts{istream_iterator<Point>{cin}, {}};

    // Compute bounding box of points
    auto boundsX =
        minmax_element(pts.begin(), pts.end(),
                       [](Point& p1, Point& p2) { return p1.x < p2.x; });
    auto boundsY =
        minmax_element(pts.begin(), pts.end(),
                       [](Point& p1, Point& p2) { return p1.y < p2.y; });
    int minX = boundsX.first->x, maxX = boundsX.second->x;
    int minY = boundsY.first->y, maxY = boundsY.second->y;

    // For each grid location x,y find the closest input point.
    int regionSize = 0;
    for (int y = minY; y <= maxY; y++) {      // for each row
        for (int x = minX; x <= maxX; x++) {  // for each col
            int idx = 0, minDist = 1e9, totDist = 0;
            bool tied = false;
            for (size_t k = 0; k < pts.size(); k++) {  // for each point
                int curDist = pts[k].distance(x, y);
                totDist += curDist;     // part 2
                if (curDist < minDist)  // part 1
                    minDist = curDist, idx = k, tied = false;
                else if (curDist == minDist)
                    tied = true;
            }
            if (!tied)  // part 1
                ++pts[idx].area;
            if ((x == minX || x == maxX || y == minY || y == maxY))
                pts[idx].area = -1e9;  // set border points to -infinity
            if (totDist < 10000)       // part 2
                ++regionSize;
        }
    }

    // Part 1: Sort points by area to look for finite areas.
    // sort(pts.begin(), pts.end(),
    //      [](Point &p1, Point &p2) { return p1.area < p2.area; });
    // for (auto p : pts) {
    //     if ((minX < p.x && p.x < maxX) || (minY < p.y && p.y < maxY))
    //         cout << p.x << "\t" << p.y << "\t" << p.area << endl;
    // }
    // int largestArea = (pts.end() - 1)->area;
    int largestArea =
        max_element(pts.begin(), pts.end(),
                    [](Point& p1, Point& p2) { return p1.area < p2.area; })
            ->area;
    cout << "[Part 1] Largest Area = " << largestArea << endl;  // 3722
    cout << "[Part 2] Region Size  = " << regionSize << endl;   // 44634
}