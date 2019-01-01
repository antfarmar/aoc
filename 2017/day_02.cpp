// #include <bits/stdc++.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <vector>
using namespace std;

int divisible(vector<int> &v) {
  for (auto &e1 : v)
    for (auto &e2 : v) {
      if (e1 > e2 && e1 % e2 == 0)
        return e1 / e2;
    }
  return 0;
}

void part1and2() {
  int checksum1{0};
  int checksum2{0};
  for (string line; getline(cin, line);) {
    istringstream iss(line);
    vector<int> v{istream_iterator<int>{iss}, {}};
    auto [min, max]{minmax_element(begin(v), end(v))};
    checksum1 += *max - *min;  // part 1
    checksum2 += divisible(v); // part 2
  }
  cout << "Checksum 1 = " << checksum1 << endl;
  cout << "Checksum 2 = " << checksum2 << endl;
}

int main() { part1and2(); }