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

# Add the main executable
# add_executable(tanks_game src/main.cpp)
# target_link_libraries(tanks_game PRIVATE tank_battle_lib)

# Configure tests
file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")
add_executable(tank_battle_tests ${TEST_SOURCES})
target_include_directories(tank_battle_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(tank_battle_tests PRIVATE tank_battle_lib gtest gmock gtest_main)

# Discover tests
include(GoogleTest)
gtest_discover_tests(tank_battle_tests)