cmake_minimum_required(VERSION 3.20)

project(killer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_MSVC_ANALYZE "Enable MSVC static analysis" OFF)
option(BUILD_TESTING "Build tests" OFF)
option(KILLER_FETCH_GTEST "Download GoogleTest if it is not found locally" OFF)

add_library(killer_core STATIC
    src/CommandLine.cpp
    src/WildcardMatcher.cpp
    src/ProcessFilter.cpp
)

target_include_directories(killer_core PUBLIC
    src
)

add_executable(killer
    src/main.cpp
)

target_link_libraries(killer PRIVATE
    killer_core
)

if(MSVC AND ENABLE_MSVC_ANALYZE)
    target_compile_options(killer_core PRIVATE /analyze)
    target_compile_options(killer PRIVATE /analyze)
endif()

if(BUILD_TESTING)
    enable_testing()

    find_package(GTest CONFIG QUIET)

    if(NOT GTest_FOUND)
        if(KILLER_FETCH_GTEST)
            include(FetchContent)

            set(GOOGLETEST_SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/googletest-src")

            if(EXISTS "${GOOGLETEST_SOURCE_DIR}/CMakeLists.txt")
                message(STATUS "Using cached GoogleTest from ${GOOGLETEST_SOURCE_DIR}.")
            else()
                message(STATUS "GoogleTest package was not found by find_package.")
                message(STATUS "Downloading GoogleTest via CMake FetchContent...")
                message(STATUS "This step requires network access on the first test build.")
            endif()

            FetchContent_Declare(
                googletest
                URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
                DOWNLOAD_EXTRACT_TIMESTAMP TRUE
                INACTIVITY_TIMEOUT 20
            )

            set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
            set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
            set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

            FetchContent_MakeAvailable(googletest)
        else()
            message(FATAL_ERROR
                "GoogleTest was not found. Install GoogleTest or configure with "
                "-DKILLER_FETCH_GTEST=ON to download it via CMake FetchContent."
            )
        endif()
    endif()

    add_executable(killer_tests
        tests/CommandLineTests.cpp
        tests/WildcardMatcherTests.cpp
        tests/ProcessFilterTests.cpp
    )

    target_link_libraries(killer_tests PRIVATE
        killer_core
        GTest::gtest_main
    )

    if(MSVC AND ENABLE_MSVC_ANALYZE)
        target_compile_options(killer_tests PRIVATE /analyze)
    endif()

    include(GoogleTest)
    gtest_discover_tests(killer_tests)
endif()