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

    array<String^>^ SolverManager::getAlgorithmNames()
    {
        std::vector<std::string> namesVec = m_Instance->getAlgorithmNames();
        array<String^>^ names = gcnew array<String^>(size(namesVec));
        for (auto i = 0; i < size(namesVec); i++)
        {
            names[i] = gcnew String(namesVec[i].c_str());
        }
        return names;
    }

    array<Tuple<int>^, 2>^ SolverManager::run(int algId, array<int>^ machines, array<int, 2>^ taskTimes)
    {
        std::vector<int> nativeMachines(machines->Length);
        for (int i = 0; i < machines->Length; i++) {
            nativeMachines[i] = machines[i];
        }

        int rows = taskTimes->GetLength(0);
        int cols = taskTimes->GetLength(1);
        std::vector<std::vector<int>> nativeTaskTimes(rows, std::vector<int>(cols));

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                nativeTaskTimes[i][j] = taskTimes[i, j];
            }
        }

        std::vector<std::vector<std::pair<int, int>>> nativeResult = m_Instance->run(algId, nativeMachines, nativeTaskTimes);

        int resultRows = nativeResult.size();
        int resultCols = resultRows > 0 ? nativeResult[0].size() : 0;

        array<Tuple<int>^, 2>^ managedResult = gcnew array<Tuple<int>^, 2>(resultRows, resultCols);

        for (int i = 0; i < resultRows; i++) {
            for (int j = 0; j < resultCols; j++) {
                managedResult[i, j] = gcnew Tuple<int>(nativeResult[i][j].first);
            }
        }

        return managedResult;
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