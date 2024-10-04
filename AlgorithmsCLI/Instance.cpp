#include "Instance.h"
namespace CLI
{
    Instance::Instance(int numOfStages, int numOfTasks, int maxNumOfMachinesInStage, int maxTaskTime)
        : ManagedObject(new ZippAlgorithmsLibrary::Instance(numOfStages, numOfTasks, maxNumOfMachinesInStage, maxTaskTime))
    {
        //Console::WriteLine("Creating a new Entity-wrapper object!");
    }

    void Instance::randomizeArrays(int maxMIS, int maxTT)
    {
        m_Instance->randomizeArrays(maxMIS, maxTT);
    }

    array<int>^ Instance::getMachines()
    {
        const std::vector<int>& data = m_Instance->getMachines();
        array<int>^ managedArray = gcnew array<int>(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            managedArray[i] = data[i];
        }
        return managedArray;
    }
    array<int, 2>^ Instance::getTaskTimes()
    {
        int s = m_Instance->getNumOfStages();
        int n = m_Instance->getNumOfTasks();
        const std::vector<std::vector<int>>& data = m_Instance->getTaskTimes();
        array<int, 2>^ managedArray = gcnew array<int, 2>(n,s);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < s; j++)
                managedArray[i, j] = data[i][j];
        return managedArray;
    }
}