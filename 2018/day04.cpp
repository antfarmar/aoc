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

using timecards = std::unordered_map<int, std::array<int, 60>>;
using timestamps = std::vector<std::string>;

int main() {
    // Scan the date record event lines and sort them
    timestamps events;
    for (std::string ln; std::getline(std::cin, ln);)
        events.push_back(ln);
    std::sort(events.begin(), events.end());  // chronological dates

    // Map guard ids to their sleep schedule array of minutes.
    timecards cards;
    int* minutes_0 = nullptr;
    int sleep_at = -1;
    std::smatch match;

    // Parse the timestamps and compute all guard sleep schedules.
    for (const auto& event : events) {
        if (std::regex_search(event, match,
                              std::regex(R"(.*Guard #(\d+) begins shift)")))
            minutes_0 = cards[std::stoi(match[1])].data();
        else if (std::regex_search(event, match,
                                   std::regex(R"(.*:(\d+)\] wakes up)")))
            std::for_each(minutes_0 + sleep_at, minutes_0 + std::stoi(match[1]),
                          [](auto& x) { ++x; });
        else if (std::regex_search(event, match,
                                   std::regex(R"(.*:(\d+)\] falls asleep)")))
            sleep_at = std::stoi(match[1]);
    }

    // Returns the <index,value> of the minute with the maximum sleep time
    // value.
    auto maxMinute = [](const auto& minutes_array) {
        return [&](auto&& minute) -> std::pair<int, int> {
            return {std::distance(minutes_array.cbegin(), minute), *minute};
        }(std::max_element(minutes_array.cbegin(), minutes_array.cend()));
    };

    // Part 1: Find which guard has the most minutes asleep.
    const auto& [guard_id1, guard_minutes1] = *std::max_element(
        cards.cbegin(), cards.cend(), [](const auto& lhs, const auto& rhs) {
            auto sum_minutes = [](const auto& arr) {
                return std::accumulate(arr.cbegin(), arr.cend(), 0);
            };
            auto& [lg_id, lg_mins] = lhs;
            auto& [rg_id, rg_mins] = rhs;
            return sum_minutes(lg_mins) < sum_minutes(rg_mins);
        });

    // Part 2: Find which guard is most frequently asleep on the same minute.
    const auto& [guard_id2, guard_minutes2] = *std::max_element(
        cards.cbegin(), cards.cend(), [&](const auto& lhs, const auto& rhs) {
            return maxMinute(lhs.second).second < maxMinute(rhs.second).second;
        });

    // Retrieve the sleepiest minute for the sleepiest guards found.
    const auto& [minute1, amt1] = maxMinute(guard_minutes1);
    const auto& [minute2, amt2] = maxMinute(guard_minutes2);

    // Output the solution.
    // Part 1: Guard #1021 x Minute: 30 = 30630
    // Part 2: Guard #3331 x Minute: 41 = 136571
    std::cout << "Part 1: "
              << "Guard #" << guard_id1 << " x Minute: " << minute1 << " = "
              << minute1 * guard_id1 << "\n"
              << "Part 2: "
              << "Guard #" << guard_id2 << " x Minute: " << minute2 << " = "
              << minute2 * guard_id2 << "\n"
              << std::flush;
}