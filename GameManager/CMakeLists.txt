# GameManager CMakeLists.txt - Assignment 3 Phase 1
cmake_minimum_required(VERSION 3.14)

# GameManager project configuration
project(GameManager)

# Collect GameManager source files
file(GLOB_RECURSE GAMEMANAGER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE GAMEMANAGER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")
file(GLOB_RECURSE TEST_DIR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
list(REMOVE_ITEM GAMEMANAGER_SOURCES ${TEST_SOURCES})
list(REMOVE_ITEM GAMEMANAGER_SOURCES ${TEST_DIR_SOURCES})

# Create GameManager shared library with specific naming
add_library(GameManager_318835816_211314471 SHARED 
    ${GAMEMANAGER_SOURCES}
)

# Set library properties - .so file goes to GameManager/ directory
set_target_properties(GameManager_318835816_211314471 PROPERTIES
    OUTPUT_NAME "GameManager_318835816_211314471"
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/GameManager"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/GameManager"
)

# Only set hidden visibility if not building tests
if(NOT ENABLE_TESTING)
    set_target_properties(GameManager_318835816_211314471 PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endif()

# Link UserCommon library
target_link_libraries(GameManager_318835816_211314471 PRIVATE UserCommon)

# Include directories for GameManager library  
target_include_directories(GameManager_318835816_211314471 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Pass ENABLE_VISUALIZATION definition if visualization is enabled
if(ENABLE_VISUALIZATION)
    target_compile_definitions(GameManager_318835816_211314471 PRIVATE ENABLE_VISUALIZATION)
endif()

# Create alias target for convenience (using custom target since ALIAS doesn't work with custom targets)
add_custom_target(gamemanager DEPENDS GameManager_318835816_211314471)

# Testing configuration
if(ENABLE_TESTING)
    enable_testing()
    
    # Create test executable
    add_executable(GameManager_tests ${TEST_SOURCES})
    
    # Disable registration macros for test build to prevent segmentation fault
    target_compile_definitions(GameManager_tests PRIVATE 
        DISABLE_STATIC_REGISTRATION=1
    )
    
    # Pass ENABLE_VISUALIZATION definition if visualization is enabled
    if(ENABLE_VISUALIZATION)
        target_compile_definitions(GameManager_tests PRIVATE ENABLE_VISUALIZATION)
    endif()
    
    # Test executable properties - use common tests directory
    set_target_properties(GameManager_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_TESTS_OUTPUT_DIRECTORY}"
    )
    
    # Test include directories
    target_include_directories(GameManager_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    
    # Collect test helper files that were excluded from main library
    file(GLOB_RECURSE TEST_HELPER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
    
    # Link test executable with GameManager sources and test helpers
    target_sources(GameManager_tests PRIVATE 
        ${GAMEMANAGER_SOURCES}
        ${TEST_HELPER_SOURCES}
    )
    
    target_link_libraries(GameManager_tests PRIVATE
        UserCommon
        gtest
        gmock
        gtest_main
    )
    
    # Register tests with CTest  
    include(GoogleTest)
    gtest_discover_tests(GameManager_tests)
    add_test(NAME GameManager_tests COMMAND GameManager_tests)
    
    # Create alias for test target
    add_custom_target(gamemanager_tests DEPENDS GameManager_tests)
endif()