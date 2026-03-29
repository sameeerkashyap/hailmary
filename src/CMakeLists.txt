add_subdirectory(core)
add_subdirectory(physics)

if(PHM_BUILD_METAL)
    add_subdirectory(gpu)
    add_subdirectory(ui)
endif()

if(PHM_BUILD_WEBGPU)
    add_subdirectory(webgpu)
endif()

# ── Main executable ────────────────────────────────────────────────────────────
add_executable(phm_engine main.cpp)

target_link_libraries(phm_engine PRIVATE
    phm_core
    phm_physics
    nlohmann_json::nlohmann_json
)

if(PHM_BUILD_METAL)
    target_link_libraries(phm_engine PRIVATE phm_gpu_render phm_ui)
endif()

set_target_properties(phm_engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
