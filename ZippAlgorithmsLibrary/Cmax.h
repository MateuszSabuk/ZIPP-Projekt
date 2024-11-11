#pragma once
#include <vector>
#include <algorithm>

std::vector<std::vector<std::pair<int, int>>> create_schedule(int& cmax, const std::vector<int>& machines, const std::vector<std::vector<int>>& task_times, const std::vector<int>& order);