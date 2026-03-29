# phm_core — header-only, C++20 stdlib only, zero external deps
add_library(phm_core INTERFACE)
add_library(phm::core ALIAS phm_core)

target_include_directories(phm_core INTERFACE
    "${CMAKE_SOURCE_DIR}/src"
)

target_compile_features(phm_core INTERFACE cxx_std_20)
