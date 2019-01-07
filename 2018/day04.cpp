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

using minutes_array = std::array<int, 60>;
using timecards = std::unordered_map<int, minutes_array>;
using timestamps = std::vector<std::string>;

int main() {
    // parse the dated event record input lines and sort them
    timestamps guard_events;
    for (std::string event; std::getline(std::cin, event);)
        guard_events.push_back(event);
    std::sort(guard_events.begin(), guard_events.end());  // chronological order

    // timestamp regexes
    static const std::regex re_guard(R"(.*Guard #(\d+) begins shift)");
    static const std::regex re_falls(R"(.*:(\d+)\] falls asleep)");
    static const std::regex re_wakes(R"(.*:(\d+)\] wakes up)");

    timecards cards;            // maps guard ids to sleep counts per minute
    int* minutes_00 = nullptr;  // ptr to a guard's sleep schedule array
    size_t slept_at, wakes_at;  // minutes parsed from timestamps
    std::smatch match;          // regex matcher

    // parse the sorted timestamps and compute each guard's sleep habits per min
    auto increment_time_slept = [](int& slept) { slept++; };
    for (const auto& event : guard_events)
        if (std::regex_search(event, match, re_guard)) {
            int guard_id = std::stoi(match[1]);
            minutes_00 = cards[guard_id].data();
        } else if (std::regex_search(event, match, re_wakes)) {
            wakes_at = std::stoi(match[1]);
            std::for_each(minutes_00 + slept_at, minutes_00 + wakes_at,
                          increment_time_slept);
        } else if (std::regex_search(event, match, re_falls))
            slept_at = std::stoi(match[1]);

    // returns the <index,count> of the minute with the max sleep time count
    auto maxMinute = [](const auto& min_arr) {
        return [&](const auto&& minute) -> std::pair<int, int> {
            return {std::distance(min_arr.cbegin(), minute), *minute};
        }(std::max_element(min_arr.cbegin(), min_arr.cend()));
    };

    // Part 1: Find which guard has the most minutes asleep
    const auto& [guard_id1, guard_minutes1] = *std::max_element(
        cards.cbegin(), cards.cend(), [](const auto& L, const auto& R) {
            const auto& [L_id, L_minutes] = L;
            const auto& [R_id, R_minutes] = R;
            auto sum_time_slept = [](const auto& min_arr) {
                return std::accumulate(min_arr.cbegin(), min_arr.cend(), 0);
            };
            return sum_time_slept(L_minutes) < sum_time_slept(R_minutes);
        });

    // Part 2: Find which guard is most frequently asleep on the same minute
    const auto& [guard_id2, guard_minutes2] = *std::max_element(
        cards.cbegin(), cards.cend(), [&](const auto& L, const auto& R) {
            const auto& [L_id, L_minutes] = L;
            const auto& [R_id, R_minutes] = R;
            const auto&& [L_index, L_sleep_cnt] = maxMinute(L_minutes);
            const auto&& [R_index, R_sleep_cnt] = maxMinute(R_minutes);
            return L_sleep_cnt < R_sleep_cnt;
        });

    // retrieve the sleepiest minute for the sleepiest guards found
    const auto& [minute1, cnt1] = maxMinute(guard_minutes1);
    const auto& [minute2, cnt2] = maxMinute(guard_minutes2);

    // output the solution
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