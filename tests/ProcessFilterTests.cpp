/// @file ProcessFilterTests.cpp
/// Unit tests for process filtering.

#include "ProcessFilter.hpp"

#include <array>
#include <chrono>
#include <vector>

#include <gtest/gtest.h>

namespace {

using namespace std::chrono_literals;

FilterCriteria make_criteria(std::wstring_view process_mask,
                             std::chrono::minutes minimum_runtime,
                             unsigned long current_process_id = 999) {
    return FilterCriteria{
        .process_mask = process_mask,
        .minimum_runtime = minimum_runtime,
        .current_process_id = current_process_id,
    };
}

} // namespace

TEST(ProcessFilterTests, ReturnsProcessesMatchingNameAndRuntime) {
    const std::array<ProcessInfo, 4> processes{
        ProcessInfo{.process_id = 1, .process_name = L"chrome.exe", .runtime = 10min},
        ProcessInfo{.process_id = 2, .process_name = L"chrome_helper.exe", .runtime = 12min},
        ProcessInfo{.process_id = 3, .process_name = L"notepad.exe", .runtime = 20min},
        ProcessInfo{.process_id = 4, .process_name = L"chrome_short.exe", .runtime = 2min},
    };

    const std::vector<ProcessInfo> result{filter_processes(processes, make_criteria(L"*chrome*", 5min))};

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0].process_id, 1);
    EXPECT_EQ(result[1].process_id, 2);
}

TEST(ProcessFilterTests, FiltersByRuntimeBoundary) {
    const std::array<ProcessInfo, 3> processes{
        ProcessInfo{.process_id = 1, .process_name = L"chrome.exe", .runtime = 4min},
        ProcessInfo{.process_id = 2, .process_name = L"chrome.exe", .runtime = 5min},
        ProcessInfo{.process_id = 3, .process_name = L"chrome.exe", .runtime = 6min},
    };

    const std::vector<ProcessInfo> result{filter_processes(processes, make_criteria(L"chrome.exe", 5min))};

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0].process_id, 2);
    EXPECT_EQ(result[1].process_id, 3);
}

TEST(ProcessFilterTests, ExcludesCurrentProcessId) {
    const std::array<ProcessInfo, 3> processes{
        ProcessInfo{.process_id = 1, .process_name = L"chrome.exe", .runtime = 10min},
        ProcessInfo{.process_id = 2, .process_name = L"chrome.exe", .runtime = 10min},
        ProcessInfo{.process_id = 3, .process_name = L"chrome.exe", .runtime = 10min},
    };

    const std::vector<ProcessInfo> result{filter_processes(processes, make_criteria(L"chrome.exe", 5min, 2))};

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0].process_id, 1);
    EXPECT_EQ(result[1].process_id, 3);
}

TEST(ProcessFilterTests, ReturnsEmptyWhenNoProcessesMatch) {
    const std::array<ProcessInfo, 3> processes{
        ProcessInfo{.process_id = 1, .process_name = L"notepad.exe", .runtime = 10min},
        ProcessInfo{.process_id = 2, .process_name = L"explorer.exe", .runtime = 10min},
        ProcessInfo{.process_id = 3, .process_name = L"chrome.exe", .runtime = 2min},
    };

    const std::vector<ProcessInfo> result{filter_processes(processes, make_criteria(L"*chrome*", 5min))};

    EXPECT_TRUE(result.empty());
}

TEST(ProcessFilterTests, ReturnsEmptyWhenInputIsEmpty) {
    const std::array<ProcessInfo, 0> processes{};

    const std::vector<ProcessInfo> result{filter_processes(processes, make_criteria(L"*", 0min))};

    EXPECT_TRUE(result.empty());
}
