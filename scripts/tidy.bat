@echo off
setlocal

rem -----------------------------------------------------------------------------
rem tidy.bat
rem
rem Configures a separate clang-tidy build directory and runs clang-tidy for
rem project source files.
rem
rem Tests are intentionally not analyzed here because they depend on GoogleTest
rem and use framework macros that do not follow the project naming rules.
rem
rem MSVC /analyze is intentionally not enabled here because clang-tidy consumes
rem compile_commands.json and may not understand MSVC analyzer-specific flags.
rem
rem Run from x64 Native Tools Command Prompt for Visual Studio.
rem -----------------------------------------------------------------------------

pushd "%~dp0.."

if not exist CMakeLists.txt (
    echo Error: CMakeLists.txt not found.
    echo This script must be located in the scripts directory under the project root.
    popd
    exit /b 1
)

where cmake >nul 2>nul
if errorlevel 1 (
    echo Error: CMake not found.
    popd
    exit /b 1
)

where clang-tidy >nul 2>nul
if errorlevel 1 (
    echo Error: clang-tidy not found.
    popd
    exit /b 1
)

cmake -B build-tidy -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
if errorlevel 1 (
    popd
    exit /b 1
)

set TIDY_RESULT=0

for /R src %%f in (*.cpp) do (
    clang-tidy -quiet "%%f" -p build-tidy
    if errorlevel 1 set TIDY_RESULT=1
)

if %TIDY_RESULT% neq 0 (
    echo clang-tidy failed.
    popd
    exit /b 1
)

echo clang-tidy passed.

popd
exit /b 0