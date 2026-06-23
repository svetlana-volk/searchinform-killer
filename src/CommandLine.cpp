/// @file CommandLine.cpp
/// Command-line parsing implementation for the killer utility.

#include "CommandLine.hpp"

#include <algorithm>
#include <cstddef>
#include <cwctype>
#include <format>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace {

constexpr std::wstring_view process_mask_option{L"--procmask"};
constexpr std::wstring_view time_option{L"--time"};
constexpr std::wstring_view dry_run_option{L"--dry-run"};
constexpr std::wstring_view help_option{L"--help"};
constexpr std::wstring_view short_help_option{L"-h"};

/// Creates an error message for a duplicate option.
/// @param option_name Option name.
/// @return Formatted error message.
std::wstring duplicate_option_message(std::wstring_view option_name) {
    return std::format(L"Duplicate {} option.", option_name);
}

/// Creates an error message for a missing option value.
/// @param option_name Option name.
/// @return Formatted error message.
std::wstring missing_value_message(std::wstring_view option_name) {
    return std::format(L"Missing value for {}.", option_name);
}

/// Creates an error message for a missing required option.
/// @param option_name Option name.
/// @return Formatted error message.
std::wstring missing_required_message(std::wstring_view option_name) {
    return std::format(L"Missing required {} option.", option_name);
}

/// Creates an error message for an invalid positive integer.
/// @param option_name Option name.
/// @return Formatted error message.
std::wstring invalid_positive_int_message(std::wstring_view option_name) {
    return std::format(L"{} must be a positive integer (1 to 2147483647).", option_name);
}

/// Creates an error message for an empty process mask.
/// @param option_name Option name.
/// @return Formatted error message.
std::wstring empty_process_mask_message(std::wstring_view option_name) {
    return std::format(L"{} must not be empty.", option_name);
}

/// Creates an error message for an unknown argument.
/// @param arg Unknown command-line argument.
/// @return Formatted error message.
std::wstring unknown_argument_message(std::wstring_view arg) {
    return std::format(L"Unknown argument: {}", arg);
}

/// Checks whether an argument requests help.
/// @param arg Command-line argument.
/// @return True if the argument is a help option.
bool is_help_option(std::wstring_view arg) {
    return arg == help_option || arg == short_help_option;
}

/// Checks whether an argument is a supported option.
/// @param arg Command-line argument.
/// @return True if the argument is a known option.
bool is_known_option(std::wstring_view arg) {
    return arg == process_mask_option || arg == time_option || arg == dry_run_option || arg == help_option ||
           arg == short_help_option;
}

/// Checks whether an option has a following value.
/// @param args Command-line arguments.
/// @param arg_index Option argument index.
/// @return True if the next argument exists and is not another known option.
bool has_required_option_value(std::span<const std::wstring_view> args, std::size_t arg_index) {
    return arg_index + 1 < args.size() && !is_known_option(args[arg_index + 1]);
}

/// Parses a positive decimal integer.
/// @param value Text value to parse.
/// @return Parsed integer, or std::nullopt if the value is invalid.
std::optional<int> parse_positive_int(std::wstring_view value) {
    if (value.empty() || !std::ranges::all_of(value, [](wchar_t ch) {
            return std::iswdigit(ch) != 0;
        })) {
        return std::nullopt;
    }

    try {
        const int parsed_value{std::stoi(std::wstring{value})};

        return (parsed_value > 0) ? std::optional<int>{parsed_value} : std::nullopt;

    } catch (const std::out_of_range&) {
        return std::nullopt;
    }
}

/// Checks whether a process mask is valid.
/// @param value Process mask value.
/// @return True if the process mask is non-empty.
bool is_valid_process_mask(std::wstring_view value) {
    return !value.empty();
}

} // namespace

std::wstring_view usage_text() {
    return L"Usage: killer.exe --procmask <mask> --time <minutes> [--dry-run]\n"
           L"\n"
           L"  --procmask <mask>   Process name mask, supports * and ?\n"
           L"                      Examples: \"chrome.exe\", \"*chrome*\", \"chrome?.exe\"\n"
           L"  --time <minutes>    Minimum running time in minutes, positive decimal integer\n"
           L"  --dry-run           Report matching processes without terminating them\n"
           L"  --help, -h          Show this message\n";
}

ParseResult parse_command_line(std::span<const std::wstring_view> args) {
    std::optional<std::wstring> process_mask{};
    std::optional<int> time_minutes{};
    bool dry_run{false};

    for (std::size_t arg_index{1}; arg_index < args.size(); ++arg_index) {
        const std::wstring_view arg{args[arg_index]};

        if (is_help_option(arg)) {
            return HelpRequested{};
        }

        if (arg == process_mask_option) {
            if (process_mask.has_value()) {
                return ParseError{.error_message = duplicate_option_message(process_mask_option)};
            }

            if (!has_required_option_value(args, arg_index)) {
                return ParseError{.error_message = missing_value_message(process_mask_option)};
            }

            const std::wstring_view mask_value{args[arg_index + 1]};

            if (!is_valid_process_mask(mask_value)) {
                return ParseError{.error_message = empty_process_mask_message(process_mask_option)};
            }

            process_mask = std::wstring{mask_value};

            ++arg_index;
            continue;
        }

        if (arg == time_option) {
            if (time_minutes.has_value()) {
                return ParseError{.error_message = duplicate_option_message(time_option)};
            }

            if (!has_required_option_value(args, arg_index)) {
                return ParseError{.error_message = missing_value_message(time_option)};
            }

            const auto parsed_time{parse_positive_int(args[arg_index + 1])};
            if (!parsed_time.has_value()) {
                return ParseError{.error_message = invalid_positive_int_message(time_option)};
            }

            time_minutes = *parsed_time;
            ++arg_index;
            continue;
        }

        if (arg == dry_run_option) {
            if (dry_run) {
                return ParseError{.error_message = duplicate_option_message(dry_run_option)};
            }

            dry_run = true;
            continue;
        }

        return ParseError{.error_message = unknown_argument_message(arg)};
    }

    if (!process_mask.has_value()) {
        return ParseError{.error_message = missing_required_message(process_mask_option)};
    }

    if (!time_minutes.has_value()) {
        return ParseError{.error_message = missing_required_message(time_option)};
    }

    return Options{
        .process_mask = std::move(*process_mask),
        .time_minutes = *time_minutes,
        .dry_run = dry_run,
    };
}