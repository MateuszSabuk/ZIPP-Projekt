#include "Algorithm.h"

std::string Algorithm::getName()
{
    return name;
}

std::unordered_map<std::string, int> Algorithm::getParams()
{
	return parameters;
}

void Algorithm::setParams(std::unordered_map<std::string, int> params)
{
    for (const auto& pair : params) {
        const std::string& key = pair.first;
        const int value = pair.second;

        auto it = parameters.find(key);
        if (it != parameters.end()) {
            it->second = value;
        }
    }
}

void Algorithm::cancel()
{
    isCanceled = true;
}

std::vector<std::vector<std::pair<int, int>>> Algorithm::run(std::vector<int> machines, std::vector<std::vector<int>> taskTimes) {
    isCanceled = false;
    return start(machines, taskTimes);
}