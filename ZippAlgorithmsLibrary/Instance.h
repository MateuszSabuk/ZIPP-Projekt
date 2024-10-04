#pragma once

#include <vector>

namespace ZippAlgorithmsLibrary {
	class Instance
	{
	public:
		Instance(int numOfStages, int numOfTasks, int maxNumOfMachinesInStage, int maxTaskTime);
		// Helpers
		void randomizeArrays(int maxNumOfMachinesInStage, int maxTaskTime);
		// Getters
		int getNumOfStages();
		int getNumOfTasks();
		const std::vector<int>& getMachines();
		const std::vector<std::vector<int>>& getTaskTimes();

	private:
		int s;
		int n;
		std::vector<int> machines;
		std::vector<std::vector<int>> taskTimes;
	};
}