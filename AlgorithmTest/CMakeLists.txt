# AlgorithmTest CMakeLists.txt - Independent Test Algorithm Library
cmake_minimum_required(VERSION 3.14)
project(AlgorithmTest)

# Set output directory for shared library
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# Define test algorithm sources
set(TEST_ALGORITHM_SOURCES
    test_basic_player.cpp
    test_basic_tank_algorithm.cpp
    test_battle_info_impl.cpp
)

# Create shared library with test algorithm naming convention
add_library(TestAlgorithm_098765432_123456789 SHARED 
    ${TEST_ALGORITHM_SOURCES}
)

set_target_properties(TestAlgorithm_098765432_123456789 PROPERTIES PREFIX "")

# Link UserCommon library directly
target_link_libraries(TestAlgorithm_098765432_123456789 PRIVATE 
    "/home/roiguri/projects/uni/advanced_programming/c-Game_Board/build/libUserCommon.a"
)

# Include directories
target_include_directories(TestAlgorithm_098765432_123456789 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/common"
    "/home/roiguri/projects/uni/advanced_programming/c-Game_Board"
    "/home/roiguri/projects/uni/advanced_programming/c-Game_Board/common"
    "/home/roiguri/projects/uni/advanced_programming/c-Game_Board/UserCommon"
)