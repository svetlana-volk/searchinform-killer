/// @file ProcessInfo.hpp
/// Process information model used by process enumeration and filtering.

#pragma once

#include <chrono>
#include <cstdint>
#include <string>

/// Process identifier used by the killer utility.
using ProcessId = std::uint32_t;

/// Basic process information required by the killer utility.
struct ProcessInfo {
    ProcessId process_id{};
    std::wstring process_name{};
    std::chrono::seconds runtime{};
};