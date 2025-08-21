# GameManager Test Infrastructure
cmake_minimum_required(VERSION 3.14)

# Test categories
set(UNIT_TEST_SOURCES
    unit/interface_compliance_test.cpp
    unit/satellite_view_integration_test.cpp
    unit/player_algorithm_integration_test.cpp
    unit/game_logic_test.cpp
    unit/edge_cases_test.cpp
)

set(INTEGRATION_TEST_SOURCES
    integration/full_game_scenarios_test.cpp
    integration/performance_test.cpp
    integration/memory_test.cpp
)

set(MOCK_HEADERS
    mocks/enhanced_mock_player.h
    mocks/enhanced_mock_algorithm.h
    mocks/mock_factories.h
    mocks/scenario_mock_satellite_view.h
)

set(HELPER_HEADERS
    helpers/game_result_validator.h
    helpers/game_scenario_builder.h
    helpers/test_utilities.h
    helpers/game_object_utilities.h
)

# Create enhanced GameManager test executable
add_executable(gamemanager_enhanced_tests
    ${UNIT_TEST_SOURCES}
    ${INTEGRATION_TEST_SOURCES}
)

# Include directories for tests
target_include_directories(gamemanager_enhanced_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/GameManager
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/UserCommon
    ${CMAKE_SOURCE_DIR}
)

# Link libraries
target_link_libraries(gamemanager_enhanced_tests PRIVATE
    GameManager_318835816_211314471
    gtest
    gtest_main
    gmock
    gmock_main
    pthread
)

# Compiler options
target_compile_features(gamemanager_enhanced_tests PRIVATE cxx_std_17)
target_compile_options(gamemanager_enhanced_tests PRIVATE
    -Wall -Wextra -Werror -pedantic
)

# Test discovery for CTest
include(GoogleTest)
gtest_discover_tests(gamemanager_enhanced_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DISCOVERY_MODE PRE_TEST
)

# Custom test targets for different categories
add_custom_target(test_interface
    COMMAND ${CMAKE_CTEST_COMMAND} -R "InterfaceComplianceTest.*"
    DEPENDS gamemanager_enhanced_tests
    COMMENT "Running interface compliance tests"
)

add_custom_target(test_unit
    COMMAND ${CMAKE_CTEST_COMMAND} -R ".*Test.*" -E "Integration.*|Performance.*|Memory.*"
    DEPENDS gamemanager_enhanced_tests
    COMMENT "Running unit tests"
)

add_custom_target(test_integration
    COMMAND ${CMAKE_CTEST_COMMAND} -R "Integration.*"
    DEPENDS gamemanager_enhanced_tests
    COMMENT "Running integration tests"
)

add_custom_target(test_performance
    COMMAND ${CMAKE_CTEST_COMMAND} -R "Performance.*"
    DEPENDS gamemanager_enhanced_tests
    COMMENT "Running performance tests"
)

add_custom_target(test_memory
    COMMAND ${CMAKE_CTEST_COMMAND} -R "Memory.*"
    DEPENDS gamemanager_enhanced_tests
    COMMENT "Running memory tests"
)

# Coverage target (requires gcov)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_program(GCOV_PATH gcov)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    
    if(GCOV_PATH AND LCOV_PATH AND GENHTML_PATH)
        target_compile_options(gamemanager_enhanced_tests PRIVATE
            --coverage -fprofile-arcs -ftest-coverage
        )
        target_link_libraries(gamemanager_enhanced_tests PRIVATE --coverage)
        
        add_custom_target(coverage
            COMMAND ${CMAKE_CTEST_COMMAND}
            COMMAND ${LCOV_PATH} --capture --directory . --output-file coverage.info
            COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' --output-file coverage.info
            COMMAND ${LCOV_PATH} --remove coverage.info '*/test/*' --output-file coverage.info
            COMMAND ${GENHTML_PATH} coverage.info --output-directory coverage_html
            DEPENDS gamemanager_enhanced_tests
            COMMENT "Generating test coverage report"
        )
    endif()
endif()

# Memory leak detection with valgrind
find_program(VALGRIND_PATH valgrind)
if(VALGRIND_PATH)
    add_custom_target(test_memcheck
        COMMAND ${VALGRIND_PATH} --tool=memcheck --leak-check=full --show-leak-kinds=all 
                --track-origins=yes --error-exitcode=1
                $<TARGET_FILE:gamemanager_enhanced_tests>
        DEPENDS gamemanager_enhanced_tests
        COMMENT "Running memory leak detection with valgrind"
    )
endif()

# Benchmark target for performance analysis
add_custom_target(benchmark
    COMMAND ${CMAKE_CTEST_COMMAND} -R "Performance.*" --verbose
    DEPENDS gamemanager_enhanced_tests
    COMMENT "Running performance benchmarks"
)

# Test data files (if needed)
file(COPY test_data DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
     FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

# Install test executable for packaging
install(TARGETS gamemanager_enhanced_tests
        RUNTIME DESTINATION bin/tests
        COMPONENT testing)

# Print test configuration summary
message(STATUS "GameManager Enhanced Tests Configuration:")
message(STATUS "  Unit tests: ${UNIT_TEST_SOURCES}")
message(STATUS "  Integration tests: ${INTEGRATION_TEST_SOURCES}")
message(STATUS "  Mock headers: ${MOCK_HEADERS}")
message(STATUS "  Helper headers: ${HELPER_HEADERS}")
message(STATUS "  Coverage support: ${LCOV_PATH}")
message(STATUS "  Memory check support: ${VALGRIND_PATH}") 