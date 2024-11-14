#include "Bruteforce.h"

// Set the name of the algorithm
Bruteforce::Bruteforce() : Algorithm("Bruteforce")
{
	// A parameter example
	parameters.insert({ "Thread number" , 1 });
}

std::vector<std::vector<std::pair<int, int>>> Bruteforce::start(const std::vector<int> &machines, const std::vector<std::vector<int>> &taskTimes)
{
    int maxThreads = std::thread::hardware_concurrency();
    int requestedThreads = parameters["Thread number"];
    int threadCount = std::min(requestedThreads, maxThreads > 0 ? maxThreads : 1);

    auto schedule = std::vector<std::vector<std::pair<int, int>>>(taskTimes.size(), std::vector<std::pair<int, int>>(machines.size()));

    int minTime = std::numeric_limits<int>::max();
    std::mutex mtx;

    auto allPermutations = quickPerm(taskTimes.size());

    int permutationCount = allPermutations.size();
    int chunkSize = permutationCount / threadCount;
    int remainder = permutationCount % threadCount;

    auto worker = [&](int startIndex, int endIndex)
    {
        int localMinTime = std::numeric_limits<int>::max();
        std::vector<std::vector<std::pair<int, int>>> localSchedule;

        for (int i = startIndex; i < endIndex; i++) {
            int time;
            auto permutationSchedule = create_schedule(time, machines, taskTimes, allPermutations[i]);

            // Update local minimum time and schedule
            if (time < localMinTime) {
                localMinTime = time;
                localSchedule = permutationSchedule;
            }
        }

        // Lock to update global minTime and schedule
        std::lock_guard<std::mutex> lock(mtx);
        if (localMinTime < minTime) {
            minTime = localMinTime;
            schedule = localSchedule;
        }

    };

    std::vector<std::thread> threads;
    
    int startIdx = 0;
    for (int i = 0; i < threadCount; ++i) {
        int endIdx = startIdx + chunkSize + (i < remainder ? 1 : 0);
        threads.emplace_back(worker, startIdx, endIdx);
        startIdx = endIdx;
    }

    // Wait for all futures to complete
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    return schedule;
}

std::vector<std::vector<int>> Bruteforce::quickPerm(int length)
{
    std::vector<std::vector<int>> out;
    auto a = std::vector<int>(length);
    auto p = std::vector<int>(length + 1);

    for (int i = 0; i < length; i++)
    {
        if (isCanceled) throw "canceled";
		a[i] = p[i] = i;
    }
    out.push_back(a);

    int i = 0;
    while (i < length)
    {
        if (isCanceled) throw "canceled";
        --p[i];
		int j = i % 2 * p[i];
        std::swap(a[i], a[j]);
        out.push_back(a);
        i = 1;
		while (!p[i])
        {
            p[i] = i;
            i++;
        }
    }
    return out;
}
