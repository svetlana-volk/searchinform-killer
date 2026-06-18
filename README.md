# killer

Windows command-line utility for terminating processes by name mask and runtime threshold.

## Requirements

* Windows 10/11
* Visual Studio 2022 C++ Build Tools or newer, with MSVC and Windows SDK
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
--procmask <mask>   Non-empty process name mask. Supports exact names and wildcard masks.
                    Wildcard characters: * matches any sequence, ? matches one character.
                    Other characters are matched literally.
                    Examples: chrome.exe, *chrome*, chrome?.exe, "My App.exe"

--time <minutes>    Minimum process running time in minutes.
                    Must be a positive decimal integer.

--dry-run           Print matching processes without terminating them.

--help, -h          Show usage information.
```

Examples:

```bat
killer.exe --procmask "*chrome*" --time 5
killer.exe --procmask "chrome.exe" --time 10 --dry-run
killer.exe --help
```

## Tests

Run tests:

```bat
scripts\run-tests.bat
```

Command-line parsing is covered by:

```text
tests\CommandLineTests.cpp
```

Tests use GoogleTest. The test build first tries to use a locally available GoogleTest package. If GoogleTest is not found, CMake uses FetchContent to download GoogleTest for the test build.

The downloaded GoogleTest sources are cached inside the test build directory:

```text
build-test\_deps\
```

This dependency is used only for tests and is not linked into the final `killer.exe` application.

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
