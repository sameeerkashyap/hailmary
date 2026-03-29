# phm_physics — header-only, C++20 + <cmath> only, zero external deps
add_library(phm_physics INTERFACE)
add_library(phm::physics ALIAS phm_physics)

target_include_directories(phm_physics INTERFACE
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(phm_physics INTERFACE phm_core)
target_compile_features(phm_physics INTERFACE cxx_std_20)
