#pragma once

#include <vector>
#include <string>
#include <stdexcept> // Used for error propagation
#include <unordered_map> // Used to store the parameters
#include "Cmax.h"

class Algorithm
{
public:
    Algorithm(const std::string& algoName) : name(algoName) {}
    virtual ~Algorithm() = default;

    // Returns the name of the algorithm (name is defined for child class)
    std::string getName();
    // Gets current parameters map from the algorithm object
    std::unordered_map<std::string, int> getParams();
    // Sets parameters of the algorithm object
    void setParams(std::unordered_map<std::string, int> params);
    // Starts the solve using the input data and set parameters
    std::vector<std::vector<std::pair<int, int>>> run(std::vector<int> machines, std::vector<std::vector<int>> taskTimes);
    // Sets the cancel flag
    void cancel();

protected:
    // Defined for each child class (used for selection display
    const std::string name;
    // Checked in algorithms loops to stop the execution of the solver
    bool isCanceled = false;
    // Map of names and values of parameters used for the algorithm
    std::unordered_map<std::string, int> parameters;

    // Overrideable function called by run() after setting cancel flag low
    virtual std::vector<std::vector<std::pair<int, int>>> start(std::vector<int> machines, std::vector<std::vector<int>> taskTimes) = 0;
};
