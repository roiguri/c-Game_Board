# GameManager CMakeLists.txt - Assignment 3 Phase 1
cmake_minimum_required(VERSION 3.14)

# GameManager project configuration
project(GameManager)

# Collect GameManager source files
file(GLOB_RECURSE GAMEMANAGER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE GAMEMANAGER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# Remove test files from library sources
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")
list(REMOVE_ITEM GAMEMANAGER_SOURCES ${TEST_SOURCES})

# Create GameManager shared library with specific naming
add_library(GameManager_098765432_123456789 SHARED 
    ${GAMEMANAGER_SOURCES}
    ${USERCOMMON_SOURCES}  # Include UserCommon sources from parent
)

# Set library properties - .so file goes to GameManager/ directory
set_target_properties(GameManager_098765432_123456789 PROPERTIES
    OUTPUT_NAME "GameManager_098765432_123456789"
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/GameManager"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/GameManager"
)

# Only set hidden visibility if not building tests
if(NOT ENABLE_TESTING)
    set_target_properties(GameManager_098765432_123456789 PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endif()

# Include directories for GameManager library
target_include_directories(GameManager_098765432_123456789 PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/common"
    "${CMAKE_SOURCE_DIR}"
)

# Private include directories (for internal compilation)
target_include_directories(GameManager_098765432_123456789 PRIVATE
    "${CMAKE_SOURCE_DIR}/UserCommon"
    "${CMAKE_SOURCE_DIR}/third_party"
)

# Create alias target for convenience (using custom target since ALIAS doesn't work with custom targets)
add_custom_target(gamemanager DEPENDS GameManager_098765432_123456789)

# Testing configuration
if(ENABLE_TESTING)
    enable_testing()
    
    # Create test executable
    add_executable(GameManager_tests ${TEST_SOURCES})
    
    # Test executable properties
    set_target_properties(GameManager_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/GameManager"
    )
    
    # Test include directories
    target_include_directories(GameManager_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/common"
        "${CMAKE_SOURCE_DIR}/UserCommon"
    )
    
    # Link test executable - include UserCommon sources directly for testing
    target_sources(GameManager_tests PRIVATE 
        ${USERCOMMON_SOURCES}
    )
    target_link_libraries(GameManager_tests PRIVATE
        GameManager_098765432_123456789
        gtest
        gmock
        gtest_main
    )
    
    # Register tests with CTest
    include(GoogleTest)
    gtest_discover_tests(GameManager_tests)
    
    # Create alias for test target
    add_custom_target(gamemanager_tests DEPENDS GameManager_tests)
endif()