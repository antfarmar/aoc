#include <algorithm>
#include <iostream>
using namespace std;

int to_int(char c) { return c - '0'; }

void part1(string in) {
  auto current = in.begin();
  int sum = (*current == *in.rbegin()) ? to_int(*current) : 0;

  for (;;) {
    current = std::adjacent_find(current, in.end());
    if (current == in.end())
      break;
    sum += to_int(*current++);
  }
  cout << "Sum 1: " << sum << endl;
}

void part2(string in) {
  int sum = 0;
  int full = in.length();
  int half = full / 2;

  for (int i = 0; i < full; ++i) {
    if (in[i] == in[(i + half) % full])
      sum += to_int(in[i]);
  }
  cout << "Sum 2: " << sum << endl;
}

int main() {
  string s;
  cin >> s;
  part1(s);
  part2(s);
}