# Test Libraries CMakeLists.txt - Separate compilation for test data
# This CMake file is used to compile real test libraries for game mode testing
# It does NOT affect the production build system

cmake_minimum_required(VERSION 3.14)
project(TestLibraries)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add parent directory to include path for common headers
include_directories(${CMAKE_SOURCE_DIR}/..)
include_directories(${CMAKE_SOURCE_DIR}/../UserCommon)
include_directories(${CMAKE_SOURCE_DIR}/../common)

# Create output directories for test libraries
set(TEST_ALGORITHMS_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/algorithms)
set(TEST_GAMEMANAGERS_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/game_managers)

file(MAKE_DIRECTORY ${TEST_ALGORITHMS_OUTPUT})
file(MAKE_DIRECTORY ${TEST_GAMEMANAGERS_OUTPUT})

# Algorithm Libraries
# ====================

# 1. DoNothing Algorithm
add_library(TestAlgorithm_DoNothing_098765432_123456789 SHARED
    algorithms/do_nothing_player.cpp
    algorithms/do_nothing_tank_algorithm.cpp
)
target_link_libraries(TestAlgorithm_DoNothing_098765432_123456789 
    ${CMAKE_SOURCE_DIR}/../build/libUserCommon.a
    dl
)
set_target_properties(TestAlgorithm_DoNothing_098765432_123456789 PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${TEST_ALGORITHMS_OUTPUT}
    OUTPUT_NAME "TestAlgorithm_DoNothing_098765432_123456789"
)

# 2. AlwaysMoveForward Algorithm
add_library(TestAlgorithm_AlwaysMoveForward_098765432_123456789 SHARED
    algorithms/always_move_forward_player.cpp
    algorithms/always_move_forward_tank_algorithm.cpp
)
target_link_libraries(TestAlgorithm_AlwaysMoveForward_098765432_123456789 
    ${CMAKE_SOURCE_DIR}/../build/libUserCommon.a
    dl
)
set_target_properties(TestAlgorithm_AlwaysMoveForward_098765432_123456789 PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${TEST_ALGORITHMS_OUTPUT}
    OUTPUT_NAME "TestAlgorithm_AlwaysMoveForward_098765432_123456789"
)

# 3. AlwaysShoot Algorithm
add_library(TestAlgorithm_AlwaysShoot_098765432_123456789 SHARED
    algorithms/always_shoot_player.cpp
    algorithms/always_shoot_tank_algorithm.cpp
)
target_link_libraries(TestAlgorithm_AlwaysShoot_098765432_123456789 
    ${CMAKE_SOURCE_DIR}/../build/libUserCommon.a
    dl
)
set_target_properties(TestAlgorithm_AlwaysShoot_098765432_123456789 PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${TEST_ALGORITHMS_OUTPUT}
    OUTPUT_NAME "TestAlgorithm_AlwaysShoot_098765432_123456789"
)

# GameManager Libraries
# ======================

# 1. Predictable GameManager
add_library(TestGameManager_Predictable_098765432_123456789 SHARED
    game_managers/predictable_game_manager.cpp
)
target_link_libraries(TestGameManager_Predictable_098765432_123456789 
    ${CMAKE_SOURCE_DIR}/../build/libUserCommon.a
    dl
)
set_target_properties(TestGameManager_Predictable_098765432_123456789 PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${TEST_GAMEMANAGERS_OUTPUT}
    OUTPUT_NAME "TestGameManager_Predictable_098765432_123456789"
)

# 2. AlwaysTie GameManager
add_library(TestGameManager_AlwaysTie_098765432_123456789 SHARED
    game_managers/tie_game_manager.cpp
)
target_link_libraries(TestGameManager_AlwaysTie_098765432_123456789 
    ${CMAKE_SOURCE_DIR}/../build/libUserCommon.a
    dl
)
set_target_properties(TestGameManager_AlwaysTie_098765432_123456789 PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${TEST_GAMEMANAGERS_OUTPUT}
    OUTPUT_NAME "TestGameManager_AlwaysTie_098765432_123456789"
)

# Custom targets for convenience
add_custom_target(test_algorithms DEPENDS
    TestAlgorithm_DoNothing_098765432_123456789
    TestAlgorithm_AlwaysMoveForward_098765432_123456789
    TestAlgorithm_AlwaysShoot_098765432_123456789
)

add_custom_target(test_gamemanagers DEPENDS
    TestGameManager_Predictable_098765432_123456789
    TestGameManager_AlwaysTie_098765432_123456789
)

add_custom_target(all_test_libraries DEPENDS
    test_algorithms
    test_gamemanagers
)

# Print information about where libraries will be created
message(STATUS "Test algorithms will be built in: ${TEST_ALGORITHMS_OUTPUT}")
message(STATUS "Test GameManagers will be built in: ${TEST_GAMEMANAGERS_OUTPUT}")

# Add install targets for easy deployment
install(TARGETS 
    TestAlgorithm_DoNothing_098765432_123456789
    TestAlgorithm_AlwaysMoveForward_098765432_123456789
    TestAlgorithm_AlwaysShoot_098765432_123456789
    LIBRARY DESTINATION test_data/algorithms
)

install(TARGETS
    TestGameManager_Predictable_098765432_123456789
    TestGameManager_AlwaysTie_098765432_123456789
    LIBRARY DESTINATION test_data/game_managers
)