/// @file WinProcessEnumerator.hpp
/// Windows process enumeration API.

#pragma once

#include "ProcessInfo.hpp"

#include <vector>

/// Enumerates currently running Windows processes.
/// Processes that cannot be opened or inspected may be skipped by the implementation.
/// @return List of processes with process ID, executable name, and runtime.
std::vector<ProcessInfo> enumerate_processes();