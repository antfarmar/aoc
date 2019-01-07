// Advent of Code 2018
// Day 04: Repose Record
// https://adventofcode.com/2018/day/4

#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

int main() {
    // Scan the date record event lines and sort them
    std::vector<std::string> events;
    for (std::string ln; std::getline(std::cin, ln);)
        events.push_back(ln);
    std::sort(events.begin(), events.end());  // chronological dates

    // Map guard ids to their sleep schedule array of minutes.
    std::unordered_map<int, std::array<int, 60>> guards;
    int* minutes_0 = nullptr;
    int sleep_at = -1;
    std::smatch match;

    // Parse the timestamps and compute all guard sleep schedules.
    for (const auto& line : events) {
        if (std::regex_search(line, match,
                              std::regex(R"(.*Guard #(\d+) begins shift)")))
            minutes_0 = guards[std::stoi(match[1])].data();
        else if (std::regex_search(line, match,
                                   std::regex(R"(.*:(\d+)\] wakes up)")))
            std::for_each(minutes_0 + sleep_at, minutes_0 + std::stoi(match[1]),
                          [](auto& x) { ++x; });
        else if (std::regex_search(line, match,
                                   std::regex(R"(.*:(\d+)\] falls asleep)")))
            sleep_at = std::stoi(match[1]);
    }

    // Returns the <index,value> of the minute with the maximum sleep time
    // value.
    auto maxMinute = [](const auto& gi) {
        return [&gi](auto minute) -> std::pair<int, int> {
            return {std::distance(gi.begin(), minute), *minute};
        }(std::max_element(gi.begin(), gi.end()));
    };

    // Part 1: Find which guard has the most minutes asleep.
    const auto& [guard_id1, guard_minutes1] =  // structured binding
        *std::max_element(guards.begin(), guards.end(), [](auto& g1, auto& g2) {
            return std::accumulate(g1.second.begin(), g1.second.end(), 0) <
                   std::accumulate(g2.second.begin(), g2.second.end(), 0);
        });

    // Part 2: Find which guard is most frequently asleep on the same minute.
    const auto& [guard_id2, guard_minutes2] =  // structured binding
        *std::max_element(guards.begin(), guards.end(),
                          [&](auto& g1, auto& g2) {
                              return maxMinute(g1.second).second <
                                     maxMinute(g2.second).second;
                          });

    // Retrieve the sleepiest minute for the sleepiest guards found.
    const auto& minute1 = maxMinute(guard_minutes1).first;
    const auto& minute2 = maxMinute(guard_minutes2).first;

    // Output the solution.
    std::cout << "Part 1: "  // 30630
              << "Guard #" << guard_id1 << " x Minute: " << minute1 << " = "
              << minute1 * guard_id1 << "\n"
              << "Part 2: "  // 136571
              << "Guard #" << guard_id2 << " x Minute: " << minute2 << " = "
              << minute2 * guard_id2 << "\n"
              << std::flush;
}