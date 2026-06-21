/// @file Application.hpp
/// Application orchestration API.

#pragma once

#include "CommandLine.hpp"

/// Runs the killer application logic using parsed command-line options.
/// @param options Parsed command-line options.
/// @return Process exit code.
int run_application(const Options& options);