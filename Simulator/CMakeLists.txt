# Simulator CMakeLists.txt - Assignment 3 Phase 1
cmake_minimum_required(VERSION 3.14)

# Simulator project configuration
project(Simulator)

# Collect Simulator source files
file(GLOB_RECURSE SIMULATOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE SIMULATOR_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# Remove test files, main.cpp, and all test directory files from library sources
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")
file(GLOB_RECURSE TEST_DIR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
list(REMOVE_ITEM SIMULATOR_SOURCES ${TEST_SOURCES})
list(REMOVE_ITEM SIMULATOR_SOURCES ${TEST_DIR_SOURCES})
list(REMOVE_ITEM SIMULATOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

# Create Simulator static library
add_library(Simulator_lib STATIC ${SIMULATOR_SOURCES})

# Set library properties
set_target_properties(Simulator_lib PROPERTIES
    OUTPUT_NAME "Simulator_lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Simulator"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Simulator"
)

# Link UserCommon library, dl for dynamic loading, and pthread for threading
target_link_libraries(Simulator_lib PRIVATE UserCommon dl pthread)

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
add_executable(simulator_318835816_211314471 main.cpp ${REGISTRATION_SOURCES})

# Set executable properties
set_target_properties(simulator_318835816_211314471 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Simulator"
)

# Link main executable with Simulator library and enable dynamic symbol export
target_link_libraries(simulator_318835816_211314471 PRIVATE Simulator_lib pthread)
target_link_options(simulator_318835816_211314471 PRIVATE -rdynamic)

# Create alias target for convenience (using custom target since ALIAS doesn't work with custom targets)
add_custom_target(simulator DEPENDS Simulator_lib simulator_318835816_211314471)

# Testing configuration
if(ENABLE_TESTING)
    enable_testing()
    
    # Collect test helper files that were excluded from main library
    file(GLOB_RECURSE TEST_HELPER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
    
    # Create test executable with test sources and helpers
    add_executable(Simulator_tests ${TEST_SOURCES} ${TEST_HELPER_SOURCES})
    
    # Test executable properties - use common tests directory
    set_target_properties(Simulator_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_TESTS_OUTPUT_DIRECTORY}"
    )
    
    # Link test executable with libraries
    target_link_libraries(Simulator_tests PRIVATE
        Simulator_lib
        gtest
        gmock
        gtest_main
        pthread
    )
    
    # Register tests with CTest
    include(GoogleTest)
    gtest_discover_tests(Simulator_tests)
    add_test(NAME Simulator_tests COMMAND Simulator_tests)
    
    # Create alias for test target
    add_custom_target(simulator_tests DEPENDS Simulator_tests)
endif()