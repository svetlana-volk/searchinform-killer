#include "CommandLine.hpp"

#include <iostream>
#include <string_view>
#include <vector>

int wmain(int argc, wchar_t* argv[]) {
    std::vector<std::wstring_view> args;
    args.reserve(static_cast<std::size_t>(argc));

    for (int arg_index{0}; arg_index < argc; ++arg_index) {
        args.emplace_back(argv[arg_index]);
    }

    const ParseResult parse_result{parse_command_line(args)};

    if (parse_result.help_requested) {
        std::wcout << usage_text();
        return 0;
    }

    if (!parse_result.error_message.empty()) {
        std::wcerr << L"Error: " << parse_result.error_message << L"\n\n";
        std::wcerr << usage_text();
        return 1;
    }

    const Options& options{*parse_result.options};

    std::wcout << L"Process mask: " << options.process_mask << L'\n';
    std::wcout << L"Time minutes: " << options.time_minutes << L'\n';
    std::wcout << L"Dry run: " << std::boolalpha << options.dry_run << L'\n';

    return 0;
}