/// @file main.cpp
/// Application entry point.

#include "Application.hpp"
#include "CommandLine.hpp"
#include "ExitCode.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>
#include <vector>

namespace {

/// Creates non-owning views over wide command-line arguments.
/// @param argc Number of command-line arguments.
/// @param argv Wide command-line argument array.
/// @return Argument views suitable for command-line parsing.
std::vector<std::wstring_view> make_argument_views(int argc, wchar_t* argv[]) {
    std::vector<std::wstring_view> args{};
    args.reserve(static_cast<std::size_t>(argc));

    for (int arg_index{0}; arg_index < argc; ++arg_index) {
        args.emplace_back(argv[arg_index]);
    }

    return args;
}
} // namespace

int wmain(int argc, wchar_t* argv[]) {
    const std::vector<std::wstring_view> args{make_argument_views(argc, argv)};
    const ParseResult parse_result{parse_command_line(args)};

    if (parse_result.help_requested) {
        std::wcout << usage_text();
        return exit_code::success;
    }

    if (!parse_result.error_message.empty()) {
        std::wcerr << L"Error: " << parse_result.error_message << L"\n\n";
        std::wcerr << usage_text();
        return exit_code::failure;
    }

    if (!parse_result.options.has_value()) {
        std::wcerr << L"Internal error: parsed options are missing.\n";
        return exit_code::failure;
    }

    return run_application(*parse_result.options);
}