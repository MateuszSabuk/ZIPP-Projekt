#include "Manager.h"

Manager::Manager()
{
	//// TODO Add all algorithms
	// algorithms.push_back(annealing or something)
}

std::pair<std::vector<int>, std::vector<std::vector<int>>> Manager::generate(int numOfStages, int numOfTasks, int maxNumOfMachinesInStage, int maxTaskTime)
{
	//// TODO Validation
	//if (numOfStages < 1) throw gcnew System::Exception();
	//if (numOfTasks < 1) throw gcnew System::Exception();
	//if (maxTaskTime < 1) throw gcnew System::Exception();
	//if (maxNumOfMachinesInStage < 2) throw gcnew System::Exception();

	auto machines = std::vector<int>(numOfStages);
	auto taskTimes = std::vector<std::vector<int>>(numOfTasks, std::vector<int>(numOfStages));
	
	// Randomization
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> machineDistrib(1, maxNumOfMachinesInStage);
	std::uniform_int_distribution<> timeDistrib(1, maxTaskTime);

	for (int i = 0; i < machines.size(); i++) {
		machines[i] = machineDistrib(gen);
	}
	// TODO Make sure there are at least m+1 machines

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
		names.push_back(it->name);
	}
	return names;
}

std::vector<std::vector<std::pair<int, int>>> Manager::run(int algId, std::vector<int> machines, std::vector<std::vector<int>> taskTimes)
{
	// TODO Null or Error propagation
	// if (isSolving) 

	// TODO Validate arguments

	isSolving = true;

	// TODO Make a thread
	auto solution = algorithms[algId].run(machines, taskTimes);
	
	isSolving = false;
	return solution;
}

void Manager::stop()
{
	// TODO
	// if (isSolving) killRunningThread();
	isSolving = false;
}
