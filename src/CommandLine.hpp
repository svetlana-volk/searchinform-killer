/// @file CommandLine.hpp
/// Command-line parsing API for the killer utility.

#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <variant>

/// Parsed command-line options for a normal application run.
struct Options {
    std::wstring process_mask;
    int time_minutes;
    bool dry_run{false};
};

/// Indicates that the user requested command line help.
struct HelpRequested {};

/// Describes an invalid command line.
struct ParseError {
    std::wstring error_message{};
};

/// Result of command-line parsing or help, or error state.
using ParseResult = std::variant<Options, HelpRequested, ParseError>;

/// Parses command-line arguments into application options.
/// Invalid user input is reported through ParseResult::error_message.
/// @param args Command-line arguments, including the executable name at index 0.
/// @return Parse result containing either parsed options, help request, or error message.
ParseResult parse_command_line(std::span<const std::wstring_view> args);

/// Returns command-line usage text.
/// @return Static usage text for displaying help and parse errors.
std::wstring_view usage_text();