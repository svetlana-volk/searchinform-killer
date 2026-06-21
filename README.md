# killer

## Overview

`killer` is a Windows command-line utility for terminating processes that match a name mask and have been running for at least a given amount of time.

The utility enumerates running Windows processes, obtains their runtime from process creation time, filters them by wildcard process-name mask and minimum running time, and then either previews or terminates the matching processes.

## Features

* Wildcard process-name matching with `*` and `?`.
* Case-insensitive process-name matching.
* Wide-character process-name handling, including non-ASCII names such as Cyrillic process names.
* Runtime filtering by minimum process age in minutes.
* `--dry-run` mode for safe preview before termination.
* Self-protection: the running `killer.exe` process is excluded from termination candidates.
* Summary output with matched, terminated, skipped, and failed process counts.
* Explicit exit codes for success, application/input failure, and partial operation failure.

## Requirements

* Windows 10/11
* C++20-capable MSVC compiler
* Visual Studio 2022 C++ Build Tools or newer, with Windows SDK
* CMake 3.20 or newer
* clang-format, for source formatting
* clang-tidy, for additional static analysis

Tested locally with:

* Visual Studio 2022 Build Tools
* MSVC 19.44
* CMake 3.31.6
* clang-format 19.1.5
* clang-tidy 19.1.5

Build scripts are intended to be run from **x64 Native Tools Command Prompt for Visual Studio**.

## Build

Debug build:

```bat
scripts\build-debug.bat
```

Debug builds enable MSVC static analysis.

Release build:

```bat
scripts\build-release.bat
```

Application builds do not require GoogleTest.

## Run

After building, run the application from the corresponding build directory.

> Warning: without `--dry-run`, matching processes are terminated immediately and without confirmation.

Debug build example:

```bat
build-debug\killer.exe --procmask "*chrome*" --time 5
```

Release build example:

```bat
build-release\killer.exe --procmask "*chrome*" --time 5
```

Command-line format:

```bat
killer.exe --procmask <mask> --time <minutes> [--dry-run]
```

Supported options:

```text
--procmask <mask>   Non-empty process name mask. Matching is case-insensitive.
                    Supports exact names and wildcard masks.
                    Wildcard characters: * matches any sequence, ? matches one character.
                    Other characters are matched literally.
                    Examples: chrome.exe, CHROME.EXE, *Chrome*, chrome?.exe, "My App.exe"

--time <minutes>    Minimum process running time in minutes.
                    Must be a positive decimal integer.

--dry-run           Preview processes that would be terminated without terminating them.

--help, -h          Show usage information.
```

Examples:

```bat
killer.exe --procmask "*chrome*" --time 5 --dry-run
killer.exe --procmask "chrome.exe" --time 10 --dry-run
killer.exe --procmask "*chrome*" --time 5
killer.exe --help
```

## Output

In dry-run mode, the application prints matching processes that can be opened with termination rights, but does not terminate them.

Example dry-run summary:

```text
Dry run: no processes will be terminated.
PID: 1234  Name: chrome.exe  Runtime: 600 sec

Summary:
  Matched process candidates: 1
  Processes that would be terminated: 1
  Skipped because termination access is unavailable: 0
```

In normal mode, the application attempts to terminate matching processes and prints a summary of successful and failed termination attempts.

Example normal-run summary:

```text
PID: 1234  Name: chrome.exe  Runtime: 600 sec

Summary:
  Matched process candidates: 1
  Terminated: 1
  Failed: 0
```

## Behavior notes

Process running time is measured from process creation time, not CPU time.

The running `killer.exe` process is always excluded from termination candidates, even if it matches the provided process mask.

Process names are handled as wide strings using Windows wide-character process APIs. On Windows, this means UTF-16 process-name handling. Non-ASCII process names, such as Cyrillic names, are supported by the matching logic.

## Known limitations

Process enumeration and termination are separate steps. A process may exit or change state between enumeration, filtering, and termination. In that case, termination may fail and the failure is reported in the summary.

`--dry-run` checks termination access at the time of the dry-run check. This does not guarantee that a later normal run will be able to terminate the same processes.

Some processes may be skipped or may fail to terminate because the current user does not have sufficient permissions to open them with termination rights.

## Exit codes

```text
0   Success.
    Includes help, successful dry-run, no matching processes, and successful termination run.

1   Application or input failure.
    Includes invalid command line, internal parse-result inconsistency, or inability to enumerate processes.

2   Partial operation failure.
    One or more matching processes could not be terminated.
```

## Tests

Run tests:

```bat
scripts\run-tests.bat
```

Command-line parsing, wildcard matching, and process filtering are covered by:

```text
tests\CommandLineTests.cpp
tests\WildcardMatcherTests.cpp
tests\ProcessFilterTests.cpp
```

By contrast, Windows process enumeration and termination wrap WinAPI calls directly and are verified manually rather than with unit tests.

Tests use GoogleTest. The test build first tries to use a locally available GoogleTest package. If GoogleTest is not found, CMake uses FetchContent to download GoogleTest for the test build.

The downloaded GoogleTest sources are cached inside the test build directory:

```text
build-test\_deps\
```

This dependency is used only for tests and is not linked into the final `killer.exe` application.

## Manual smoke check

A safe dry-run smoke check can be run after a Debug build:

```bat
build-debug\killer.exe --procmask "*" --time 1 --dry-run
```

This command exercises command-line parsing, process enumeration, process filtering, and termination-access checking without terminating processes.

## Format

Format project source and test files:

```bat
scripts\format.bat
```

The script runs `clang-format`, uses formatting rules from `.clang-format`, and modifies files in place.

## Static analysis

The project uses two static-analysis paths.

### MSVC `/analyze`

Debug builds enable MSVC static analysis via `/analyze`:

```bat
scripts\build-debug.bat
```

This check is part of the regular Debug build.

### clang-tidy

clang-tidy can be run separately for project source files:

```bat
scripts\tidy.bat
```

The clang-tidy script configures a separate `build-tidy` directory and uses its `compile_commands.json` for analysis. This keeps clang-tidy configuration separate from the Debug build, where MSVC `/analyze` is enabled.

clang-tidy is used as an additional diagnostic tool. Its warnings are not treated as errors automatically; reported issues are reviewed and fixed manually when they are relevant to the project code.

## Project layout

```text
killer/
├── CMakeLists.txt
├── README.md
├── scripts/
├── src/
└── tests/
```
