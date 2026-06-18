@echo off
setlocal

rem -----------------------------------------------------------------------------
rem build-release.bat
rem
rem Configures and builds the Release version of the project.
rem This is the build intended for final manual verification.
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

where cl >nul 2>nul
if errorlevel 1 (
    echo Error: MSVC compiler not found.
    echo Please run this script from x64 Native Tools Command Prompt for Visual Studio.
    popd
    exit /b 1
)

where cmake >nul 2>nul
if errorlevel 1 (
    echo Error: CMake not found.
    popd
    exit /b 1
)

cmake -B build-release -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    popd
    exit /b 1
)

cmake --build build-release
set BUILD_RESULT=%errorlevel%

popd
exit /b %BUILD_RESULT%