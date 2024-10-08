#pragma once

#include <vector>
#include <string>
#include <unordered_map>

class Algorithm
{
public:
	const std::string name;
	std::unordered_map<std::string, int> getParams();
	void setParams(std::unordered_map<std::string, int> params);
	virtual std::vector<std::vector<std::pair<int, int>>> run(std::vector<int> machines, std::vector<std::vector<int>> taskTimes) = 0;
	virtual ~Algorithm() = default;
private:
	std::unordered_map<std::string, int> parameters;
};
