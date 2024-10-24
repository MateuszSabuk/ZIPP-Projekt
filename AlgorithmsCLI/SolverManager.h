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
        // Returns a Tuple of arrays:
        // - array of numbers of machines for each stage
        // - array for each task containing an array with times for each stage
        Tuple<array<int>^, array<int, 2>^>^ generate(int numOfStages, int numOfTasks, int maxNumOfMachinesInStage, int maxTaskTime);
        
        // Get names of all available algorithms
        array<String^>^ getAlgorithmNames();
        // Get parameters of the chosen algorithm
        System::Collections::Generic::Dictionary<String^, int>^ getAlgorithmParams(int algId);
        // Set the parameters of the chosen algorithm
        void setAlgorithmParams(int algId, System::Collections::Generic::Dictionary<String^, int>^ params);

        // Run the chosen algorithm solve
        array<Tuple<int,int>^, 2>^ run(int algId, array<int>^ machines, array<int, 2>^ taskTimes);
        // Cancel the solve of the running algorithm
        void cancelAlgorithm();
        
    private:
        // Conversion helper functions
        static array<int>^ vec2array(const std::vector<int>& data);
        static array<int, 2>^ vec2array2d(std::vector<std::vector<int>>& data);
        static void MarshalString(String^ s, std::string& os);
    };
}