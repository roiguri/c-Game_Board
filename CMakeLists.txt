# Root CMakeLists.txt - Assignment 3 Phase 1 Build System
cmake_minimum_required(VERSION 3.14)
project(TanksGame_Assignment3)

# Set C++20 standard as specified in assignment requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add required compiler flags for shared libraries
add_compile_options(-Wall -Wextra -Werror -pedantic -fPIC)

# Build configuration options
option(ENABLE_TESTING "Build tests" OFF)
option(ENABLE_VISUALIZATION "Enable visualization features" OFF)
option(ENABLE_UI "Build web UI server" OFF)

# Set output directories - build artifacts to build/, libraries will be set per project
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

# Create a common tests directory for all test executables
set(CMAKE_TESTS_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tests")

# Download and configure external dependencies
include(FetchContent)

# Google Test for testing (only when ENABLE_TESTING is ON)
if(ENABLE_TESTING)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# UI dependencies (only when ENABLE_UI is ON)
if(ENABLE_UI)
    # cpp-httplib for web server functionality
    FetchContent_Declare(
        httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG v0.14.3
    )
    
    # reproc for process management
    FetchContent_Declare(
        reproc
        GIT_REPOSITORY https://github.com/DaanDeMeyer/reproc.git
        GIT_TAG v14.2.1
    )

    # nlohmann/json for JSON handling
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
    )
    
    FetchContent_MakeAvailable(httplib reproc nlohmann_json)
endif()

# Create UserCommon static library
file(GLOB_RECURSE USERCOMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/*.cpp")
file(GLOB_RECURSE USERCOMMON_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/*_test.cpp")

# Remove test files from library sources
list(REMOVE_ITEM USERCOMMON_SOURCES ${USERCOMMON_TEST_SOURCES})

# Handle visualization sources conditionally
file(GLOB_RECURSE VIZ_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/bonus/visualization/*.cpp")
file(GLOB_RECURSE VIZ_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/bonus/visualization/*_test.cpp")

# Remove visualization sources when visualization is disabled
if(NOT ENABLE_VISUALIZATION)
    list(REMOVE_ITEM USERCOMMON_SOURCES ${VIZ_SOURCES})
endif()

# Remove visualization tests when either testing or visualization is disabled
if(NOT ENABLE_TESTING OR NOT ENABLE_VISUALIZATION)
    list(REMOVE_ITEM USERCOMMON_TEST_SOURCES ${VIZ_TEST_SOURCES})
endif()

# Remove ui_server sources (they have their own build target)
file(GLOB_RECURSE UI_SERVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/UserCommon/bonus/ui_server/*.cpp")
list(REMOVE_ITEM USERCOMMON_SOURCES ${UI_SERVER_SOURCES})

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

# Add preprocessor definition for visualization when enabled
if(ENABLE_VISUALIZATION)
    target_compile_definitions(UserCommon PUBLIC ENABLE_VISUALIZATION)
endif()

# Create UserCommon test executable
if(ENABLE_TESTING)
    enable_testing()
    
    # Create usercommon_tests executable
    add_executable(usercommon_tests ${USERCOMMON_TEST_SOURCES})
    
    # Set test executable output directory
    set_target_properties(usercommon_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_TESTS_OUTPUT_DIRECTORY}"
    )
    
    # Link UserCommon library and Google Test
    target_link_libraries(usercommon_tests PRIVATE 
        UserCommon 
        gtest 
        gtest_main
    )
    
    # Add to CTest with test discovery
    include(GoogleTest)
    gtest_discover_tests(usercommon_tests)
    add_test(NAME UserCommonTests COMMAND usercommon_tests)
endif()

# Add project subdirectories
add_subdirectory(Algorithm)
add_subdirectory(GameManager)
add_subdirectory(Simulator)

# Add UI server subdirectory (only when ENABLE_UI is ON)
if(ENABLE_UI)
    add_subdirectory(UserCommon/bonus/ui_server)
endif()

# Global convenience targets
if(ENABLE_UI)
    add_custom_target(all_projects
        DEPENDS Algorithm_318835816_211314471 gamemanager simulator tank_simulator_ui
        COMMENT "Building all project libraries and executables (including UI)"
    )
else()
    add_custom_target(all_projects
        DEPENDS Algorithm_318835816_211314471 gamemanager simulator
        COMMENT "Building all project libraries and executables"
    )
endif()

add_custom_target(all_tests
    DEPENDS algorithm_tests gamemanager_tests simulator_tests usercommon_tests
    COMMENT "Building all test executables"
)

add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS all_tests
    COMMENT "Building and running all tests"
)