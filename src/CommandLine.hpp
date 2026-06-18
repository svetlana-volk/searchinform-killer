/// @file CommandLine.hpp
/// Command-line parsing API for the killer utility.

#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>

/// Parsed command-line options for a normal application run.
struct Options {
    std::wstring process_mask;
    int time_minutes;
    bool dry_run{false};
};

/// Result of command-line parsing, including success, help, or error state.
struct ParseResult {
    std::optional<Options> options{};
    bool help_requested{false};
    std::wstring error_message{};
};

/// Parses command-line arguments into application options.
/// Invalid user input is reported through ParseResult::error_message.
/// @param args Command-line arguments, including the executable name at index 0.
/// @return Parse result containing either parsed options, help request, or error message.
ParseResult parse_command_line(std::span<const std::wstring_view> args);

/// Returns command-line usage text.
/// @return Static usage text for displaying help and parse errors.
std::wstring_view usage_text();