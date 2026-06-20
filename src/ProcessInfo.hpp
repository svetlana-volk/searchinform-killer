/// @file ProcessInfo.hpp
/// Process information model used by process enumeration and filtering.

#pragma once

#include <chrono>
#include <string>

/// Basic process information required by the killer utility.
struct ProcessInfo {
    unsigned long process_id{};
    std::wstring process_name{};
    std::chrono::minutes runtime{};
};