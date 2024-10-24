#include "Bruteforce.h"
#include "Windows.h"

// Set the name of the algorithm
Bruteforce::Bruteforce() : Algorithm("Bruteforce")
{
	// A parameter example
	parameters.insert({ "Test parameter" , 1 });
}

std::vector<std::vector<std::pair<int, int>>> Bruteforce::start(std::vector<int> machines, std::vector<std::vector<int>> taskTimes)
{
	auto result = std::vector<std::vector<std::pair<int, int>>>(size(taskTimes), std::vector<std::pair<int, int>>(size(machines)));

	int counter = 0;
	for (auto & task : result)
	{
		for (auto& tuple : task)
		{
			if (isCanceled) throw "canceled";
			tuple.first = parameters["Test parameter"]++;
			tuple.second = counter++ + parameters["Test parameter"];
		}
	}

	return result;
}
