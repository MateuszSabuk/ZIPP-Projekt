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

    System::Collections::Generic::Dictionary<String^, int>^ SolverManager::getAlgorithmParams(int algId)
    {
        auto dict = gcnew System::Collections::Generic::Dictionary<String^, int>();
        for (const auto& pair : m_Instance->getAlgorithmParams(algId)) {
            dict->Add(gcnew String(pair.first.c_str()), pair.second);
        }
        return dict;
    }

    void SolverManager::setAlgorithmParams(int algId, System::Collections::Generic::Dictionary<String^, int>^ params)
    {
        std::unordered_map<std::string, int> paramsMap;
        for each (System::Collections::Generic::KeyValuePair<String^, int> pair in params)
        {
            // Use marshal_as to convert String^ to std::string
            std::string key;
            MarshalString(pair.Key, key);
            paramsMap.insert({ key, pair.Value });
        }
        m_Instance->setAlgorithmParams(algId, paramsMap);
    }

    array<Tuple<int, int>^, 2>^ SolverManager::run(int algId, array<int>^ machines, array<int, 2>^ taskTimes)
    {
        // Convert managed array<int> to std::vector<int>
        std::vector<int> machineVec(machines->Length);
        for (int i = 0; i < machines->Length; i++)
        {
            machineVec[i] = machines[i];
        }

        // Convert managed array<int, 2> to std::vector<std::vector<int>>
        std::vector<std::vector<int>> taskTimesVec;
        for (int i = 0; i < taskTimes->GetLength(0); i++)
        {
            std::vector<int> taskRow;
            for (int j = 0; j < taskTimes->GetLength(1); j++)
            {
                taskRow.push_back(int(taskTimes[i, j]));
            }
            taskTimesVec.push_back(taskRow);
        }

        // Call the C++ function
        try {
            auto solution = m_Instance->run(algId, machineVec, taskTimesVec);

            // Convert std::vector<std::vector<std::pair<int, int>>> to array<Tuple<int, int>^, 2>
            int rows = static_cast<int>(solution.size());
            int cols = static_cast<int>(solution.empty() ? 0 : solution[0].size());

            array<Tuple<int, int>^, 2>^ result = gcnew array<Tuple<int, int>^, 2>(rows, cols);

            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result[i, j] = gcnew Tuple<int, int>(solution[i][j].first, solution[i][j].second);
                }
            }

            return result;
        }
        catch (...)
        {
            // TODO Error handling
        }

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
        array<int, 2>^ managedArray = gcnew array<int, 2>(n, s);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < s; j++)
                managedArray[i, j] = data[i][j];
        return managedArray;
    }

    void SolverManager::MarshalString(String^ s, std::string& os) {
        using namespace Runtime::InteropServices;
        const char* chars =
            (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
        os = chars;
        Marshal::FreeHGlobal(IntPtr((void*)chars));
    }
}