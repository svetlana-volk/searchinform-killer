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
                             std::chrono::seconds minimum_runtime,
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
        ProcessInfo{.process_id = 1, .process_name = L"chrome.exe", .runtime = 100s},
        ProcessInfo{.process_id = 2, .process_name = L"chrome_helper.exe", .runtime = 120s},
        ProcessInfo{.process_id = 3, .process_name = L"notepad.exe", .runtime = 200s},
        ProcessInfo{.process_id = 4, .process_name = L"chrome_short.exe", .runtime = 20s},
    };

    const std::vector<ProcessInfo> result{filter_processes(processes, make_criteria(L"*chrome*", 50s))};

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0].process_id, 1);
    EXPECT_EQ(result[1].process_id, 2);
}

TEST(ProcessFilterTests, FiltersByRuntimeBoundary) {
    const std::array<ProcessInfo, 3> processes{
        ProcessInfo{.process_id = 1, .process_name = L"chrome.exe", .runtime = 40s},
        ProcessInfo{.process_id = 2, .process_name = L"chrome.exe", .runtime = 50s},
        ProcessInfo{.process_id = 3, .process_name = L"chrome.exe", .runtime = 60s},
    };

    const std::vector<ProcessInfo> result{filter_processes(processes, make_criteria(L"chrome.exe", 50s))};

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0].process_id, 2);
    EXPECT_EQ(result[1].process_id, 3);
}

TEST(ProcessFilterTests, ExcludesCurrentProcessId) {
    const std::array<ProcessInfo, 3> processes{
        ProcessInfo{.process_id = 1, .process_name = L"chrome.exe", .runtime = 100s},
        ProcessInfo{.process_id = 2, .process_name = L"chrome.exe", .runtime = 100s},
        ProcessInfo{.process_id = 3, .process_name = L"chrome.exe", .runtime = 100s},
    };

    const std::vector<ProcessInfo> result{filter_processes(processes, make_criteria(L"chrome.exe", 50s, 2))};

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0].process_id, 1);
    EXPECT_EQ(result[1].process_id, 3);
}

TEST(ProcessFilterTests, ReturnsEmptyWhenNoProcessesMatch) {
    const std::array<ProcessInfo, 3> processes{
        ProcessInfo{.process_id = 1, .process_name = L"notepad.exe", .runtime = 1s},
        ProcessInfo{.process_id = 2, .process_name = L"explorer.exe", .runtime = 10s},
        ProcessInfo{.process_id = 3, .process_name = L"chrome.exe", .runtime = 20s},
    };

    const std::vector<ProcessInfo> result{filter_processes(processes, make_criteria(L"*chrome*", 50s))};

    EXPECT_TRUE(result.empty());
}

TEST(ProcessFilterTests, ReturnsEmptyWhenInputIsEmpty) {
    const std::array<ProcessInfo, 0> processes{};

    const std::vector<ProcessInfo> result{filter_processes(processes, make_criteria(L"*", 0s))};

    EXPECT_TRUE(result.empty());
}
