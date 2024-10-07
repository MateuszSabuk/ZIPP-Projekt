#include "SolverManager.h"
namespace CLI
{
    SolverManager::SolverManager()
        : ManagedObject(new Manager())
    {
        //Console::WriteLine("Creating a new Entity-wrapper object!");
    }

    Tuple<array<int>^, array<int, 2>^>^ SolverManager::generate(int numOfStages, int numOfTasks, int maxNumOfMachinesInStage, int maxTaskTime)
    {
        std::pair<std::vector<int>, std::vector<std::vector<int>>> data =
            m_Instance->generate(numOfStages, numOfTasks, maxNumOfMachinesInStage, maxTaskTime);
        auto machines = vec2array(data.first);
        auto taskTimes = vec2array2d(data.second);

        return gcnew Tuple<array<int>^, array<int, 2>^>(machines, taskTimes);
    }

    array<int>^ SolverManager::vec2array(const std::vector<int>& data)
    {
        auto size = static_cast<int>(data.size());
        array<int>^ managedArray = gcnew array<int>(size);
        for (int i = 0; i < data.size(); ++i) {
            managedArray[i] = data[i];
        }
        return managedArray;
    }

    array<int, 2>^ SolverManager::vec2array2d(std::vector<std::vector<int>>& data)
    {
        int s = static_cast<int>(data[0].size());
        int n = static_cast<int>(data.size());
        array<int, 2>^ managedArray = gcnew array<int, 2>(n,s);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < s; j++)
                managedArray[i, j] = data[i][j];
        return managedArray;
    }
}