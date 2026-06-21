/// @file ProcessTerminator.hpp
/// Windows process termination API.

#pragma once

#include "ProcessInfo.hpp"

#include <cstdint>

/// Result status of a process termination attempt.
enum class TerminationStatus {
    Succeeded,
    OpenProcessFailed,
    TerminateProcessFailed,
};

/// Result of a process termination attempt.
struct TerminationResult {
    TerminationStatus status{};
    std::uint32_t error_code{};
};

/// Checks whether a process can be opened with termination rights.
/// @param process_id Process ID to check.
/// @return True if the process can be opened with termination rights.
bool check_termination_access(ProcessId process_id);

/// Attempts to terminate a process by process ID.
/// The terminated process receives exit code 1.
/// @param process_id Process ID to terminate.
/// @return Termination status and Win32 error code for failures.
TerminationResult terminate_process(ProcessId process_id);