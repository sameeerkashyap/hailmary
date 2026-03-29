include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# ── Eigen 3.4 ─────────────────────────────────────────────────────────────────
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG        3.4.0
    GIT_SHALLOW    TRUE
)
set(EIGEN_BUILD_DOC        OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING          OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_PKGCONFIG  OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(eigen)

# ── nlohmann/json ─────────────────────────────────────────────────────────────
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(nlohmann_json)

# ── tinygltf ──────────────────────────────────────────────────────────────────
FetchContent_Declare(
    tinygltf
    GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
    GIT_TAG        v2.9.3
    GIT_SHALLOW    TRUE
)
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL "" FORCE)
set(TINYGLTF_HEADER_ONLY          ON  CACHE BOOL "" FORCE)
set(TINYGLTF_INSTALL              OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(tinygltf)

# Expose tinygltf as an interface target (header-only)
if(NOT TARGET tinygltf::tinygltf)
    add_library(tinygltf::tinygltf INTERFACE IMPORTED GLOBAL)
    target_include_directories(tinygltf::tinygltf INTERFACE
        ${tinygltf_SOURCE_DIR}
    )
    target_compile_definitions(tinygltf::tinygltf INTERFACE
        TINYGLTF_IMPLEMENTATION
        STB_IMAGE_IMPLEMENTATION
        STB_IMAGE_WRITE_IMPLEMENTATION
    )
endif()

# ── Dear ImGui (docking branch) ───────────────────────────────────────────────
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

# Build ImGui as a static library (core + Metal + OSX backends)
if(NOT TARGET imgui::imgui)
    set(IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
    )
    if(APPLE AND PHM_BUILD_METAL)
        list(APPEND IMGUI_SOURCES
            ${imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm
            ${imgui_SOURCE_DIR}/backends/imgui_impl_osx.mm
        )
    endif()

    add_library(imgui STATIC ${IMGUI_SOURCES})
    add_library(imgui::imgui ALIAS imgui)
    target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )
    set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)

    if(APPLE AND PHM_BUILD_METAL)
        target_compile_definitions(imgui PUBLIC IMGUI_IMPL_METAL_CPP)
        target_link_libraries(imgui PUBLIC
            "-framework Metal"
            "-framework MetalKit"
            "-framework AppKit"
            "-framework Foundation"
            "-framework QuartzCore"
        )
    endif()
endif()

# ── Google Test ───────────────────────────────────────────────────────────────
if(PHM_BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
        GIT_SHALLOW    TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK            OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# ── Google Benchmark ──────────────────────────────────────────────────────────
if(PHM_BUILD_BENCHMARKS)
    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG        v1.8.4
        GIT_SHALLOW    TRUE
    )
    set(BENCHMARK_ENABLE_TESTING   OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_INSTALL_DOCS     OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(benchmark)
endif()
