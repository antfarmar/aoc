// Advent of Code 2018
// Day 1: Chronal Calibration

#include <iostream>
#include <iterator>
#include <numeric>
#include <unordered_set>
#include <vector>

// Sum the frequency data.
int part1(std::vector<int> &data) {
    return std::accumulate(data.begin(), data.end(), 0);
}

// Cycle through the frequencies until we get a sum match.
int part2(std::vector<int> &data) {
    int i{0}, sum{0}, size = data.size();
    std::unordered_set<int> memo{};
    while (memo.insert(sum += data[i++ % size]).second)
        ;
    return sum;
}

// Parse the frequency data and solve.
int main() {
    std::vector<int> data{std::istream_iterator<int>{std::cin}, {}};
    std::cout << "Part 1: Sum = " << part1(data) << std::endl; // 520
    std::cout << "Part 2: Hit = " << part2(data) << std::endl; // 394
}
