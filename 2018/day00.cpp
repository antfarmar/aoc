// Advent of Code 2018
// Day 00:
// #include <bits/stdc++.h>
// #include <array>
// #include <bitset>
// #include <cassert>
// #include <deque>
// #include <list>
// #include <map>
// #include <numeric>
// #include <regex>
// #include <set>
// #include <sstream>
// #include <string>
// #include <tuple>
// #include <unordered_map>
// #include <unordered_set>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <istream>
#include <iterator>
#include <vector>
using namespace std;

// Test suite of examples
void test(bool doTest = false) {
    if (!doTest)
        return;
    cerr << "Testing..." << endl;

    // // suite of given sample examples
    // typedef pair<string, int> in_out;
    // vector<in_out> suite = {};

    // // test the testsuite
    // for (auto const &[input, output] : suite) {
    //     auto got = fn();
    //     if (got != output) {
    //         cerr << "For: " << input << endl;
    //         cerr << "Exp: " << output << endl;
    //         cerr << "Got: " << got << endl;
    //         abort();
    //     }
    // }
}

// std::istream &operator>>(std::istream &is, T &t) {
//     int ig = 99;
//     char ic;
//     cin.ignore(ig, '#');
//     is >> t >> ic;
//     return is;
// }

// Parse input via sscanf.
// vector<int> readInput() {
//     vector<int> v;
//     char form[] = "position=<%d>";
//     for (string line; getline(cin, line);) {
//         int n;
//         sscanf(line.c_str(), form, &n);
//         v.push_back(n);
//     }
//     return v;
// }

void solve() {
    test();
    // vector<string> data{istream_iterator<string>{cin}, {}};
    // vector<int> data = readInput();

    // int x;
    // while (cin >> x) {
    //     ;
    // }

    // for (string line; getline(cin, line);) {
    //     ;
    // }

    // sscanf("Text %d text  %d text", &var, &var);

    cout << "[Part 01] = " << 1 << endl;
    cout << "[Part 02] = " << 2 << endl;
}

// Main: Time the solver.
int main() {
    // Speed up C++ io by unsyncing with C stdio and untie cin/cout.
    ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    // Time the solve() function in milliseconds.
    auto start_time = std::chrono::high_resolution_clock::now();
    solve();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto ms_count = std::chrono::duration_cast<std::chrono::milliseconds>(
                        end_time - start_time)
                        .count();
    std::clog << "[Runtime] = " << ms_count << "ms" << std::endl;
}