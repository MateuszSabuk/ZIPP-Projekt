#include "Algorithm.h"

std::string Algorithm::getName()
{
    return name;
}

std::unordered_map<std::string, int> Algorithm::getParams()
{
	return parameters;
}

void Algorithm::setParams(std::unordered_map<std::string, int> params)
{
    // Loop over all arguments
    for (const auto& pair : params) {
        const std::string& key = pair.first;
        const int value = pair.second;

        // If parameter exists set it to a given value
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            it->second = value;
        }
    }
}

void Algorithm::cancel()
{
    isCanceled = true;
}

std::vector<std::vector<std::pair<int, int>>> Algorithm::run(const std::vector<int> &machines, const std::vector<std::vector<int>> &taskTimes) {
    isCanceled = false;
    return start(machines, taskTimes);
}

std::vector<std::vector<std::pair<int, int>>> Algorithm::create_schedule(int& cmax, const std::vector<int>& machines, const std::vector<std::vector<int>>& task_times, const std::vector<int>& order) {
    int num_of_stages = machines.size();
    int num_of_tasks = task_times.size();

    auto schedule = std::vector<std::vector<std::pair<int, int>>>(num_of_tasks, std::vector<std::pair<int, int>>(num_of_stages, std::make_pair(0, 0)));
    auto free_times = std::vector<std::vector<int>>(num_of_stages); //vector with times at which given machine in job is free
    auto prev_step_end_times = std::vector<int>(num_of_tasks, 0); //vector holding times at which tasks ended in previous step

    // Fill free_times vector with zeroes
    for (size_t step = 0; step < num_of_stages; step++) {
        free_times[step].resize(machines[step], 0);
    }

    //iterate by task order to fit schedule "to the left"
    for (size_t step = 0; step < num_of_stages; step++) {
        //iterate steps of a given task
        int prev_task = -1;
        for (int task : order) {
            //find first empty machine iterator and determine its index
            std::vector<int>::iterator first_free_machine = std::min_element(free_times[step].begin(), free_times[step].end());
            int free_machine_no = std::distance(free_times[step].begin(), first_free_machine);

            //make sure the new task won't start before the previous one
            if (prev_task != -1) prev_step_end_times[task] = std::max(prev_step_end_times[task], schedule[prev_task][step].second);

            //mark in schedule that this machine will be used and calculate start time for the step
            schedule[task][step].first = free_machine_no;
            schedule[task][step].second = std::max(prev_step_end_times[task], *first_free_machine);

            //save end time of that step and update machine time
            prev_step_end_times[task] = schedule[task][step].second + task_times[task][step];
            free_times[step][free_machine_no] = prev_step_end_times[task];

            prev_task = task;
        }
    }

    //find machine which works last, and its end time is cmax 
    std::vector<int>::iterator last_machine = std::max_element(prev_step_end_times.begin(), prev_step_end_times.end());
    cmax = *last_machine;

    return schedule;
}
