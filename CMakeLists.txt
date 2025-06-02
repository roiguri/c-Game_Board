# Root CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(TanksGame)

# Set C++ standard to C++20 and add compiler flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Werror -pedantic)

# Build configuration options
option(ENABLE_TESTING "Build tests" OFF)
option(ENABLE_VISUALIZATION "Enable visualization features" OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Common include directories
set(COMMON_INCLUDES 
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)

# Collect source files
file(GLOB_RECURSE ALL_SOURCES "src/*.cpp")
file(GLOB_RECURSE ALL_HEADERS "include/*.h")
file(GLOB COMMON_HEADERS "common/*.h")

# Remove visualization sources if disabled
if(NOT ENABLE_VISUALIZATION)
    file(GLOB_RECURSE VIZ_SOURCES "src/bonus/visualization/*.cpp")
    list(REMOVE_ITEM ALL_SOURCES ${VIZ_SOURCES})
else()
    add_definitions(-DENABLE_VISUALIZATION)
endif()

# Explicitly remove main game and analysis tool sources from library sources
list(REMOVE_ITEM CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
list(REMOVE_ITEM CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/bonus/analysis/analysis_tool.cpp")

# Create the core library
add_library(tanks_game_lib ${ALL_SOURCES} ${ALL_HEADERS} ${COMMON_HEADERS})
target_include_directories(tanks_game_lib PUBLIC 
    ${COMMON_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

# Main executable
add_executable(tanks_game src/main.cpp)
target_link_libraries(tanks_game PRIVATE tanks_game_lib)
target_include_directories(tanks_game PRIVATE ${COMMON_INCLUDES})

# Add the analysis tool executable
add_executable(analysis_tool "${CMAKE_CURRENT_SOURCE_DIR}/src/bonus/analysis/analysis_tool.cpp")
target_link_libraries(analysis_tool PRIVATE tanks_game_lib)
target_include_directories(analysis_tool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/common)
set_target_properties(analysis_tool
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Testing configuration
if(ENABLE_TESTING)
    # Setup Google Test
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/CMakeLists.txt")
        add_subdirectory(third_party/googletest)
    else()
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
    
    # Collect test sources
    file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")

    # Remove visualization tests if visualization is disabled
    if(NOT ENABLE_VISUALIZATION)
        file(GLOB_RECURSE VIZ_TEST_SOURCES "test/bonus/visualization/*.cpp")
        list(REMOVE_ITEM TEST_SOURCES ${VIZ_TEST_SOURCES})
    endif()
    
    add_executable(tanks_game_tests ${TEST_SOURCES})
    target_compile_definitions(tanks_game_tests PRIVATE TEST_BUILD) 
    target_include_directories(tanks_game_tests PRIVATE ${COMMON_INCLUDES})
    target_link_libraries(tanks_game_tests PRIVATE tanks_game_lib gtest gmock gtest_main)
    set_target_properties(tanks_game_tests PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
    )
    
    include(GoogleTest)
    gtest_discover_tests(tanks_game_tests)
endif()

# Copy visualization templates if enabled
if(ENABLE_VISUALIZATION)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/bonus/visualization/visualizers/html_visualizer/templates/
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/templates)
endif()