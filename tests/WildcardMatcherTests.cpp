/// @file WildcardMatcherTests.cpp
/// Unit tests for process name wildcard matching.

#include "WildcardMatcher.hpp"

#include <gtest/gtest.h>

TEST(WildcardMatcherTests, MatchesExactProcessName) {
    const WildcardMatcher matcher{L"chrome.exe"};

    EXPECT_TRUE(matcher.matches(L"chrome.exe"));
}

TEST(WildcardMatcherTests, MatchesCaseInsensitiveProcessName) {
    const WildcardMatcher matcher{L"chrome.exe"};

    EXPECT_TRUE(matcher.matches(L"CHROME.EXE"));
    EXPECT_TRUE(matcher.matches(L"Chrome.Exe"));
}

TEST(WildcardMatcherTests, RejectsDifferentExactProcessName) {
    const WildcardMatcher matcher{L"chrome.exe"};

    EXPECT_FALSE(matcher.matches(L"notepad.exe"));
}

TEST(WildcardMatcherTests, StarMatchesAnySequence) {
    const WildcardMatcher matcher{L"*chrome*"};

    EXPECT_TRUE(matcher.matches(L"chrome.exe"));
    EXPECT_TRUE(matcher.matches(L"my-chrome-helper.exe"));
    EXPECT_TRUE(matcher.matches(L"old_chrome"));
}

TEST(WildcardMatcherTests, StarCanMatchEmptySequence) {
    const WildcardMatcher matcher{L"chrome*.exe"};

    EXPECT_TRUE(matcher.matches(L"chrome.exe"));
    EXPECT_TRUE(matcher.matches(L"chrome_child.exe"));
}

TEST(WildcardMatcherTests, QuestionMarkMatchesExactlyOneCharacter) {
    const WildcardMatcher matcher{L"chrome?.exe"};

    EXPECT_TRUE(matcher.matches(L"chrome1.exe"));
    EXPECT_TRUE(matcher.matches(L"chromeA.exe"));

    EXPECT_FALSE(matcher.matches(L"chrome.exe"));
    EXPECT_FALSE(matcher.matches(L"chrome12.exe"));
}

TEST(WildcardMatcherTests, MatchesExtensionMask) {
    const WildcardMatcher matcher{L"*.exe"};

    EXPECT_TRUE(matcher.matches(L"chrome.exe"));
    EXPECT_TRUE(matcher.matches(L"notepad.exe"));
    EXPECT_FALSE(matcher.matches(L"chrome.dll"));
}

TEST(WildcardMatcherTests, StarOnlyMatchesAnyProcessName) {
    const WildcardMatcher matcher{L"*"};

    EXPECT_TRUE(matcher.matches(L"chrome.exe"));
    EXPECT_TRUE(matcher.matches(L""));
}

TEST(WildcardMatcherTests, EmptyMaskMatchesOnlyEmptyName) {
    const WildcardMatcher matcher{L""};

    EXPECT_TRUE(matcher.matches(L""));
    EXPECT_FALSE(matcher.matches(L"chrome.exe"));
}

TEST(WildcardMatcherTests, MatchesProcessNameWithSpaces) {
    const WildcardMatcher matcher{L"My App.exe"};

    EXPECT_TRUE(matcher.matches(L"My App.exe"));
    EXPECT_TRUE(matcher.matches(L"my app.exe"));
}

TEST(WildcardMatcherTests, MatchesCyrillicProcessName) {
    const WildcardMatcher matcher{L"*тест*"};

    EXPECT_TRUE(matcher.matches(L"мой_тест.exe"));
    EXPECT_FALSE(matcher.matches(L"chrome.exe"));
}

TEST(WildcardMatcherTests, MatchesCyrillicInsensitiveProcessName) {
    const WildcardMatcher matcher{L"*тест*"};

    EXPECT_TRUE(matcher.matches(L"мой_Тест.exe"));
    EXPECT_FALSE(matcher.matches(L"chrome.exe"));
}

TEST(WildcardMatcherTests, ConsecutiveStarsBehaveLikeSingleStar) {
    const WildcardMatcher matcher{L"**chrome***"};

    EXPECT_TRUE(matcher.matches(L"chrome.exe"));
    EXPECT_TRUE(matcher.matches(L"my-chrome-helper.exe"));
    EXPECT_FALSE(matcher.matches(L"notepad.exe"));
}