cmake_minimum_required(VERSION 3.27)
project(phm_engine VERSION 0.1.0 LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Build options ──────────────────────────────────────────────────────────────
option(PHM_BUILD_METAL      "Build Metal GPU modules (requires vendor/metal-cpp)" ${APPLE})
option(PHM_BUILD_WEBGPU     "Build WebGPU module (requires Dawn)"                  OFF)
option(PHM_BUILD_TESTS      "Build unit tests"                                      ON)
option(PHM_BUILD_BENCHMARKS "Build micro-benchmarks"                                ON)

# ── Dependencies (FetchContent) ────────────────────────────────────────────────
include(cmake/Deps.cmake)

# ── metal-cpp detection ────────────────────────────────────────────────────────
if(PHM_BUILD_METAL)
    include(cmake/MetalCpp.cmake)
endif()

# ── Subdirectories ─────────────────────────────────────────────────────────────
add_subdirectory(src)

if(PHM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(PHM_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# ── Compiler flags ─────────────────────────────────────────────────────────────
add_compile_options(
    -Wall -Wextra -Wpedantic
    -Wno-unused-parameter
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-g -O0 -fsanitize=address,undefined>
)
add_link_options(
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)
