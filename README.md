# killer

Windows command-line utility for terminating processes by name mask and runtime threshold.

The expected command-line interface follows the assignment example:

```bat
killer.exe --procmask "*chrome*" --time 5
```

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

Release build:

```bat
scripts\build-release.bat
```

## Format

Format source files:

```bat
scripts\format.bat
```

The format script modifies files in place.

## Static analysis

The project uses two static-analysis paths.

### MSVC `/analyze`

Debug builds enable MSVC static analysis via `/analyze`:

```bat
scripts\build-debug.bat
```

This check is part of the regular Debug build.

### clang-tidy

clang-tidy can be run separately:

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
