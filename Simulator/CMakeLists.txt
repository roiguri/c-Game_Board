# Simulator CMakeLists.txt - Assignment 3 Phase 1
cmake_minimum_required(VERSION 3.14)

# Simulator project configuration
project(Simulator)

# Collect Simulator source files
file(GLOB_RECURSE SIMULATOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE SIMULATOR_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# Remove test files from library sources
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")
list(REMOVE_ITEM SIMULATOR_SOURCES ${TEST_SOURCES})

# Create Simulator static library
add_library(Simulator_lib STATIC 
    ${SIMULATOR_SOURCES}
    ${USERCOMMON_SOURCES}  # Include UserCommon sources from parent
)

# Set library properties
set_target_properties(Simulator_lib PROPERTIES
    OUTPUT_NAME "Simulator_lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Simulator"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Simulator"
)

# Include directories for Simulator library
target_include_directories(Simulator_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/common"
    "${CMAKE_SOURCE_DIR}"
)

# Private include directories (for internal compilation)
target_include_directories(Simulator_lib PRIVATE
    "${CMAKE_SOURCE_DIR}/UserCommon"
    "${CMAKE_SOURCE_DIR}/third_party"
)

# Create alias target for convenience (using custom target since ALIAS doesn't work with custom targets)
add_custom_target(simulator DEPENDS Simulator_lib)

# Testing configuration
if(ENABLE_TESTING)
    enable_testing()
    
    # Create test executable
    add_executable(Simulator_tests ${TEST_SOURCES})
    
    # Test executable properties
    set_target_properties(Simulator_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Simulator"
    )
    
    # Test include directories
    target_include_directories(Simulator_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/common"
        "${CMAKE_SOURCE_DIR}/UserCommon"
    )
    
    # Link test executable - include UserCommon sources directly for testing
    target_sources(Simulator_tests PRIVATE 
        ${USERCOMMON_SOURCES}
    )
    target_link_libraries(Simulator_tests PRIVATE
        Simulator_lib
        gtest
        gmock
        gtest_main
        dl  # For dynamic loading functionality
    )
    
    # Register tests with CTest
    include(GoogleTest)
    gtest_discover_tests(Simulator_tests)
    
    # Create alias for test target
    add_custom_target(simulator_tests DEPENDS Simulator_tests)
endif()