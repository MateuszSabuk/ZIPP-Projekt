#include "Bruteforce.h"
#include <iostream>
#include <numeric>
#include <string>

// Set the name of the algorithm
Bruteforce::Bruteforce() : Algorithm("Bruteforce")
{
	// A parameter example
	parameters.insert({ "Thread number" , std::thread::hardware_concurrency() });
}

std::vector<std::vector<std::pair<int, int>>> Bruteforce::start(std::vector<int>& solvedPermutation, const std::vector<int> &machines, const std::vector<std::vector<int>> &taskTimes)
{

    int maxThreads = std::thread::hardware_concurrency();
    int requestedThreads = parameters["Thread number"];
    int threadCount = std::min(requestedThreads, maxThreads > 0 ? maxThreads : 1);
    // Validation
    if (threadCount <= 0) throw "Thread number parameter must be positive";

    auto schedule = std::vector<std::vector<std::pair<int, int>>>(taskTimes.size(), std::vector<std::pair<int, int>>(machines.size()));

    int minTime = std::numeric_limits<int>::max();
    std::vector<int> bestYetPermutation;
    std::mutex mtx;

    auto worker = [&](std::vector<int> permutation, size_t numOfPermutations)
    {
        int localMinTime = std::numeric_limits<int>::max();
        std::vector<std::vector<std::pair<int, int>>> localSchedule;
        std::vector<int> localPermutation;

        size_t i = 0;
        do
        {
            if (isCanceled) return;
            int time;
            auto permutationSchedule = create_schedule(time, machines, taskTimes, permutation);

            // Update local minimum time and schedule
            if (time < localMinTime) {
                localMinTime = time;
                localSchedule = permutationSchedule;
                localPermutation = permutation;
            }
        }
        while (++i < numOfPermutations && std::next_permutation(permutation.begin(), permutation.end()));

        // Lock to update global minTime and schedule
        std::lock_guard<std::mutex> lock(mtx);
        if (localMinTime < minTime) {
            minTime = localMinTime;
            schedule = localSchedule;
            bestYetPermutation = localPermutation;
        }
    };

    std::vector<std::thread> threads;

    std::vector<int> startPermutation(taskTimes.size());
    std::iota(startPermutation.begin(), startPermutation.end(), 0);

    size_t permutationCount = 1;
    for (int i = 1; i <= startPermutation.size(); ++i) {
        permutationCount *= i;
    }
    size_t chunkSize = permutationCount / threadCount;
    size_t remainder = permutationCount % threadCount;
    
    for (int i = 1; i < threadCount; i++) {
        int length = chunkSize + (i <= remainder ? 1 : 0);

        threads.emplace_back(worker, startPermutation, length);

        for (int j = 0; j < length; j++) {
            if (isCanceled) throw "canceled";
            std::next_permutation(startPermutation.begin(), startPermutation.end());
        }
    }
    threads.emplace_back(worker, startPermutation, chunkSize);


    // Wait for all futures to complete
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    if (isCanceled) throw "canceled";

    solvedPermutation = bestYetPermutation;

    return schedule;
}
