# phm_webgpu — WebGPU browser port (Phase 6)
# Requires Dawn: add via FetchContent when this module is activated.
# cmake -DPHM_BUILD_WEBGPU=ON ..

message(STATUS "[PHM] WebGPU module enabled — ensure Dawn is available")

add_library(phm_webgpu INTERFACE)
add_library(phm::webgpu ALIAS phm_webgpu)

target_include_directories(phm_webgpu INTERFACE
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(phm_webgpu INTERFACE
    phm_core
    phm_physics
    # dawn::webgpu_dawn  ← add when Dawn FetchContent is wired up
)
