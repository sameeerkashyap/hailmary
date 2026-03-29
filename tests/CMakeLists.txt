include(GoogleTest)

add_executable(phm_tests
    test_relativity.cpp
    test_brachistochrone.cpp
    test_allocators.cpp
    test_orbital.cpp
)

target_link_libraries(phm_tests PRIVATE
    phm_core
    phm_physics
    GTest::gtest_main
)

set_target_properties(phm_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

gtest_discover_tests(phm_tests)
