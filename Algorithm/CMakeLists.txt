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
add_library(Algorithm_098765432_123456789 SHARED ${ALGORITHM_SOURCES})

set_target_properties(Algorithm_098765432_123456789 PROPERTIES PREFIX "")

# Include directories
target_include_directories(Algorithm_098765432_123456789 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${CMAKE_CURRENT_SOURCE_DIR}/../common"
    "${CMAKE_CURRENT_SOURCE_DIR}/../UserCommon"
)

# Compiler flags for shared library
target_compile_options(Algorithm_098765432_123456789 PRIVATE -fPIC)

# Link UserCommon sources
target_sources(Algorithm_098765432_123456789 PRIVATE ${USERCOMMON_SOURCES})

if(ENABLE_TESTING)
    # Create Algorithm test executable
    add_executable(algorithm_tests ${ALGORITHM_TEST_SOURCES})
    
    # Link Algorithm library and Google Test
    target_link_libraries(algorithm_tests PRIVATE Algorithm_098765432_123456789 gtest gtest_main gmock gmock_main)
    target_include_directories(algorithm_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        "${CMAKE_CURRENT_SOURCE_DIR}/../common"
        "${CMAKE_CURRENT_SOURCE_DIR}/../UserCommon"
    )
    
    # Add to CTest
    add_test(NAME AlgorithmTests COMMAND algorithm_tests)
endif()