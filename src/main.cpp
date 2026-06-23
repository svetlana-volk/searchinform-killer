/// @file main.cpp
/// Application entry point.

#include "Application.hpp"
#include "CommandLine.hpp"
#include "ExitCode.hpp"

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <string_view>
#include <vector>

#include <fcntl.h>
#include <io.h>

namespace {

/// Configures one output stream for wide-character text.
/// @param stream Output stream to configure.
void configure_wide_output_stream(FILE* stream) {
    const int file_descriptor{_fileno(stream)};

    if (file_descriptor == -1) {
        return;
    }

    if (_isatty(file_descriptor) != 0) {
        static_cast<void>(_setmode(file_descriptor, _O_U16TEXT));
        return;
    }

    static_cast<void>(_setmode(file_descriptor, _O_U8TEXT));
}

/// Configures standard output streams for wide-character text.
void configure_wide_output() {
    configure_wide_output_stream(stdout);
    configure_wide_output_stream(stderr);
}

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
    configure_wide_output();

    const std::vector<std::wstring_view> args{make_argument_views(argc, argv)};
    const ParseResult parse_result{parse_command_line(args)};

    if (std::holds_alternative<HelpRequested>(parse_result)) {
        std::wcout << usage_text();
        return exit_code::success;
    }

    if (const auto* error = std::get_if<ParseError>(&parse_result)) {
        std::wcerr << L"Error: " << error->error_message << L"\n\n";
        std::wcerr << usage_text();
        return exit_code::failure;
    }

    const Options& options = std::get<Options>(parse_result);
    return run_application(options);
}