#pragma once
#include <vector>
#include <string>
#include <random>
#include <stdexcept>
#include <thread>

#include "Algorithm.h"
#include "Bruteforce.h"

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
	std::unordered_map<std::string, int> getAlgorithmParams(int algId);
	void setAlgorithmParams(int algId, std::unordered_map<std::string, int> params);

	std::vector<std::vector<std::pair<int, int>>> run(int algId, std::vector<int> machines, std::vector<std::vector<int>> taskTimes);

private:
	std::vector<std::unique_ptr<Algorithm>> algorithms;
};

