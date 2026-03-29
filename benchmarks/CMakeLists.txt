add_executable(phm_benchmarks
    bench_allocators.cpp
    bench_particles.cpp
)

target_link_libraries(phm_benchmarks PRIVATE
    phm_core
    phm_physics
    benchmark::benchmark_main
)

set_target_properties(phm_benchmarks PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
