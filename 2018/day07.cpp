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
using JobQueue = std::deque<Job>;

// DAG: directed acyclic graph (map (node) -> set (adjaceny edges))
// the DAG represents jobs and their dependencies (job)->(jobs it depends on)
// ordered map/set since job ordering ties rely on sorted jobs (chars)
using DAG = std::map<Job, std::set<Job>>;

// an elf worker data struct (for part 2)
struct Worker {
    Job job{'?'};
    bool idle{true};
    int timeToFinishJob{0};
};

// Find jobs that have no dependencies in the jobsDAG
JobQueue getFreeJobs(DAG& jobsDAG) {
    JobQueue freeJobs;
    for (auto& [job, deps] : jobsDAG)
        if (deps.empty())
            freeJobs.push_back(job);
    return freeJobs;
}

// Part 1
// In what order should the jobs in your instructions be completed?
// Your puzzle answer was BFKEGNOVATIHXYZRMCJDLSUPWQ
JobSequence part1(DAG jobsDAG) {
    JobSequence jobOrder;

    while (!jobsDAG.empty()) {
        JobQueue freeJobs = getFreeJobs(jobsDAG);
        Job nextJob = freeJobs.front();
        for (auto& [_, deps] : jobsDAG)
            deps.erase(nextJob);
        jobOrder.push_back(nextJob);
        jobsDAG.erase(nextJob);
    }

    return jobOrder;
}

// Part 2
// With 5 workers and 60+ second job durations, how long will it take
// to complete all of the jobs?
// Your puzzle answer was 1020
int part2(DAG jobsDAG, const int numWorkers, const int jobDuration) {
    std::vector<Worker> workers(numWorkers);
    bool allWorkersIdle = true;
    int timeElapsed = -1;

    // assign jobs to elves until all jobs done
    while (not(jobsDAG.empty() and allWorkersIdle)) {
        timeElapsed++;
        // has any elf worker completed a job?
        for (Worker& elf : workers) {
            if (not(elf.idle or --elf.timeToFinishJob)) {
                elf.idle = true;
                for (auto& [_, deps] : jobsDAG)
                    deps.erase(elf.job);
            }
        }
        // assign jobs to idle elves while still available
        JobQueue freeJobs = getFreeJobs(jobsDAG);
        allWorkersIdle = true;
        for (Worker& elf : workers) {
            if (elf.idle and not freeJobs.empty()) {
                elf.idle = false;
                elf.job = freeJobs.front();
                elf.timeToFinishJob = jobDuration + int(elf.job - 'A') + 1;
                freeJobs.pop_front();
                jobsDAG.erase(elf.job);
            }
            allWorkersIdle &= elf.idle;
        }
    }

    return timeElapsed;
}

// Solve Day 07
int main() {
    // Job Dependencies: Directed Acyclic Graph (DAG)
    // Adj. Edge Map: Job Node -> Dependency Job Nodes Set(ordered)
    DAG jobsDAG;

    // Parse the job dependencies and build the DAG
    // e.g. "Step A must be finished before step B can begin."
    for (std::string line; std::getline(std::cin, line);) {
        Job jobA = line[5];
        Job jobB = line[36];
        jobsDAG[jobA];
        jobsDAG[jobB].emplace(jobA);
    }

    // Debug printing
    for (auto& [job, deps] : jobsDAG) {
        std::cout << job << ": ";
        for (auto& d : deps)
            std::cout << d << " ";
        std::cout << std::endl;
    }

    // Output solutions
    std::cout << "Part 1: Job Order = " << part1(jobsDAG)
              << std::endl;  // CABDFE // BFKEGNOVATIHXYZRMCJDLSUPWQ
    std::cout << "Part 2: Duration  = " << part2(jobsDAG, 5, 60)
              << std::endl;  // 15 // 1020
}