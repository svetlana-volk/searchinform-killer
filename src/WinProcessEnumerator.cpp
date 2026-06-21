/// @file WinProcessEnumerator.cpp
/// Windows process enumeration implementation.

#include "WinProcessEnumerator.hpp"

#include "UniqueHandle.hpp"

#include <chrono>
#include <cstdint>
#include <optional>
#include <ratio>
#include <vector>

#include <tlhelp32.h>
#include <windows.h>

namespace {

/// Strong wrapper for Windows FILETIME ticks.
struct FiletimeTicks {
    std::uint64_t value{};
};

/// Converts Windows FILETIME to 100-nanosecond ticks.
/// @param file_time Windows FILETIME value.
/// @return FILETIME tick count.
FiletimeTicks filetime_to_ticks(const FILETIME& file_time) {
    ULARGE_INTEGER value{};
    value.LowPart = file_time.dwLowDateTime;
    value.HighPart = file_time.dwHighDateTime;

    return FiletimeTicks{.value = value.QuadPart};
}

/// Gets process runtime from its creation time.
/// @param process_id Process ID to inspect.
/// @param current_ticks Current system time expressed as FILETIME ticks.
/// @return Process runtime, or std::nullopt if runtime cannot be obtained.
std::optional<std::chrono::seconds> get_process_runtime(ProcessId process_id, FiletimeTicks current_ticks) {
    const UniqueHandle process_handle{OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, process_id)};

    if (!process_handle.is_valid()) {
        return std::nullopt;
    }

    FILETIME creation_time{};
    FILETIME exit_time{};
    FILETIME kernel_time{};
    FILETIME user_time{};

    if (!GetProcessTimes(process_handle.get(), &creation_time, &exit_time, &kernel_time, &user_time)) {
        return std::nullopt;
    }

    const FiletimeTicks process_ticks{filetime_to_ticks(creation_time)};

    if (current_ticks.value < process_ticks.value) {
        return std::nullopt;
    }

    const std::uint64_t runtime_ticks{current_ticks.value - process_ticks.value};

    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::duration<std::uint64_t, std::ratio<1, 10'000'000>>{runtime_ticks});
}

} // namespace

std::vector<ProcessInfo> enumerate_processes() {
    std::vector<ProcessInfo> processes{};

    const UniqueHandle snapshot{CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)};

    if (!snapshot.is_valid()) {
        return processes;
    }

    FILETIME current_time{};
    GetSystemTimeAsFileTime(&current_time);
    const FiletimeTicks current_ticks{filetime_to_ticks(current_time)};

    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);

    if (!Process32FirstW(snapshot.get(), &entry)) {
        return processes;
    }

    do {
        const std::optional<std::chrono::seconds> runtime{get_process_runtime(entry.th32ProcessID, current_ticks)};

        if (!runtime.has_value()) {
            continue;
        }

        processes.push_back(ProcessInfo{
            .process_id = entry.th32ProcessID,
            .process_name = entry.szExeFile,
            .runtime = *runtime,
        });

    } while (Process32NextW(snapshot.get(), &entry));

    return processes;
}