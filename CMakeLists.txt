# Root CMakeLists.txt - Assignment 3 Phase 1 Build System
cmake_minimum_required(VERSION 3.14)
project(TanksGame_Assignment3)

# Set C++20 standard as specified in assignment requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add required compiler flags for shared libraries
add_compile_options(-Wall -Wextra -Werror -pedantic -fPIC) # TODO: check if we need -fPIC

# Build configuration options
option(ENABLE_TESTING "Build tests" ON)
option(ENABLE_VISUALIZATION "Enable visualization features" OFF)

# Set output directories - build artifacts to build/, libraries will be set per project
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

# TODO: make this conditional - only download if not already downloaded
# Download and configure Google Test for all projects
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Create UserCommon static library
file(GLOB_RECURSE USERCOMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/*.cpp")
file(GLOB_RECURSE USERCOMMON_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/*_test.cpp")

# Remove test files from library sources
list(REMOVE_ITEM USERCOMMON_SOURCES ${USERCOMMON_TEST_SOURCES})

# Remove analysis sources as they depend on Algorithm components
file(GLOB_RECURSE ANALYSIS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/bonus/analysis/*.cpp")
list(REMOVE_ITEM USERCOMMON_SOURCES ${ANALYSIS_SOURCES})

# Remove analysis test files that have namespace issues
file(GLOB_RECURSE ANALYSIS_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/bonus/analysis/*_test.cpp")
list(REMOVE_ITEM USERCOMMON_TEST_SOURCES ${ANALYSIS_TEST_SOURCES})

# Remove visualization sources and tests (they have namespace issues)
file(GLOB_RECURSE VIZ_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/bonus/visualization/*.cpp")
list(REMOVE_ITEM USERCOMMON_SOURCES ${VIZ_SOURCES})
file(GLOB_RECURSE VIZ_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/bonus/visualization/*_test.cpp")
list(REMOVE_ITEM USERCOMMON_TEST_SOURCES ${VIZ_TEST_SOURCES})

# Create UserCommon static library
add_library(UserCommon STATIC ${USERCOMMON_SOURCES})

# Set UserCommon properties
set_target_properties(UserCommon PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
)

# UserCommon include directories
target_include_directories(UserCommon PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(UserCommon PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party"
)

# Create UserCommon test executable
if(ENABLE_TESTING)
    enable_testing()
    
    # Create usercommon_tests executable
    add_executable(usercommon_tests ${USERCOMMON_TEST_SOURCES})
    
    # Link UserCommon library and Google Test
    target_link_libraries(usercommon_tests PRIVATE 
        UserCommon 
        gtest 
        gtest_main
    )
    
    # Add to CTest
    add_test(NAME UserCommonTests COMMAND usercommon_tests)
endif()

# Add project subdirectories
add_subdirectory(Algorithm)
add_subdirectory(GameManager)
add_subdirectory(Simulator)

# Global convenience targets
add_custom_target(all_projects
    DEPENDS Algorithm_098765432_123456789 gamemanager simulator
    COMMENT "Building all project libraries and executables"
)

add_custom_target(all_tests
    DEPENDS algorithm_tests gamemanager_tests simulator_tests usercommon_tests
    COMMENT "Building all test executables"
)

add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS all_tests
    COMMENT "Building and running all tests"
)