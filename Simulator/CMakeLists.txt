# Simulator CMakeLists.txt - Assignment 3 Phase 1
cmake_minimum_required(VERSION 3.14)

# Simulator project configuration
project(Simulator)

# Collect Simulator source files
file(GLOB_RECURSE SIMULATOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE SIMULATOR_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# Remove test files and main.cpp from library sources
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")
list(REMOVE_ITEM SIMULATOR_SOURCES ${TEST_SOURCES})
list(REMOVE_ITEM SIMULATOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

# Create Simulator static library
add_library(Simulator_lib STATIC ${SIMULATOR_SOURCES})

# Set library properties
set_target_properties(Simulator_lib PROPERTIES
    OUTPUT_NAME "Simulator_lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Simulator"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Simulator"
)

# Link UserCommon library and dl for dynamic loading
target_link_libraries(Simulator_lib PRIVATE UserCommon dl)

# Include directories for Simulator library
target_include_directories(Simulator_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

# Define registration implementation files explicitly
set(REGISTRATION_SOURCES
    registration/PlayerRegistration.cpp
    registration/TankAlgorithmRegistration.cpp
    registration/GameManagerRegistration.cpp
    registration/AlgorithmRegistrar.cpp
    registration/GameManagerRegistrar.cpp
)

# Create main executable with registration files
add_executable(tanks_game main.cpp ${REGISTRATION_SOURCES})

# Set executable properties
set_target_properties(tanks_game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin"
)

# Link main executable with Simulator library and enable dynamic symbol export
target_link_libraries(tanks_game PRIVATE Simulator_lib)
target_link_options(tanks_game PRIVATE -rdynamic)

# Create alias target for convenience (using custom target since ALIAS doesn't work with custom targets)
add_custom_target(simulator DEPENDS Simulator_lib tanks_game)

# Testing configuration
if(ENABLE_TESTING)
    enable_testing()
    
    # Create test executable
    add_executable(Simulator_tests ${TEST_SOURCES})
    
    # Test executable properties
    set_target_properties(Simulator_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Simulator"
    )
    
    # Link test executable with libraries
    target_link_libraries(Simulator_tests PRIVATE
        Simulator_lib
        gtest
        gmock
        gtest_main
    )
    
    # Register tests with CTest
    include(GoogleTest)
    # gtest_discover_tests(Simulator_tests)  # Disabled due to main() conflict
    add_test(NAME Simulator_tests COMMAND Simulator_tests)
    
    # Create alias for test target
    add_custom_target(simulator_tests DEPENDS Simulator_tests)
endif()