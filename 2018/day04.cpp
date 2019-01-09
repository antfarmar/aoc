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

// type aliases for convenience and readability
using minutes_array = std::array<int, 60>;
using timecards = std::unordered_map<int, minutes_array>;
using timestamps = std::vector<std::string>;

// parse the dated event record input lines into a vector
timestamps parse_lines(std::istream& is) {
    timestamps guard_events;
    for (std::string event; std::getline(is, event);)
        guard_events.push_back(event);
    return guard_events;
}

// makes a "timecard" for each guard according to the recorded events
timecards fillout_timecards(timestamps& guard_events) {
    // timestamp regexes to match
    std::smatch match;
    static const std::regex re_guard(R"(.*Guard #(\d+) begins shift)");
    static const std::regex re_falls(R"(.*:(\d+)\] falls asleep)");
    static const std::regex re_wakes(R"(.*:(\d+)\] wakes up)");

    timecards cards;            // maps guard ids to sleep counts per minute
    int* minutes_00 = nullptr;  // ptr to a guard's sleep schedule array
    int slept_at, wakes_at;     // the minutes field parsed from the timestamps

    // first sort the shift events in chronological order
    std::sort(guard_events.begin(), guard_events.end());

    // then parse the timestamps and compute each guard's sleep habits per min
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

    return cards;
}

int main() {
    // the timestamped guard shift events
    timestamps guard_events{parse_lines(std::cin)};

    // maps guard ids to their sleep amount per minute
    timecards cards{fillout_timecards(guard_events)};

    // returns the <index,count> of the minute with the max sleep time count
    auto max_minute_asleep = [](const auto& min_arr) {
        return [&](const auto&& minute_itr) -> std::pair<int, int> {
            return {std::distance(min_arr.cbegin(), minute_itr), *minute_itr};
        }(std::max_element(min_arr.cbegin(), min_arr.cend()));
    };

    // Part 1
    // Strategy 1: Find the guard that has the most minutes asleep. What minute
    // does that guard spend asleep the most?
    // Solution: Guard #1021 x Minute 30 = 30630
    const auto& [guard_id1, guard_minutes1] = *std::max_element(
        cards.cbegin(), cards.cend(), [](const auto& L, const auto& R) {
            auto sum_time_slept = [](const auto& min_arr) {
                return std::accumulate(min_arr.cbegin(), min_arr.cend(), 0);
            };
            const auto& [L_id, L_minutes] = L;
            const auto& [R_id, R_minutes] = R;
            return sum_time_slept(L_minutes) < sum_time_slept(R_minutes);
        });

    // Part 2
    // Strategy 2: Of all guards, which guard is most frequently asleep on the
    // same minute?
    // Solution: Guard #3331 x Minute 41 = 136571
    const auto& [guard_id2, guard_minutes2] = *std::max_element(
        cards.cbegin(), cards.cend(), [&](const auto& L, const auto& R) {
            const auto& [L_id, L_minutes] = L;
            const auto& [R_id, R_minutes] = R;
            const auto&& [L_index, L_sleep_cnt] = max_minute_asleep(L_minutes);
            const auto&& [R_index, R_sleep_cnt] = max_minute_asleep(R_minutes);
            return L_sleep_cnt < R_sleep_cnt;
        });

    // retrieve the sleepiest minute for each of the sleepiest guards found
    const auto& [max_minute1, cnt1] = max_minute_asleep(guard_minutes1);
    const auto& [max_minute2, cnt2] = max_minute_asleep(guard_minutes2);

    // output the solutions
    // Part 1: Guard #1021 x Minute 30 = 30630
    // Part 2: Guard #3331 x Minute 41 = 136571
    std::cout << "Part 1: "
              << "Guard #" << guard_id1 << " x Minute " << max_minute1 << " = "
              << max_minute1 * guard_id1 << "\n"
              << "Part 2: "
              << "Guard #" << guard_id2 << " x Minute " << max_minute2 << " = "
              << max_minute2 * guard_id2 << "\n"
              << std::flush;
}