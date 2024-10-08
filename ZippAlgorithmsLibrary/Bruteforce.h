#pragma once
#include "Algorithm.h"

class Bruteforce : public Algorithm
{
public:
    Bruteforce() : Algorithm("Bruteforce") {}
    std::vector<std::vector<std::pair<int, int>>> run(std::vector<int> machines, std::vector<std::vector<int>> taskTimes) override;
};
