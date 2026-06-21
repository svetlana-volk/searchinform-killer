/// @file ExitCode.hpp
/// Application process exit codes.

#pragma once

namespace exit_code {

inline constexpr int success{0};
inline constexpr int failure{1};
inline constexpr int partial_failure{2};

} // namespace exit_code