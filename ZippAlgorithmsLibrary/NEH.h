#pragma once
#include "Algorithm.h"
#include <algorithm>
#include <numeric>

class NEH : public Algorithm
{
public:
	NEH();
	std::vector<std::vector<std::pair<int, int>>> start(std::vector<int>& solvedPermutation, const std::vector<int> &machines, const std::vector<std::vector<int>> &taskTimes) override;
private:
	std::vector<int> sort_by_weight(const std::vector<std::vector<int>>& taskTimes) const;

};

