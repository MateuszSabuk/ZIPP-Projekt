#include "NEH.h"

NEH::NEH() : Algorithm("NEH") {}

std::vector<int> NEH::sort_by_weight(const std::vector<std::vector<int>>& taskTimes) const {
    std::vector<std::pair<int, int>> flow_times(taskTimes.size());
	std::vector<int> order;

	for (size_t task = 0; task < taskTimes.size(); task++){
		flow_times[task] = std::make_pair<int, int>(task, std::accumulate(taskTimes[task].begin(), taskTimes[task].end(), taskTimes[task][0]));
	}

    std::sort(flow_times.begin(), flow_times.end(), [](const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) { return lhs.second > rhs.second; });

	for (std::pair<int, int> task : flow_times){
		order.push_back(task.first);
	}
	return order;
}

std::vector<std::vector<std::pair<int, int>>> NEH::start(std::vector<int>& solvedPermutation, const std::vector<int> &machines, const std::vector<std::vector<int>> &taskTimes) {
	std::vector<int> sorted_task_order = sort_by_weight(taskTimes);
	std::vector<int> best_order;
	std::vector<int> current_order;
	int min_step_cmax;
	int current_cmax;
	size_t step;
	std::vector<std::vector<std::pair<int, int>>> current_schedule;
	


	for (int task : sorted_task_order) {
		current_order.push_back(task);
		best_order = current_order;
		current_schedule = create_schedule(current_cmax, machines, taskTimes, current_order);
		min_step_cmax = current_cmax;
		step = current_order.size();
		for (size_t offset = 1; offset <= step - 1; offset++){
			std::swap(current_order[step - offset - 1], current_order[step - offset]);
			current_schedule = create_schedule(current_cmax, machines, taskTimes, current_order);
			if (current_cmax < min_step_cmax){
				best_order = current_order;
				min_step_cmax = current_cmax;
			}
		}
		current_order = best_order;
	}

	solvedPermutation = current_order;
	return create_schedule(current_cmax, machines, taskTimes, best_order);
}