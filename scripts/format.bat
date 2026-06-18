@echo off
setlocal

rem -----------------------------------------------------------------------------
rem format.bat
rem
rem Formats project source files with clang-format.
rem This script modifies files in place.
rem
rem Run from an environment where clang-format is available in PATH.
rem -----------------------------------------------------------------------------

pushd "%~dp0.."

if not exist CMakeLists.txt (
    echo Error: CMakeLists.txt not found.
    echo This script must be located in the scripts directory under the project root.
    popd
    exit /b 1
)

where clang-format >nul 2>nul
if errorlevel 1 (
    echo Error: clang-format not found.
    popd
    exit /b 1
)

for /R src %%f in (*.cpp *.hpp) do clang-format -i "%%f"

if exist tests (
    for /R tests %%f in (*.cpp *.hpp) do clang-format -i "%%f"
)

popd
exit /b 0