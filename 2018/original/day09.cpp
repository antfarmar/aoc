// Advent of Code 2018
// Day 9: Marble Mania
// #include <regex>
// #include <numeric>
// #include <unordered_set>
// #include <unordered_map>
// #include <set>
// #include <map>
// #include <string>
// #include <deque>
// #include <array>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>
// using namespace std;

int main() {
    int numPlayers{10}, numMarbles{1618};
    scanf("%d players; last marble is worth %d points", &numPlayers,
          &numMarbles);

    // The marble circle: linked list.
    std::list<int> circle;
    circle.push_back(0);

    // Circular list iteration functions using lambdas.
    auto prev = [&](auto itr) {
        return (itr == circle.begin()) ? (itr = circle.end(), --itr) : --itr;
    };
    auto next = [&](auto itr) {
        return (++itr == circle.end()) ? circle.begin() : itr;
    };
    auto iterate = [&](auto itr, int dst) {
        while (dst)
            (dst < 0) ? (itr = prev(itr), ++dst) : (itr = next(itr), --dst);
        return itr;
    };
    auto erase = [&](auto itr) {
        itr = circle.erase(itr);
        return (itr == circle.end()) ? circle.begin() : itr;
    };

    std::vector<unsigned long long> players(numPlayers);
    // Calculate the winning Elf's score according to the game rules.
    auto score1{0ULL}, score2{0ULL};
    auto curPos = circle.begin();
    for (int marble = 1; marble < numMarbles * 100; ++marble) {
        if (marble % 23 == 0) {
            curPos = iterate(curPos, -7);
            players[marble % numPlayers] += *curPos + marble;
            curPos = erase(curPos);
        } else
            curPos = circle.insert(iterate(curPos, 2), marble);
        if (marble == numMarbles) // Part 1
            score1 = *std::max_element(players.begin(), players.end());
    }
    score2 = *std::max_element(players.begin(), players.end()); // Part 2

    // Part 1: What is the winning Elf's score? // 439635
    std::cout << "[Part 1] "
              << "Players: " << numPlayers << " Marbles: " << numMarbles
              << " Elf's Score = " << score1 << "\n";

    // Part 2: Winning Elf's score with numMarbles * 100 // 3562722971
    std::cout << "[Part 2] "
              << "Players: " << numPlayers << " Marbles: " << numMarbles
              << " Elf's Score = " << score2 << "\n";
}

// Circular iterator.
/* void rotate_iterator(std::list<uint32_t> &circle,
                     std::list<uint32_t>::iterator &itr, int n) {
    if (n > 0) {
        while (n-- > 0) {
            if (itr == circle.end())
                itr = circle.begin();
            std::advance(itr, 1);
        }

        if (itr == circle.end())
            itr = circle.begin();

    } else if (n < 0) {
        n = std::abs(n);
        while (n-- > 0) {
            if (itr == circle.begin())
                itr = circle.end();
            std::advance(itr, -1);
        }
    }
} */
