/// @file ProcessFilter.hpp
/// Process filtering API for selecting matching process candidates.

#pragma once

#include "ProcessInfo.hpp"

#include <chrono>
#include <span>
#include <string_view>
#include <vector>

/// Criteria used to select matching process candidates.
struct FilterCriteria {
    std::wstring_view process_mask{};
    std::chrono::seconds minimum_runtime{};
    ProcessId current_process_id{};
};

/// Filters processes by name mask and minimum runtime.
/// The process mask is expected to be validated by command-line parsing.
/// The current process ID is always excluded to prevent the utility from
/// selecting itself as a termination candidate.
/// @param processes Processes to filter.
/// @param criteria Filtering criteria.
/// @return Processes matching the mask and runtime thresho
std::vector<ProcessInfo> filter_processes(std::span<const ProcessInfo> processes, const FilterCriteria& criteria);