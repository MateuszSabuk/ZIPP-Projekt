#pragma once
#include "ManagedObject.h"
#include "../ZippAlgorithmsLibrary/Manager.h"

using namespace System;
namespace CLI
{
    public ref class SolverManager : public ManagedObject<Manager>
    {
    public:
        SolverManager();
        Tuple<array<int>^, array<int, 2>^>^ generate(int numOfStages, int numOfTasks, int maxNumOfMachinesInStage, int maxTaskTime);
        array<String^>^ getAlgorithmNames();
        System::Collections::Generic::Dictionary<String^, int>^ getAlgorithmParams(int algId);
        void setAlgorithmParams(int algId, System::Collections::Generic::Dictionary<String^, int>^ params);

        array<Tuple<int,int>^, 2>^ run(int algId, array<int>^ machines, array<int, 2>^ taskTimes);
        void cancelAlgorithm();
        
    private:
        static array<int>^ vec2array(const std::vector<int>& data);
        static array<int, 2>^ vec2array2d(std::vector<std::vector<int>>& data);
        static void MarshalString(String^ s, std::string& os);
    };
}