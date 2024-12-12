#include "SimulatedAnnealing.h"

SimulatedAnnealing::SimulatedAnnealing() : Algorithm("Simulated Annealing") {
	parameters.insert({ "Alpha" , 999 });
	parameters.insert({ "T0", 100 });
	parameters.insert({ "Tk", 10 });
	parameters.insert({ "Iterations", 500 });
	parameters.insert({ "Neighbours num", -1 });
	parameters.insert({ "Cooling schedule", Exp });
}

std::vector<std::vector<std::pair<int, int>>> SimulatedAnnealing::start(std::vector<int>& solvedPermutation, const std::vector<int> &machines, const std::vector<std::vector<int>> &taskTimes) {
	// Validation
	if (parameters.at("Alpha") <= 0 || parameters.at("Alpha") >= 1000) throw "Alpha parameter has to be between 0 and 1000";
	if (parameters.at("T0") <= 0) throw "T0 parameter has to be positive";
	if (parameters.at("Tk") <= 0) throw "Tk parameter has to be positive";
	if (parameters.at("Iterations") <= 0) throw "Iterations parameter has to be positive";
	if (parameters.at("Neighbours num") <= 0 && parameters.at("Neighbours num") != -1) throw "Neighbours num parameter has to be positive or -1";
	if (parameters.at("Cooling schedule") < 0 || parameters.at("Cooling schedule") > 2) throw "Cooling schedule parameter has to be 0, 1 or 2";
	
	static std::random_device rand_device;
	static std::mt19937 rand_gen(rand_device());
	std::uniform_real_distribution<> dis(0.0, 1.0);

	int max_iter = parameters.at("Iterations");
	double min_temp = parameters.at("Tk");
	double current_temp = parameters.at("T0");
	std::vector<int> current_order(taskTimes.size());
	std::vector<std::vector<std::pair<int, int>>> current_schedule;
	
	std::iota(current_order.begin(), current_order.end(), 0);
	std::shuffle(current_order.begin(), current_order.end(), rand_gen);
	int best_cmax;
	std::vector<int> best_order;
	std::vector<std::vector<std::pair<int, int>>> best_schedule = create_schedule(best_cmax, machines, taskTimes, current_order);
	int current_cmax = best_cmax;
	for (size_t iter = 0; iter < max_iter /* && current_temp > min_temp */ ; iter++) {
		if (isCanceled) throw "canceled";

		std::vector<std::vector<int>> neighbourhood = get_neighbours(current_order);
		
		for (std::vector<int> neighbour_order : neighbourhood){
			if (isCanceled) throw "canceled";
			double uniform_rand = dis(rand_gen);
			int new_cmax;
			std::vector<std::vector<std::pair<int, int>>> new_schedule = create_schedule(new_cmax, machines, taskTimes, neighbour_order);
			if (acceptance_propability(current_cmax, new_cmax, current_temp)) {
				current_order = neighbour_order;
				current_cmax = new_cmax;
				current_schedule = new_schedule;
				break;
			}
		}

		if (current_cmax < best_cmax){
			best_cmax = current_cmax;
			best_schedule = current_schedule;
			best_order = current_order;
		}

		current_temp = calculate_temperature(iter);
	}
	solvedPermutation = best_order;
	return best_schedule;
}

std::vector<std::vector<int>> SimulatedAnnealing::get_neighbours(const std::vector<int>& base_order) const {
	int neighbour_num = parameters.at("Neighbours num");
	std::vector<std::vector<int>> neighbours;
	static std::random_device rand_device;
	static std::mt19937 rand_gen(rand_device());
	std::vector<int> swap_order(base_order.size() - 1);
	std::iota(swap_order.begin(), swap_order.end(), 0);
	std::shuffle(swap_order.begin(), swap_order.end(), rand_gen);

	if (neighbour_num < 0)
		neighbour_num = base_order.size();

	for (int neighbour = 0; neighbour < neighbour_num - 1 && neighbour < base_order.size() - 1; neighbour++){
		neighbours.push_back(base_order);
		std::swap(neighbours[neighbour][swap_order[neighbour]], neighbours[neighbour][swap_order[neighbour] + 1]);
	}

	return neighbours;
}

double SimulatedAnnealing::calculate_temperature(const int& iter) const {
	double temperature = 0.0;
	double alpha = static_cast<double>(parameters.at("Alpha")) / 1000.0;
	double T0 = static_cast<double>(parameters.at("T0"));
	
	cooling_schedules cs = cooling_schedules(parameters.at("Cooling schedule"));

	switch (cs)
	{
	case SimulatedAnnealing::Log:
		temperature = T0 / log(1 + iter);
		break;
	case SimulatedAnnealing::Exp:
		temperature = T0 * pow(alpha, iter);
		break;
	case SimulatedAnnealing::Lin:
		temperature = T0 - alpha * iter;
		break;
	}

	return temperature;
}

double SimulatedAnnealing::acceptance_propability(const int& cmax, const int& new_cmax, const int& temperature) const {
	if (new_cmax < cmax)
		return 1.0;
	else return exp((-1.0 * (static_cast<double>(new_cmax) - static_cast<double>(cmax))) / temperature);
}