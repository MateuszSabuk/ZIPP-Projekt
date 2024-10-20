#include "Manager.h"

Manager::Manager()
{
	//// TODO Add all algorithms
	algorithms.push_back(std::make_unique<Bruteforce>());
}

std::pair<std::vector<int>, std::vector<std::vector<int>>> Manager::generate(int numOfStages, int numOfTasks, int maxNumOfMachinesInStage, int maxTaskTime)
{
	// Argument validation
	if (numOfStages < 1) throw std::invalid_argument("Number of stages must be higher than zero");
	if (numOfTasks < 1) throw std::invalid_argument("Number of tasks must be higher than zero");
	if (maxTaskTime < 1) throw std::invalid_argument("Max task time must be longer than zero");
	if (maxNumOfMachinesInStage < 2) throw std::invalid_argument("Max number of machines in a stage must be at least two");

	auto machines = std::vector<int>(numOfStages);
	auto taskTimes = std::vector<std::vector<int>>(numOfTasks, std::vector<int>(numOfStages));
	
	// Randomization
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> machineDistrib(1, maxNumOfMachinesInStage);
	std::uniform_int_distribution<> timeDistrib(1, maxTaskTime);

	bool enoughMachines = false;
	for (int i = 0; i < machines.size(); i++) {
		machines[i] = machineDistrib(gen);
		if (machines[i] > 1) enoughMachines = true;
	}

	// Make sure there are at least m+1 machines
	if (!enoughMachines) machines[0]++;


	for (int i = 0; i < taskTimes.size(); i++) {
		for (int j = 0; j < taskTimes[i].size(); j++) {
			taskTimes[i][j] = timeDistrib(gen);
		}
	}

	return std::pair<std::vector<int>, std::vector<std::vector<int>>>(machines, taskTimes);
}

std::vector<std::string> Manager::getAlgorithmNames()
{
	std::vector<std::string> names;
	for (auto it = begin(algorithms); it != end(algorithms); ++it) {
		names.push_back((*it)->getName());
	}
	return names;
}

std::unordered_map<std::string, int> Manager::getAlgorithmParams(int algId)
{
	if (algId < 0 || algId >= size(algorithms)) throw std::invalid_argument("algId out of range");
	return algorithms[algId]->getParams();
}

void Manager::setAlgorithmParams(int algId, std::unordered_map<std::string, int> params)
{
	if (algId < 0 || algId >= size(algorithms)) throw std::invalid_argument("algId out of range");
	algorithms[algId]->setParams(params);
}

std::vector<std::vector<std::pair<int, int>>> Manager::run(int algId, std::vector<int> machines, std::vector<std::vector<int>> taskTimes)
{
	// Validate all the arguments
	if (algId < 0 || algId >= size(algorithms)) throw std::invalid_argument("algId out of range");
	if (size(taskTimes) == 0) throw std::invalid_argument("Empty task list");
	if (size(machines) == 0) throw std::invalid_argument("Empty machines list");
	size_t vecLen = size(machines);
	int sumOfMachines = 0;
	for (auto machineNum : machines)
		sumOfMachines += machineNum;
	if (sumOfMachines == vecLen) throw std::invalid_argument("Not enough machines");
	for (auto taskTime : taskTimes)
		if (vecLen != size(taskTime)) throw std::invalid_argument("Invalid List sizes");


	// Run the algorithm
	try {
		auto solution = algorithms[algId]->run(machines, taskTimes);
		return solution;
	}
	catch (...) {
		throw;
	}
}

void Manager::cancelAlgorithm()
{
	for (auto it = begin(algorithms); it != end(algorithms); ++it) {
		(*it)->cancel();
	}
}
