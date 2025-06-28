# Root CMakeLists.txt - Assignment 3 Phase 1 Build System
cmake_minimum_required(VERSION 3.14)
project(TanksGame_Assignment3)

# Set C++20 standard as specified in assignment requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add required compiler flags for shared libraries
add_compile_options(-Wall -Wextra -Werror -pedantic) # TODO: check if we need -fPIC

# Build configuration options
option(ENABLE_TESTING "Build tests" ON)
option(ENABLE_VISUALIZATION "Enable visualization features" OFF)

# Set output directories - build artifacts to build/, libraries will be set per project
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

# Download and configure Google Test for all projects
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Collect all UserCommon source files recursively
file(GLOB_RECURSE USERCOMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/*.cpp")
file(GLOB_RECURSE USERCOMMON_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/*.h")

# Remove analysis sources as they depend on Algorithm components
file(GLOB_RECURSE ANALYSIS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/bonus/analysis/*.cpp")
list(REMOVE_ITEM USERCOMMON_SOURCES ${ANALYSIS_SOURCES})

# Remove test files from UserCommon sources
file(GLOB_RECURSE USERCOMMON_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/*_test.cpp")
list(REMOVE_ITEM USERCOMMON_SOURCES ${USERCOMMON_TEST_SOURCES})

# Remove all bonus test files for now
file(GLOB_RECURSE BONUS_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/bonus/*_test.cpp")
list(REMOVE_ITEM USERCOMMON_TEST_SOURCES ${BONUS_TEST_SOURCES})

# Remove testing directory as it contains gtest dependencies
file(GLOB_RECURSE TESTING_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/utils/testing/*.cpp")
list(REMOVE_ITEM USERCOMMON_SOURCES ${TESTING_SOURCES})

# Remove visualization sources if disabled
if(NOT ENABLE_VISUALIZATION)
    file(GLOB_RECURSE VIZ_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/bonus/visualization/*.cpp")
    list(REMOVE_ITEM USERCOMMON_SOURCES ${VIZ_SOURCES})
else()
    add_definitions(-DENABLE_VISUALIZATION)
endif()

# UserCommon sources are now available as global variables

# Common include directories available to all projects
set(COMMON_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon"
)

# Create UserCommon test executable
if(ENABLE_TESTING)
    # Create usercommon_tests executable
    add_executable(usercommon_tests ${USERCOMMON_TEST_SOURCES})
    
    # Link UserCommon sources and Google Test
    target_sources(usercommon_tests PRIVATE ${USERCOMMON_SOURCES})
    target_link_libraries(usercommon_tests PRIVATE gtest gtest_main)
    target_include_directories(usercommon_tests PRIVATE ${COMMON_INCLUDE_DIRS})
    
    # Add to CTest
    enable_testing()
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