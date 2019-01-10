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
using namespace std;
using DAG = map<char, set<char>>;

// helper struct for part 2
struct Worker {
    bool idle{true};
    char job{0};
    int timeToFinish{0};
};

// Find jobs that have no dependencies in the jobsDAG
deque<char> getReadyJobs(DAG& jobsDAG) {
    deque<char> readyJobs;
    for (auto& [job, deps] : jobsDAG)
        if (deps.empty())
            readyJobs.push_back(job);
    return readyJobs;
}

// Part 1
// In what order should the jobs in your instructions be completed?
// Your puzzle answer was BFKEGNOVATIHXYZRMCJDLSUPWQ
string part1(DAG jobsDAG) {
    string jobOrder;
    do {
        auto readyJobs = getReadyJobs(jobsDAG);
        char first = readyJobs.front();
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
    vector<Worker> workers(numWorkers);
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
    for (string line; getline(cin, line);) {
        char jobA = line[5], jobB = line[36];
        jobsDAG[jobA], jobsDAG[jobB].emplace(jobA);
    }

    // Debug printing.
    for (auto& [job, deps] : jobsDAG) {
        cout << job << ": ";
        for (auto& d : deps)
            cout << d << " ";
        cout << endl;
    }

    // Output solutions.
    cout << "Part 1: Job Order = " << part1(jobsDAG)
         << endl;  // CABDFE // BFKEGNOVATIHXYZRMCJDLSUPWQ
    cout << "Part 2: Duration  = " << part2(jobsDAG, 5, 60)
         << endl;  // 15 // 1020
}