cmake_minimum_required(VERSION 3.10)
project(TankGameTests)

# Point to Google Test include directory
include_directories(${CMAKE_SOURCE_DIR}/../extern/googletest/googletest/include)
# Point to Google Test lib directory (if built separately)
link_directories(${CMAKE_SOURCE_DIR}/../extern/googletest/googletest/lib)

# Find your project's library
add_library(tank_game SHARED ${CMAKE_SOURCE_DIR}/../src/GameBoard.cpp)

# List test files
file(GLOB TEST_SRCS "test_files/*.cpp")

# Create test executable
add_executable(tank_game_tests ${TEST_SRCS})

# Link Google Test and your library
target_link_libraries(tank_game_tests gtest gtest_main tank_game)