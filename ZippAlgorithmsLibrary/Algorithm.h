#pragma once

#include <vector>
#include <string>

class Algorithm
{
public:
	const std::string name;
	virtual std::vector<std::vector<std::pair<int, int>>> run(std::vector<int> machines, std::vector<std::vector<int>> taskTimes) = 0;
	virtual ~Algorithm() = default;
};
