# Create the test executable
add_executable(run_tests 
    test_main.cpp
    point.cpp
    ${CMAKE_SOURCE_DIR}/src/point.cpp
)

# Link with Google Test
target_link_libraries(run_tests PRIVATE 
    gtest 
    gtest_main
)

# Add include directories
target_include_directories(run_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(run_tests)