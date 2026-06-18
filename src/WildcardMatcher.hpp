/// @file WildcardMatcher.hpp
/// Wildcard matching API for process name masks.

#pragma once

#include <string>
#include <string_view>

/// Case-insensitive wildcard matcher for process name masks.
/// Supports two wildcard characters:
/// - '*' matches zero or more characters.
/// - '?' matches exactly one character.
class WildcardMatcher {
public:
    /// Creates a matcher for the specified wildcard mask.
    /// @param process_mask Process name mask, such as "chrome.exe", "*chrome*", or "chrome?.exe".
    explicit WildcardMatcher(std::wstring_view process_mask);

    /// Checks whether a process name matches the stored wildcard mask.
    /// @param process_name Process name to check.
    /// @return True if the process name matches the mask; otherwise false.
    bool matches(std::wstring_view process_name) const;

private:
    std::wstring process_mask_;
};