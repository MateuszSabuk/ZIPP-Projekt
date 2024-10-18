#pragma once

#include <vector>
#include <string>
#include <unordered_map>

class Algorithm
{
public:
    Algorithm(const std::string& algoName) : name(algoName) {}
    virtual ~Algorithm() = default;

    std::string getName();
    std::unordered_map<std::string, int> getParams();
    void setParams(std::unordered_map<std::string, int> params);
    std::vector<std::vector<std::pair<int, int>>> run(std::vector<int> machines, std::vector<std::vector<int>> taskTimes);
    void cancel();

protected:
    const std::string name;
    bool isCanceled = false;
    std::unordered_map<std::string, int> parameters;

    virtual std::vector<std::vector<std::pair<int, int>>> start(std::vector<int> machines, std::vector<std::vector<int>> taskTimes) = 0;
};
