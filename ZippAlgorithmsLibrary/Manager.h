#pragma once
#include <vector>
#include <string>
#include <random> // Used for the generation of the data
#include <stdexcept> // Used for error propagation

#include "Algorithm.h"
// Used algorithms BEGIN
#include "Bruteforce.h"
#include "SimulatedAnnealing.h"
#include "NEH.h"
// Used algorithms END

class Manager
{
public:
	Manager();
	// Returns a pair of vectors:
	// - vector of numbers of machines for each stage
	// - vector for each task containing a vector with times for each stage
	std::pair<std::vector<int>, std::vector<std::vector<int>>> generate(int numOfStages, int numOfTasks, int maxNumOfMachinesInStage, int maxTaskTime);

	// Get names of all available algorithms
	std::vector<std::string> getAlgorithmNames();
	// Get parameters of the chosen algorithm
	std::unordered_map<std::string, int> getAlgorithmParams(int algId);
	// Set the parameters of the chosen algorithm
	void setAlgorithmParams(int algId, std::unordered_map<std::string, int> params);

	// Run the chosen algorithm solve
	std::vector<std::vector<std::pair<int, int>>> run(int algId, std::vector<int> machines, std::vector<std::vector<int>> taskTimes);
	// Cancel the solve of the running algorithm
	void cancelAlgorithm();

private:
	// List of available algorithms
	std::vector<std::unique_ptr<Algorithm>> algorithms;
};

