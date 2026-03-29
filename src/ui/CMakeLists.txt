# phm_ui — Dear ImGui dashboard (Phase 5)
add_library(phm_ui INTERFACE)
add_library(phm::ui ALIAS phm_ui)

target_include_directories(phm_ui INTERFACE
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(phm_ui INTERFACE
    phm_physics
    imgui::imgui
    metal_cpp::metal_cpp
)
