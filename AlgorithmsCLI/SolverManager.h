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
    private:
        static array<int>^ vec2array(const std::vector<int>& data);
        static array<int, 2>^ vec2array2d(std::vector<std::vector<int>>& data);
    };
}