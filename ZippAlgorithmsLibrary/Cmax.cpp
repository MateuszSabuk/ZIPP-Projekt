#include "Cmax.h"

std::vector<std::vector<std::pair<int, int>>> create_schedule(int& cmax, const std::vector<int>& machines, const std::vector<std::vector<int>>& task_times, const std::vector<int>& order) {
    std::vector<std::vector<std::pair<int, int>>> schedule;
    std::vector<std::vector<int>> free_times; //vector with times at which given machine in job is free
    std::vector<int> prev_step_end_times; //vector holding times at which tasks ended previous step

    //create schedule and vector for previous steps filled with zeros
    for (size_t task = 0; task < task_times.size(); task++) {
        schedule.push_back(std::vector<std::pair<int, int>>());
        prev_step_end_times.push_back(0);
        for (size_t step = 0; step < task_times[0].size(); step++) {
            schedule[task].push_back(std::make_pair(0, 0));
        }
    }

    //create zeros vector for machines end times
    for (size_t step = 0; step < task_times[0].size(); step++) {
        free_times.push_back(std::vector<int>());
        for (int machine = 0; machine < machines[step]; machine++)
            free_times[step].push_back(0);
    }

    //iterate by task order to fit schedule "to the left"
    for (int task : order) {

        //iterate steps of a given task
        for (size_t step = 0; step < task_times[task].size(); step++) {
            //find first empty machine iterator and determine its index
            std::vector<int>::iterator first_free_machine = std::min_element(free_times[step].begin(), free_times[step].end());
            int free_machine_no = std::distance(free_times[step].begin(), first_free_machine);

            //mark in schedule that this machine will be used and calculate start time for the step
            schedule[task][step].first = free_machine_no;
            schedule[task][step].second = std::max(prev_step_end_times[task], *first_free_machine);

            //save end time of that step and update machine time
            prev_step_end_times[task] = schedule[task][step].second + task_times[task][step];
            free_times[step][free_machine_no] = prev_step_end_times[task];
        }
    }

    //find machine which works last, and its end time is cmax 
    std::vector<int>::iterator last_machine = std::max_element(prev_step_end_times.begin(), prev_step_end_times.end());
    cmax = *last_machine;

    return schedule;
}
