@echo off
setlocal

rem -----------------------------------------------------------------------------
rem run-tests.bat
rem
rem Configures, builds, and runs project tests.
rem
rem Tests use GoogleTest. CMake handles GoogleTest discovery and FetchContent
rem fallback for the test build.
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

if not exist build-test\CMakeCache.txt (
    echo Configuring test build...

    cmake -B build-test -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DKILLER_FETCH_GTEST=ON
    if errorlevel 1 (
        popd
        exit /b 1
    )
)

cmake --build build-test --target killer_tests
if errorlevel 1 (
    popd
    exit /b 1
)

build-test\killer_tests.exe --gtest_color=yes
set TEST_RESULT=%errorlevel%

popd
exit /b %TEST_RESULT%