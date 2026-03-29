# metal-cpp detection
# Apple's official C++ wrapper for Metal (header-only).
# Download from https://developer.apple.com/metal/cpp/ and extract to vendor/metal-cpp/
# so that vendor/metal-cpp/Metal/Metal.hpp exists.

set(METAL_CPP_SEARCH_PATH "${CMAKE_SOURCE_DIR}/vendor/metal-cpp")

find_path(METAL_CPP_INCLUDE_DIR
    NAMES "Metal/Metal.hpp"
    PATHS "${METAL_CPP_SEARCH_PATH}"
    NO_DEFAULT_PATH
)

if(NOT METAL_CPP_INCLUDE_DIR)
    message(WARNING
        "[PHM] metal-cpp not found in vendor/metal-cpp/.\n"
        "      Run:  scripts/fetch_metal_cpp.sh\n"
        "      or download manually from https://developer.apple.com/metal/cpp/\n"
        "      Metal and render modules will be DISABLED."
    )
    set(PHM_BUILD_METAL OFF CACHE BOOL "" FORCE)
else()
    message(STATUS "[PHM] metal-cpp found at: ${METAL_CPP_INCLUDE_DIR}")

    add_library(metal_cpp INTERFACE)
    add_library(metal_cpp::metal_cpp ALIAS metal_cpp)

    target_include_directories(metal_cpp INTERFACE "${METAL_CPP_INCLUDE_DIR}")
    target_compile_definitions(metal_cpp INTERFACE
        NS_PRIVATE_IMPLEMENTATION
        CA_PRIVATE_IMPLEMENTATION
        MTL_PRIVATE_IMPLEMENTATION
    )
    target_link_libraries(metal_cpp INTERFACE
        "-framework Metal"
        "-framework Foundation"
        "-framework QuartzCore"
        "-framework AppKit"
        "-framework MetalKit"
    )
endif()

# Helper to compile .metal shaders into a .metallib at build time
# Usage: phm_add_metallib(TARGET_NAME output.metallib shader1.metal shader2.metal ...)
function(phm_add_metallib target_name output_name)
    set(metal_sources ${ARGN})
    set(air_files "")

    foreach(src IN LISTS metal_sources)
        get_filename_component(stem "${src}" NAME_WE)
        set(air_out "${CMAKE_CURRENT_BINARY_DIR}/${stem}.air")

        add_custom_command(
            OUTPUT  "${air_out}"
            COMMAND xcrun -sdk macosx metal -c "${src}" -o "${air_out}"
                    -std=metal3.0
                    $<$<CONFIG:Debug>:-gline-tables-only -MO>
                    $<$<CONFIG:Release>:-O3>
            DEPENDS "${src}"
            COMMENT "Compiling Metal shader: ${src}"
        )
        list(APPEND air_files "${air_out}")
    endforeach()

    set(metallib_out "${CMAKE_CURRENT_BINARY_DIR}/${output_name}")
    add_custom_command(
        OUTPUT  "${metallib_out}"
        COMMAND xcrun -sdk macosx metallib ${air_files} -o "${metallib_out}"
        DEPENDS ${air_files}
        COMMENT "Linking metallib: ${output_name}"
    )

    add_custom_target("${target_name}" ALL DEPENDS "${metallib_out}")
    set_target_properties("${target_name}" PROPERTIES
        METALLIB_PATH "${metallib_out}"
    )
endfunction()
