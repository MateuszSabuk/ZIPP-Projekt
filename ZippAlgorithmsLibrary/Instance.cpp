#include "Instance.h"
#include <random>

namespace ZippAlgorithmsLibrary {
    Instance::Instance(int numOfStages, int numOfTasks, int maxNumOfMachinesInStage, int maxTaskTime)
    {
        //// Validation
        //if (numOfStages < 1) throw gcnew System::Exception();
        //if (numOfTasks < 1) throw gcnew System::Exception();
        //if (maxTaskTime < 1) throw gcnew System::Exception();
        //if (maxNumOfMachinesInStage < 2) throw gcnew System::Exception();

        this->s = numOfStages;
        this->n = numOfTasks;

        this->machines = std::vector<int>(numOfStages);
        this->taskTimes = std::vector<std::vector<int>>(numOfTasks, std::vector<int>(numOfStages));
        this->randomizeArrays(maxNumOfMachinesInStage, maxTaskTime);
    }

    void Instance::randomizeArrays(int maxMIS, int maxTT)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> machineDistrib(1, maxMIS);
        std::uniform_int_distribution<> timeDistrib(1, maxTT);

        for (int i = 0; i < this->machines.size(); i++) {
            this->machines[i] = machineDistrib(gen);
        }
        // TODO At least m+1 machines

        for (int i = 0; i < this->taskTimes.size(); i++) {
            for (int j = 0; j < this->taskTimes[i].size(); j++) {
                this->taskTimes[i][j] = timeDistrib(gen);
            }
        }
    }

    int Instance::getNumOfStages()
    {
        return this->s;
    }

    int Instance::getNumOfTasks()
    {
        return this->n;
    }

    const std::vector<int>& Instance::getMachines()
    {
		return this->machines;
	}

    const std::vector<std::vector<int>>& Instance::getTaskTimes()
    {
        return this->taskTimes;
    }
}