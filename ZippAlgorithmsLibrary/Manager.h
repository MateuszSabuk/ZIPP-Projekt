#pragma once
#include <vector>
#include <string>
#include <random>

#include "Algorithm.h"

class Manager
{
public:
	Manager();
	/*
	Returns a pair of vectors:
	- vector of numbers of machines for each stage
	- vector for each task containing a vector with times for each stage
	*/
	std::pair<std::vector<int>, std::vector<std::vector<int>>> generate(int numOfStages, int numOfTasks, int maxNumOfMachinesInStage, int maxTaskTime);

	std::vector<std::string> getAlgorithmNames();

	std::vector<std::vector<std::pair<int, int>>> run(int algId, std::vector<int> machines, std::vector<std::vector<int>> taskTimes);
	void stop();

private:
	std::vector<Algorithm> algorithms;
	bool isSolving = false;
};

