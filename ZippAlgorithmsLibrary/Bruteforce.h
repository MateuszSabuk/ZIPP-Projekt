#pragma once
#include "Algorithm.h"
#include <thread>
#include <mutex>
#include <algorithm>

class Bruteforce : public Algorithm
{
public:
    Bruteforce();
    std::vector<std::vector<std::pair<int, int>>> start(const std::vector<int> &machines, const std::vector<std::vector<int>> &taskTimes) override;
};
