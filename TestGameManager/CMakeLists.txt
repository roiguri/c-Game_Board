# TestGameManager CMakeLists.txt - Assignment 3 Phase 1
cmake_minimum_required(VERSION 3.14)

# TestGameManager project configuration
project(TestGameManager)

# Collect TestGameManager source files
file(GLOB_RECURSE TESTGAMEMANAGER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE TESTGAMEMANAGER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# Create TestGameManager shared library with specific naming
add_library(TestGameManager_098765432_123456789 SHARED 
    ${TESTGAMEMANAGER_SOURCES}
)

# Set library properties - .so file goes to TestGameManager/ directory
set_target_properties(TestGameManager_098765432_123456789 PROPERTIES
    OUTPUT_NAME "TestGameManager_098765432_123456789"
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/TestGameManager"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/TestGameManager"
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Link UserCommon library
target_link_libraries(TestGameManager_098765432_123456789 PRIVATE UserCommon)

# Include directories for TestGameManager library  
target_include_directories(TestGameManager_098765432_123456789 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Create alias target for convenience (using custom target since ALIAS doesn't work with custom targets)
add_custom_target(testgamemanager DEPENDS TestGameManager_098765432_123456789)