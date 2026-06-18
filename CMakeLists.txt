cmake_minimum_required(VERSION 3.20)

project(killer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_MSVC_ANALYZE "Enable MSVC static analysis" OFF)

add_executable(killer
    src/main.cpp
)

target_include_directories(killer PRIVATE src)

if(MSVC AND ENABLE_MSVC_ANALYZE)
    target_compile_options(killer PRIVATE /analyze)
endif()