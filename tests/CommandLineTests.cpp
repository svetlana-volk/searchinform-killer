/// @file CommandLineTests.cpp
/// Unit tests for command-line parsing.

#include "CommandLine.hpp"

#include <array>
#include <string_view>
#include <variant>

#include <gtest/gtest.h>

namespace {

/// Returns parsed options from a successful parse result.
/// @param result Parse result to inspect.
/// @return Parsed options.
const Options& get_options(const ParseResult& result) {
    return std::get<Options>(result);
}

/// Checks that parse result is a help request.
/// @param result Parse result to inspect.
void expect_help_result(const ParseResult& result) {
    EXPECT_TRUE(std::holds_alternative<HelpRequested>(result));
}

/// Checks that parse result is an error.
/// @param result Parse result to inspect.
void expect_parse_error(const ParseResult& result) {
    const ParseError* error{std::get_if<ParseError>(&result)};

    ASSERT_NE(error, nullptr);
    EXPECT_FALSE(error->error_message.empty());
}

} // namespace

TEST(CommandLineTests, ParsesRequiredArguments) {
    const std::array<std::wstring_view, 5> args{
        L"killer.exe",
        L"--procmask",
        L"*chrome*",
        L"--time",
        L"5",
    };

    const ParseResult result{parse_command_line(args)};
    const Options& options{get_options(result)};

    EXPECT_EQ(options.process_mask, L"*chrome*");
    EXPECT_EQ(options.time_minutes, 5);
    EXPECT_FALSE(options.dry_run);
}

TEST(CommandLineTests, ParsesMaskWithQuestionMark) {
    const std::array<std::wstring_view, 5> args{
        L"killer.exe",
        L"--procmask",
        L"chrome?.exe",
        L"--time",
        L"10",
    };

    const ParseResult result{parse_command_line(args)};
    const Options& options{get_options(result)};

    EXPECT_EQ(options.process_mask, L"chrome?.exe");
    EXPECT_EQ(options.time_minutes, 10);
    EXPECT_FALSE(options.dry_run);
}

TEST(CommandLineTests, ParsesExactProcessNameAsMask) {
    const std::array<std::wstring_view, 5> args{
        L"killer.exe",
        L"--procmask",
        L"chrome.exe",
        L"--time",
        L"10",
    };

    const ParseResult result{parse_command_line(args)};
    const Options& options{get_options(result)};

    EXPECT_EQ(options.process_mask, L"chrome.exe");
    EXPECT_EQ(options.time_minutes, 10);
    EXPECT_FALSE(options.dry_run);
}

TEST(CommandLineTests, ParsesMaskWithAllowedSeparators) {
    const std::array<std::wstring_view, 5> args{
        L"killer.exe",
        L"--procmask",
        L"chrome-1_t.exe",
        L"--time",
        L"10",
    };

    const ParseResult result{parse_command_line(args)};
    const Options& options{get_options(result)};

    EXPECT_EQ(options.process_mask, L"chrome-1_t.exe");
    EXPECT_EQ(options.time_minutes, 10);
    EXPECT_FALSE(options.dry_run);
}

TEST(CommandLineTests, ParsesDryRun) {
    const std::array<std::wstring_view, 6> args{
        L"killer.exe",
        L"--procmask",
        L"chrome.exe",
        L"--time",
        L"10",
        L"--dry-run",
    };

    const ParseResult result{parse_command_line(args)};
    const Options& options{get_options(result)};

    EXPECT_TRUE(options.dry_run);
    EXPECT_EQ(options.process_mask, L"chrome.exe");
    EXPECT_EQ(options.time_minutes, 10);
}

TEST(CommandLineTests, ParsesLongHelp) {
    const std::array<std::wstring_view, 2> args{
        L"killer.exe",
        L"--help",
    };

    const ParseResult result{parse_command_line(args)};

    expect_help_result(result);
}

TEST(CommandLineTests, ParsesShortHelp) {
    const std::array<std::wstring_view, 2> args{
        L"killer.exe",
        L"-h",
    };

    const ParseResult result{parse_command_line(args)};

    expect_help_result(result);
}

TEST(CommandLineTests, RejectsMissingProcessMask) {
    const std::array<std::wstring_view, 3> args{
        L"killer.exe",
        L"--time",
        L"10",
    };

    const ParseResult result{parse_command_line(args)};

    expect_parse_error(result);
}

TEST(CommandLineTests, RejectsMissingTime) {
    const std::array<std::wstring_view, 3> args{
        L"killer.exe",
        L"--procmask",
        L"chrome.exe",
    };

    const ParseResult result{parse_command_line(args)};

    expect_parse_error(result);
}

TEST(CommandLineTests, RejectsMissingProcessMaskValueBeforeOption) {
    const std::array<std::wstring_view, 4> args{
        L"killer.exe",
        L"--procmask",
        L"--time",
        L"10",
    };

    const ParseResult result{parse_command_line(args)};

    expect_parse_error(result);
}

TEST(CommandLineTests, RejectsMissingProcessMaskValueAtEnd) {
    const std::array<std::wstring_view, 2> args{
        L"killer.exe",
        L"--procmask",
    };

    const ParseResult result{parse_command_line(args)};

    expect_parse_error(result);
}

TEST(CommandLineTests, RejectsDuplicateProcessMask) {
    const std::array<std::wstring_view, 7> args{
        L"killer.exe",
        L"--procmask",
        L"chrome1.exe",
        L"--time",
        L"10",
        L"--procmask",
        L"chrome2.exe",
    };

    const ParseResult result{parse_command_line(args)};

    expect_parse_error(result);
}

TEST(CommandLineTests, RejectsMissingTimeValueBeforeOption) {
    const std::array<std::wstring_view, 4> args{
        L"killer.exe",
        L"--time",
        L"--procmask",
        L"chrome.exe",
    };

    const ParseResult result{parse_command_line(args)};

    expect_parse_error(result);
}

TEST(CommandLineTests, RejectsMissingTimeValueAtEnd) {
    const std::array<std::wstring_view, 2> args{
        L"killer.exe",
        L"--time",
    };

    const ParseResult result{parse_command_line(args)};

    expect_parse_error(result);
}

TEST(CommandLineTests, RejectsDuplicateTime) {
    const std::array<std::wstring_view, 7> args{
        L"killer.exe",
        L"--time",
        L"10",
        L"--procmask",
        L"chrome2.exe",
        L"--time",
        L"20",
    };

    const ParseResult result{parse_command_line(args)};

    expect_parse_error(result);
}

TEST(CommandLineTests, RejectsDuplicateDryRun) {
    const std::array<std::wstring_view, 7> args{
        L"killer.exe",
        L"--procmask",
        L"chrome.exe",
        L"--dry-run",
        L"--time",
        L"10",
        L"--dry-run",
    };

    const ParseResult result{parse_command_line(args)};

    expect_parse_error(result);
}

class InvalidTimeValueTest : public ::testing::TestWithParam<std::wstring_view> {};

TEST_P(InvalidTimeValueTest, RejectsInvalidTimeValue) {
    const std::array<std::wstring_view, 5> args{
        L"killer.exe",
        L"--procmask",
        L"chrome.exe",
        L"--time",
        GetParam(),
    };

    const ParseResult result{parse_command_line(args)};

    expect_parse_error(result);
}

INSTANTIATE_TEST_SUITE_P(
    CommandLineTests,
    InvalidTimeValueTest,
    ::testing::Values(L"", L"0", L"-5", L"2.5", L"0x10", L"10min", L"abc", L"999999999999999999999999999999"));

TEST(CommandLineTests, RejectsEmptyProcessMask) {
    const std::array<std::wstring_view, 5> args{
        L"killer.exe",
        L"--procmask",
        L"",
        L"--time",
        L"10",
    };

    const ParseResult result{parse_command_line(args)};

    expect_parse_error(result);
}

TEST(CommandLineTests, ParsesMaskWithSpace) {
    const std::array<std::wstring_view, 5> args{
        L"killer.exe",
        L"--procmask",
        L"My App.exe",
        L"--time",
        L"10",
    };

    const ParseResult result{parse_command_line(args)};
    const Options& options{get_options(result)};

    EXPECT_EQ(options.process_mask, L"My App.exe");
    EXPECT_EQ(options.time_minutes, 10);
    EXPECT_FALSE(options.dry_run);
}

TEST(CommandLineTests, RejectsUnknownArgument) {
    const std::array<std::wstring_view, 6> args{
        L"killer.exe",
        L"--procmask",
        L"chrome.exe",
        L"--time",
        L"10",
        L"--unknown",
    };

    const ParseResult result{parse_command_line(args)};

    expect_parse_error(result);
}

TEST(CommandLineTests, HelpTakesPrecedenceOverFollowingArguments) {
    const std::array<std::wstring_view, 3> args{
        L"killer.exe",
        L"--help",
        L"--unknown",
    };

    const ParseResult result{parse_command_line(args)};

    expect_help_result(result);
}