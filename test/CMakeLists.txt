# Create the test executable
add_executable(run_tests 
    test_main.cpp
    CalculatorTest.cpp
    ${CMAKE_SOURCE_DIR}/src/Calculator.cpp
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