#include "Bruteforce.h"

Bruteforce::Bruteforce() : Algorithm("Bruteforce")
{
	parameters.insert({ "Test parameter" , 1 });
}

std::vector<std::vector<std::pair<int, int>>> Bruteforce::run(std::vector<int> machines, std::vector<std::vector<int>> taskTimes)
{
	return std::vector<std::vector<std::pair<int, int>>>();
}
