// Advent of Code 2018
// Day 7: The Sum of Its Parts
// https://adventofcode.com/2018/day/7

#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <vector>

// type aliases for convenience and readability
using Job = char;
using JobSequence = std::string;

// DAG: directed acyclic graph (map (node) -> set (adjaceny edges))
// the DAG represents jobs and their dependencies (job)->(jobs to do first)
// ordered map/set since job ordering ties rely on sorted jobs (chars)
using DAG = std::map<Job, std::set<Job>>;

// an elf worker data struct (for part 2)
struct Worker {
    int timeToFinish{0};
    bool idle{true};
    Job job{'?'};
};

// Find jobs that have no dependencies in the jobsDAG
std::deque<Job> getReadyJobs(DAG& jobsDAG) {
    std::deque<Job> readyJobs;
    for (auto& [job, deps] : jobsDAG)
        if (deps.empty())
            readyJobs.push_back(job);
    return readyJobs;
}

// Part 1
// In what order should the jobs in your instructions be completed?
// Your puzzle answer was BFKEGNOVATIHXYZRMCJDLSUPWQ
JobSequence part1(DAG jobsDAG) {
    JobSequence jobOrder;
    do {
        auto readyJobs = getReadyJobs(jobsDAG);
        Job first = readyJobs.front();
        for (auto& [_, deps] : jobsDAG)
            deps.erase(first);
        jobOrder.push_back(first);
        jobsDAG.erase(first);
    } while (!jobsDAG.empty());
    return jobOrder;
}

// Part 2
// With 5 workers and 60+ second job durations, how long will it take
// to complete all of the jobs?
// Your puzzle answer was 1020
int part2(DAG jobsDAG, int numWorkers, int extraTime) {
    bool allWorkersIdle;
    int currentSecond = -1;
    std::vector<Worker> workers(numWorkers);
    do {
        currentSecond++;
        allWorkersIdle = true;
        for (auto& w : workers) {
            if (!(w.idle || --w.timeToFinish)) {
                w.idle = true;
                for (auto& [_, deps] : jobsDAG)
                    deps.erase(w.job);
            }
        }
        auto readyJobs = getReadyJobs(jobsDAG);
        for (auto& w : workers) {
            if (w.idle && !readyJobs.empty()) {
                w.idle = false;
                w.job = readyJobs.front();
                w.timeToFinish = extraTime + int(w.job - 'A') + 1;
                readyJobs.pop_front();
                jobsDAG.erase(w.job);
            }
            allWorkersIdle &= w.idle;
        }
    } while (!(jobsDAG.empty() && allWorkersIdle));
    return currentSecond;
}

// Solve Day 07
int main() {
    // Job Dependencies: Directed Acyclic Graph (DAG)
    // Adj. Edge Map: Job Node -> Dependency Job Nodes Set(ordered)
    DAG jobsDAG;

    // Parse the job dependencies and build the DAG map.
    for (std::string line; std::getline(std::cin, line);) {
        Job jobA = line[5], jobB = line[36];
        jobsDAG[jobA], jobsDAG[jobB].emplace(jobA);
    }

    // Debug printing.
    for (auto& [job, deps] : jobsDAG) {
        std::cout << job << ": ";
        for (auto& d : deps)
            std::cout << d << " ";
        std::cout << std::endl;
    }

    // Output solutions.
    std::cout << "Part 1: Job Order = " << part1(jobsDAG)
              << std::endl;  // CABDFE // BFKEGNOVATIHXYZRMCJDLSUPWQ
    std::cout << "Part 2: Duration  = " << part2(jobsDAG, 5, 60)
              << std::endl;  // 15 // 1020
}