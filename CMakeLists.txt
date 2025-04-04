cmake_minimum_required(VERSION 3.10)
project(TankGame)

# Create directories if they don't exist
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/test/test_files)

# Add include directory for your project
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add subdirectory for tests
add_subdirectory(test)

# Source files for the library
file(GLOB LIB_SRCS "src/*.cpp")

# Create a library
add_library(tank_game ${LIB_SRCS})

# Optionally, an executable
file(GLOB EXEC_SRCS "app/*.cpp") # Assuming you have an app directory
if (EXEC_SRCS)
    add_executable(tank_game_app ${EXEC_SRCS})
    target_link_libraries(tank_game_app tank_game)
endif()