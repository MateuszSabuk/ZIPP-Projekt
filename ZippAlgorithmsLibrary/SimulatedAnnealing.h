#pragma once
#include "Algorithm.h"
#include <cmath>
#include <random>
#include <numeric>


class SimulatedAnnealing : public Algorithm
{
public:
    SimulatedAnnealing();
    std::vector<std::vector<std::pair<int, int>>> start(std::vector<int>& solvedPermutation, const std::vector<int> &machines, const std::vector<std::vector<int>> &taskTimes) override;

private:
    std::vector<std::vector<int>> get_neighbours(const std::vector<int>& base_order) const;

    double calculate_temperature(const int & iter) const;

    double acceptance_propability(const int& cmax, const int& new_cmax, const int& temperature) const;


    enum cooling_schedules{Log, Exp, Lin};
};

