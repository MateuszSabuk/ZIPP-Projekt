#pragma once
#include "ManagedObject.h"
#include "../ZippAlgorithmsLibrary/ZippAlgorithmsLibrary.h"
using namespace System;
namespace CLI
{
    public ref class Instance : public ManagedObject<ZippAlgorithmsLibrary::Instance>
    {
    public:
        Instance(int numOfStages, int numOfTasks, int maxNumOfMachinesInStage, int maxTaskTime);
        void randomizeArrays(int maxMIS, int maxTT);

        array<int>^ getMachines();
        array<int, 2>^ getTaskTimes();

        virtual String^ ToString() override
        {
            auto machines = this->getMachines();
            auto taskTimes = this->getTaskTimes();

            String^ str = "Instance object:\nStages:\n[";
            for (int i = 0; i < machines->Length-1; i++)
                str += machines[i].ToString() + ", ";
            str += machines[machines->Length - 1].ToString() + "]\nTasks:\n";

            for (int i = 0; i < taskTimes->GetLength(0); i++)
            {
                str += "[";
                for (int j = 0; j < taskTimes->GetLength(1)-1; j++)
                    str += taskTimes[i, j].ToString() + ", ";
                str += taskTimes[i, taskTimes->GetLength(1) - 1].ToString() + "]\n";
            }
            return str;
        }
    };
}