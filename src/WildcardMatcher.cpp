/// @file WildcardMatcher.cpp
/// Wildcard matching implementation for process name masks.

#include "WildcardMatcher.hpp"

#include <algorithm>
#include <cwctype>
#include <string>
#include <string_view>

WildcardMatcher::WildcardMatcher(std::wstring_view process_mask) : process_mask_{process_mask} {
    std::ranges::transform(process_mask_, process_mask_.begin(), [](wchar_t ch) {
        return static_cast<wchar_t>(std::towlower(ch));
    });
}

bool WildcardMatcher::matches(std::wstring_view process_name) const {
    std::size_t mask_index{0};
    std::size_t name_index{0};

    std::size_t last_star_index{std::wstring::npos};
    std::size_t name_index_after_star{0};

    while (name_index < process_name.size()) {
        const wchar_t process_name_char{static_cast<wchar_t>(std::towlower(process_name[name_index]))};

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

        if (last_star_index != std::wstring::npos) {
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