/// @file ProcessFilter.cpp
/// Process filtering implementation.

#include "ProcessFilter.hpp"

#include "WildcardMatcher.hpp"

#include <span>
#include <vector>

std::vector<ProcessInfo> filter_processes(std::span<const ProcessInfo> processes, const FilterCriteria& criteria) {
    const WildcardMatcher matcher{criteria.process_mask};

    std::vector<ProcessInfo> matched_processes{};
    matched_processes.reserve(processes.size());

    for (const ProcessInfo& process : processes) {
        if (process.process_id == criteria.current_process_id) {
            continue;
        }

        if (process.runtime < criteria.minimum_runtime) {
            continue;
        }

        if (!matcher.matches(process.process_name)) {
            continue;
        }

        matched_processes.push_back(process);
    }

    return matched_processes;
}