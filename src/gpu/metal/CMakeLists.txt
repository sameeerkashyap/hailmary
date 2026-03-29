# phm_gpu_metal — Metal compute kernels (Phase 4)
add_library(phm_gpu_metal INTERFACE)
add_library(phm::gpu::metal ALIAS phm_gpu_metal)

target_include_directories(phm_gpu_metal INTERFACE
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(phm_gpu_metal INTERFACE
    phm_core
    metal_cpp::metal_cpp
)

# Compile Metal shaders into a .metallib (Phase 4)
# Uncomment when shaders have real implementations:
# phm_add_metallib(phm_compute_metallib "phm_compute.metallib"
#     "${CMAKE_CURRENT_SOURCE_DIR}/ParticleKernel.metal"
#     "${CMAKE_CURRENT_SOURCE_DIR}/SpatialHash.metal"
#     "${CMAKE_CURRENT_SOURCE_DIR}/EnergyAbsorption.metal"
# )
