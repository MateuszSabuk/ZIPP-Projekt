#include "Bruteforce.h"
#include "Windows.h"

Bruteforce::Bruteforce() : Algorithm("Bruteforce")
{
	parameters.insert({ "Test parameter" , 1 });
	parameters.insert({ "Test prameter" , 42 });
	parameters.insert({ "Test paramter" , 5 });
	parameters.insert({ "Test paraeter" , 1 });
	parameters.insert({ "Test aparameter" , 1 });
	parameters.insert({ "Test psrameter" , 42 });
	parameters.insert({ "Test padramter" , 5 });
	parameters.insert({ "Test parfaeter" , 1 });
	parameters.insert({ "Test paragmeter" , 1 });
	parameters.insert({ "Test pramehter" , 42 });
	parameters.insert({ "Test paramtjer" , 5 });
	parameters.insert({ "Test paraetekr" , 1 });
}

std::vector<std::vector<std::pair<int, int>>> Bruteforce::start(std::vector<int> machines, std::vector<std::vector<int>> taskTimes)
{
	while (!isCanceled)
	Sleep(1);
	return std::vector<std::vector<std::pair<int, int>>>();
}
