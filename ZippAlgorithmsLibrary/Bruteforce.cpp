#include "Bruteforce.h"
#include "Windows.h"

// Set the name of the algorithm
Bruteforce::Bruteforce() : Algorithm("Bruteforce")
{
	// A parameter example
	parameters.insert({ "Thread number" , 1 });
}

std::vector<std::vector<std::pair<int, int>>> Bruteforce::start(const std::vector<int> &machines, const std::vector<std::vector<int>> &taskTimes)
{
	auto schedule = std::vector<std::vector<std::pair<int, int>>>(size(taskTimes), std::vector<std::pair<int, int>>(size(machines)));

	int minTime = INT_MAX;
	int time;
	std::vector<int> minPermutationNumbers;
	
	for (auto permutationNumbers : quickPerm(size(taskTimes)))
	{
		auto permutationSchedule = create_schedule(time, machines, taskTimes, permutationNumbers);
		if (time < minTime)
		{
			schedule = permutationSchedule;
			minTime = time;
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
