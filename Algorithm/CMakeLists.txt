# Algorithm CMakeLists.txt - Assignment 3 Algorithm Library
cmake_minimum_required(VERSION 3.14)

# Set output directory for shared library
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# Collect Algorithm source files from current directory
file(GLOB ALGORITHM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Remove test files from Algorithm sources
file(GLOB ALGORITHM_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")
list(REMOVE_ITEM ALGORITHM_SOURCES ${ALGORITHM_TEST_SOURCES})

# Create shared library with Assignment 3 naming convention
add_library(Algorithm_098765432_123456789 SHARED 
    ${ALGORITHM_SOURCES}
)

set_target_properties(Algorithm_098765432_123456789 PROPERTIES PREFIX "")

# Link UserCommon library
target_link_libraries(Algorithm_098765432_123456789 PRIVATE UserCommon)

# Include directories
target_include_directories(Algorithm_098765432_123456789 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

if(ENABLE_TESTING)
    # Create Algorithm test executable with all sources
    add_executable(algorithm_tests ${ALGORITHM_TEST_SOURCES} ${ALGORITHM_SOURCES})
    
    # Add Simulator registration sources for tests
    target_sources(algorithm_tests PRIVATE
        ../Simulator/registration/PlayerRegistration.cpp
        ../Simulator/registration/TankAlgorithmRegistration.cpp
        ../Simulator/registration/AlgorithmRegistrar.cpp
    )
    
    # Link libraries and Google Test
    target_link_libraries(algorithm_tests PRIVATE 
        UserCommon
        gtest 
        gtest_main 
        gmock 
        gmock_main
    )
    
    target_include_directories(algorithm_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Simulator"
    )
    
    # Add to CTest
    add_test(NAME AlgorithmTests COMMAND algorithm_tests)
endif()