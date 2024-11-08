#include "Bruteforce.h"
#include "Windows.h"

// Set the name of the algorithm
Bruteforce::Bruteforce() : Algorithm("Bruteforce")
{
	// A parameter example
	parameters.insert({ "Thread number" , 1 });
}

std::vector<std::vector<std::pair<int, int>>> Bruteforce::start(std::vector<int> machines, std::vector<std::vector<int>> taskTimes)
{
	auto schedule = std::vector<std::vector<std::pair<int, int>>>(size(taskTimes), std::vector<std::pair<int, int>>(size(machines)));

	int minTime = INT_MAX;
	std::vector<int> minPermutationNumbers;
	
	for (auto permutationNumbers : quickPerm(size(taskTimes)))
	{
		std::vector<std::vector<int>> permutation;
		for (int index : permutationNumbers)
		{
			if (isCanceled) throw "canceled";
			permutation.push_back(taskTimes[index]);
		}
		auto permutationSchedule = getSchedule(machines, permutation);
		int time = getWorkTime(permutationSchedule, permutation);
		if (time < minTime)
		{
			schedule = permutationSchedule;
			minTime = time;
			minPermutationNumbers = permutationNumbers;
		}
	}

	auto output = std::vector<std::vector<std::pair<int, int>>>(size(taskTimes), std::vector<std::pair<int, int>>(size(machines)));
	for (int i = 0; i < size(taskTimes); i++)
	{
		output[minPermutationNumbers[i]] = schedule[i];
	}

	return output;
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
	p[length] = length;
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
			p[i] = 1;
			i++;
		}
	}
	return out;
}

int Bruteforce::getWorkTime(const std::vector<std::vector<std::pair<int, int>>> &schedule, const std::vector<std::vector<int>> &taskTimes)
{
	int maxTime = 0;
	for (int i = 0; i < size(taskTimes); i++)
	{
		int endTime = schedule[i].back().second + taskTimes[i].back();
		if (maxTime < endTime) maxTime = endTime;
	}
	return maxTime;
}

std::vector<std::vector<std::pair<int, int>>> Bruteforce::getSchedule(std::vector<int> machines, std::vector<std::vector<int>> taskTimes)
{
	int numOfStages = size(machines);
	int numOfTasks = size(taskTimes);

	auto schedule = std::vector<std::vector<std::pair<int, int>>>(numOfTasks, std::vector<std::pair<int, int>>(numOfStages));

	auto machineEndTimes = std::vector<std::vector<int>>(numOfStages);

	for (int stage = 0; stage < numOfStages; ++stage) {
		machineEndTimes[stage].resize(machines[stage], 0);
	}

	auto taskReadyTimes = std::vector<int>(numOfTasks, 0);
	for (int stageIndex = 0; stageIndex < numOfStages; stageIndex++)
	{
		for (int taskIndex = 0; taskIndex < numOfTasks; taskIndex++)
		{
			if (isCanceled) throw "canceled";
			// Get first ready machine
			auto readyMachine = min_element(begin(machineEndTimes[stageIndex]), end(machineEndTimes[stageIndex]));
			int machineIndex = distance(begin(machineEndTimes[stageIndex]), readyMachine);
			int taskStartTime;
			// If task is ready for the machine
			if (taskReadyTimes[taskIndex] <= *readyMachine)
			{
				if (taskIndex > 0 && schedule[taskIndex - 1][stageIndex].second > *readyMachine)
					*readyMachine = schedule[taskIndex - 1][stageIndex].second;
				taskStartTime = *readyMachine;
				*readyMachine += taskTimes[taskIndex][stageIndex];
				taskReadyTimes[taskIndex] = *readyMachine;
			}
			else // If machine is waiting for the task
			{
				if (taskIndex > 0 && schedule[taskIndex - 1][stageIndex].second > taskReadyTimes[taskIndex])
					taskReadyTimes[taskIndex] = schedule[taskIndex - 1][stageIndex].second;
				taskStartTime = taskReadyTimes[taskIndex];
				taskReadyTimes[taskIndex] += taskTimes[taskIndex][stageIndex];
				*readyMachine = taskReadyTimes[taskIndex];
			}
			// Populate schedule
			schedule[taskIndex][stageIndex] = {machineIndex, taskStartTime};
		}
	}
	return schedule;
}
