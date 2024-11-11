#pragma once
#include <vector>
#include <algorithm>

// Sets cmax to the time of execution of the schedule
// Returns a 2D vector of size n*s of std::pair<int,int>
// where:
// n = number of tasks
// s = number of stages
// pair.first = machine index
// pair.second = task execution start time
std::vector<std::vector<std::pair<int, int>>> create_schedule(int& cmax, const std::vector<int>& machines, const std::vector<std::vector<int>>& task_times, const std::vector<int>& order);