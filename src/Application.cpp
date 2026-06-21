/// @file Application.cpp
/// Application orchestration implementation.

#include "Application.hpp"

#include "ExitCode.hpp"
#include "ProcessFilter.hpp"
#include "ProcessTerminator.hpp"
#include "WinProcessEnumerator.hpp"

#include <chrono>
#include <cstddef>
#include <iostream>
#include <span>
#include <vector>

#include <windows.h>

namespace {

/// Runtime statistics collected during one application run.
struct RunStatistics {
    std::size_t matched_processes{};
    std::size_t processed_processes{};
    std::size_t failed_processes{};
};

/// Prints process information to standard output.
/// @param process Process information to print.
void print_process(const ProcessInfo& process) {
    std::wcout << L"PID: " << process.process_id << L"  Name: " << process.process_name << L"  Runtime: "
               << process.runtime.count() << L" sec\n";
}

/// Prints final run statistics.
/// @param statistics Run statistics to print.
/// @param dry_run Whether the current run is a dry run.
void print_statistics(const RunStatistics& statistics, bool dry_run) {
    std::wcout << L"\nSummary:\n";
    std::wcout << L"  Matched process candidates: " << statistics.matched_processes << L'\n';

    if (dry_run) {
        std::wcout << L"  Processes that would be terminated: " << statistics.processed_processes << L'\n';
        std::wcout << L"  Skipped because termination access is unavailable: " << statistics.failed_processes << L'\n';
        return;
    }

    std::wcout << L"  Terminated: " << statistics.processed_processes << L'\n';
    std::wcout << L"  Failed: " << statistics.failed_processes << L'\n';
}

/// Runs dry-run processing for matching processes.
/// @param matching_processes Processes matching command-line criteria.
/// @param statistics Statistics updated by this run.
void run_dry_run(std::span<const ProcessInfo> matching_processes, RunStatistics& statistics) {
    std::wcout << L"Dry run: no processes will be terminated.\n";

    for (const ProcessInfo& process : matching_processes) {
        if (!check_termination_access(process.process_id)) {
            ++statistics.failed_processes;
            continue;
        }

        ++statistics.processed_processes;
        print_process(process);
    }

    print_statistics(statistics, true);
}

/// Terminates matching processes and records results.
/// @param matching_processes Processes matching command-line criteria.
/// @param statistics Statistics updated by this run.
void run_termination(std::span<const ProcessInfo> matching_processes, RunStatistics& statistics) {
    for (const ProcessInfo& process : matching_processes) {
        const TerminationResult result{terminate_process(process.process_id)};

        if (result.status == TerminationStatus::Succeeded) {
            ++statistics.processed_processes;
            print_process(process);
            continue;
        }

        ++statistics.failed_processes;

        std::wcerr << L"Failed to terminate PID " << process.process_id << L" (" << process.process_name
                   << L"), Win32 error " << result.error_code << L'\n';
    }

    print_statistics(statistics, false);
}

} // namespace

int run_application(const Options& options) {
    const std::vector<ProcessInfo> processes{enumerate_processes()};

    if (processes.empty()) {
        std::wcerr << L"Error: no processes could be enumerated.\n";
        return exit_code::failure;
    }

    const std::vector<ProcessInfo> matching_processes{
        filter_processes(processes,
                         FilterCriteria{
                             .process_mask = options.process_mask,
                             .minimum_runtime = std::chrono::minutes{options.time_minutes},
                             .current_process_id = static_cast<ProcessId>(GetCurrentProcessId()),
                         })};

    RunStatistics statistics{.matched_processes = matching_processes.size()};

    if (options.dry_run) {
        run_dry_run(matching_processes, statistics);
        return exit_code::success;
    }

    run_termination(matching_processes, statistics);

    return statistics.failed_processes == 0 ? exit_code::success : exit_code::partial_failure;
}