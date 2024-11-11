#pragma once
#include "Algorithm.h"
#include <deque>

class Bruteforce : public Algorithm
{
public:
    Bruteforce();
    std::vector<std::vector<std::pair<int, int>>> start(std::vector<int> machines, std::vector<std::vector<int>> taskTimes) override;
private:
    std::vector<std::vector<int>> quickPerm(int length);
};
