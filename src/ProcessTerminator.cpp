/// @file ProcessTerminator.cpp
/// Windows process termination implementation.

#include "ProcessTerminator.hpp"

#include "UniqueHandle.hpp"

#include <cstdint>

#include <windows.h>

namespace {

constexpr std::uint32_t forced_termination_exit_code{1};

/// Opens a process with termination rights.
/// @param process_id Process ID to open.
/// @return Owning handle to the process, or an invalid handle on failure.
UniqueHandle open_process_for_termination(ProcessId process_id) {
    return UniqueHandle{OpenProcess(PROCESS_TERMINATE, FALSE, process_id)};
}

} // namespace

bool check_termination_access(ProcessId process_id) {
    const UniqueHandle process_handle{open_process_for_termination(process_id)};

    return process_handle.is_valid();
}

TerminationResult terminate_process(ProcessId process_id) {
    const UniqueHandle process_handle{open_process_for_termination(process_id)};

    if (!process_handle.is_valid()) {
        return TerminationResult{
            .status = TerminationStatus::OpenProcessFailed,
            .error_code = GetLastError(),
        };
    }

    if (TerminateProcess(process_handle.get(), forced_termination_exit_code) == FALSE) {
        return TerminationResult{
            .status = TerminationStatus::TerminateProcessFailed,
            .error_code = GetLastError(),
        };
    }

    return TerminationResult{
        .status = TerminationStatus::Succeeded,
        .error_code = 0,
    };
}