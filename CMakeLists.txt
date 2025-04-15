# Root CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(TankBattle)

set(CMAKE_CXX_STANDARD 17)

# Add Google Test
add_subdirectory(third_party/googletest)
enable_testing()

# Collect source files
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS "include/*.h")

# Add the library
add_library(tank_battle_lib ${PROJECT_SOURCES} ${PROJECT_HEADERS})
target_include_directories(tank_battle_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set explicit output directories
set_target_properties(tank_battle_lib
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Add the main executable
add_executable(tank_battle ${PROJECT_SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
target_include_directories(tank_battle PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(tank_battle
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Configure tests executables
set(TEST_SOURCES
    test/utils/point_test.cpp
    test/utils/direction_test.cpp
    test/utils/action_test.cpp
    test/game_board_test.cpp
    test/shell_test.cpp
    test/tank_test.cpp
    test/file_loader_test.cpp
    test/collision_handler_test.cpp
    test/game_manager_test.cpp
    test/algo/algorithm_test.cpp
    test/algo/chase_algorithm_test.cpp
    test/algo/defensive_algorithm_test.cpp
    test/bonus/logger_test.cpp
    test/test_main.cpp
)

add_executable(tank_battle_tests ${TEST_SOURCES})
target_include_directories(tank_battle_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(tank_battle_tests PRIVATE tank_battle_lib gtest gmock gtest_main)
set_target_properties(tank_battle_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(tank_battle_tests)

# Visualization
# Add visualization demo executable 
add_executable(viz_demo examples/visualization/html_visualizer_demo.cpp)
target_include_directories(viz_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(viz_demo PRIVATE tank_battle_lib)

# Copy visualization templates to build directory for the demo
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/bonus/visualization/visualizers/html_visualizer/templates/
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/templates)