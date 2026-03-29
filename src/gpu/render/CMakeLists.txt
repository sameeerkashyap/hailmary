# phm_gpu_render — Metal render pipeline (Phase 3)
add_library(phm_gpu_render STATIC
    GLTFLoader.cpp
)
add_library(phm::gpu::render ALIAS phm_gpu_render)

target_include_directories(phm_gpu_render PUBLIC
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(phm_gpu_render PUBLIC
    phm_core
    phm_physics
    metal_cpp::metal_cpp
    tinygltf::tinygltf
    Eigen3::Eigen
)

# Compile render shaders (Phase 3)
# phm_add_metallib(phm_render_metallib "phm_render.metallib"
#     "${CMAKE_CURRENT_SOURCE_DIR}/Shaders.metal"
# )
