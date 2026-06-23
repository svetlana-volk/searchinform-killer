/// @file WildcardMatcher.cpp
/// Wildcard matching implementation for process name masks.

#include "WildcardMatcher.hpp"

#include <string>
#include <string_view>

#include <windows.h>

namespace {

/// Converts wide string to lowercase using Windows character mapping.
/// @param value Text to convert.
/// @return Lowercase text.
std::wstring to_lowercase(std::wstring_view value) {
    std::wstring result{value};

    if (!result.empty()) {
        CharLowerBuffW(result.data(), static_cast<DWORD>(result.size()));
    }

    return result;
}

} // namespace

WildcardMatcher::WildcardMatcher(std::wstring_view process_mask) : process_mask_{to_lowercase(process_mask)} {}

bool WildcardMatcher::matches(std::wstring_view process_name) const {
    const std::wstring lower_process_name{to_lowercase(process_name)};

    std::size_t mask_index{0};
    std::size_t name_index{0};

    std::size_t last_star_index{std::wstring_view::npos};
    std::size_t name_index_after_star{0};

    while (name_index < lower_process_name.size()) {
        const wchar_t process_name_char{lower_process_name[name_index]};

        if (mask_index < process_mask_.size() &&
            (process_mask_[mask_index] == L'?' || process_mask_[mask_index] == process_name_char)) {
            ++mask_index;
            ++name_index;
            continue;
        }

        if (mask_index < process_mask_.size() && process_mask_[mask_index] == L'*') {
            last_star_index = mask_index;
            name_index_after_star = name_index;
            ++mask_index;
            continue;
        }

        if (last_star_index != std::wstring_view::npos) {
            mask_index = last_star_index + 1;
            ++name_index_after_star;
            name_index = name_index_after_star;
            continue;
        }

        return false;
    }

    while (mask_index < process_mask_.size() && process_mask_[mask_index] == L'*') {
        ++mask_index;
    }

    return mask_index == process_mask_.size();
}