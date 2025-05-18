# Root CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(TanksGame)

# Set C++ standard to C++20 and add compiler flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# TODO: restore all warnings
add_compile_options(
  -Wall
  -Wextra
  -Werror
  -pedantic
)

# Build configuration options
option(ENABLE_TESTING "Build tests" OFF)
option(ENABLE_VISUALIZATION "Enable visualization features" OFF)

# Collect source files (exclude test and visualization for now)
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS "include/*.h")
file(GLOB COMMON_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/common/*.h")

# Define core source files by removing visualization sources if needed
set(CORE_SOURCES ${PROJECT_SOURCES})
set(CORE_HEADERS ${PROJECT_HEADERS} ${COMMON_HEADERS})

# Handle visualization-specific code
if(NOT ENABLE_VISUALIZATION)
    # Remove visualization sources if visualization is disabled
    file(GLOB_RECURSE VIZ_SOURCES "src/bonus/visualization/*.cpp")
    file(GLOB_RECURSE VIZ_HEADERS "include/bonus/visualization/*.h")
    
    foreach(VIZ_SOURCE ${VIZ_SOURCES})
        list(REMOVE_ITEM CORE_SOURCES ${VIZ_SOURCE})
    endforeach()
    
    foreach(VIZ_HEADER ${VIZ_HEADERS})
        list(REMOVE_ITEM CORE_HEADERS ${VIZ_HEADER})
    endforeach()
else()
    # Add visualization definition when enabled
    add_definitions(-DENABLE_VISUALIZATION)
endif()

# Create the core library
add_library(tanks_game_lib ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(tanks_game_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/common)

# Set explicit output directories
set_target_properties(tanks_game_lib
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Add the main executable
add_executable(tanks_game "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(tanks_game PRIVATE tanks_game_lib)
target_include_directories(tanks_game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/common)
set_target_properties(tanks_game
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Handle testing
if(ENABLE_TESTING)
    # Check if googletest is available as a submodule
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/CMakeLists.txt")
        # Use the existing submodule
        add_subdirectory(third_party/googletest)
    else()
        # Download and configure Google Test
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()
    
    enable_testing()
    
    set(TEST_SOURCES
        test/bonus/board_generator_test.cpp
        test/utils/point_test.cpp
        test/utils/direction_test.cpp
        test/utils/midpoint_test.cpp
        test/game_board_test.cpp
        test/objects/game_object_test.cpp
        test/objects/shell_test.cpp
        test/objects/tank_test.cpp
        test/file_loader_test.cpp
        test/collision_handler_test.cpp
        test/game_manager_test.cpp
        test/satellite_view_impl_test.cpp
        test/players/battle_info_impl_test.cpp
        test/players/basic_player_test.cpp
        test/players/basic_player_factory_test.cpp
        test/players/offensive_battle_info_test.cpp
        test/players/offensive_player_test.cpp
        test/algo/basic_tank_algorithm_test.cpp
        test/algo/basic_tank_algorithm_factory_test.cpp
        test/algo/offensive_tank_algorithm_test.cpp
        test/test_main.cpp
    )

    if(ENABLE_VISUALIZATION)
      file(GLOB_RECURSE VISUALIZATION_TEST_SOURCES "test/bonus/visualization/*.cpp")
      list(APPEND TEST_SOURCES ${VISUALIZATION_TEST_SOURCES})
    endif()
    
    add_executable(tanks_game_tests ${TEST_SOURCES})
    target_include_directories(tanks_game_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/common)
    target_link_libraries(tanks_game_tests PRIVATE tanks_game_lib gtest gmock gtest_main)
    set_target_properties(tanks_game_tests
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
    )
    
    # Discover tests
    include(GoogleTest)
    gtest_discover_tests(tanks_game_tests)
endif()

# Copy visualization templates to build directory if visualization is enabled
if(ENABLE_VISUALIZATION)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/bonus/visualization/visualizers/html_visualizer/templates/
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/templates)
endif()