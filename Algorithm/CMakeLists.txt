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
add_library(Algorithm_318835816_211314471 SHARED 
    ${ALGORITHM_SOURCES}
)

set_target_properties(Algorithm_318835816_211314471 PROPERTIES PREFIX "")

# Link UserCommon library
target_link_libraries(Algorithm_318835816_211314471 PRIVATE UserCommon)

# Include directories
target_include_directories(Algorithm_318835816_211314471 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

if(ENABLE_TESTING)
    # Create Algorithm test executable with all sources
    add_executable(algorithm_tests ${ALGORITHM_TEST_SOURCES} ${ALGORITHM_SOURCES})
    
    # Disable registration macros for test build to prevent BadRegistrationException
    target_compile_definitions(algorithm_tests PRIVATE 
        DISABLE_STATIC_REGISTRATION=1
    )
    
    # Set test executable output directory
    set_target_properties(algorithm_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_TESTS_OUTPUT_DIRECTORY}"
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
    )
    
    # Add to CTest with test discovery
    include(GoogleTest)
    gtest_discover_tests(algorithm_tests)
    add_test(NAME AlgorithmTests COMMAND algorithm_tests)
endif()