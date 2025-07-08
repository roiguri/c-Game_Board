#!/bin/bash

# Threading Performance Test Script
# =================================
# This script tests threading implementation and measures performance
# across different thread counts for both Competition and Comparative modes.

set -e  # Exit on any error

# Configuration
BUILD_DIR="/home/roiguri/projects/uni/advanced_programming/c-Game_Board/build"
GAME_MAPS_FOLDER="../test_libraries/compiled_libraries/game_maps"
GAME_MANAGER="../test_libraries/compiled_libraries/game_managers/libTestGameManager_Real_098765432_123456789.so"
ALGORITHMS_FOLDER="../test_libraries/compiled_libraries/algorithms"
ALGORITHM1="../test_libraries/compiled_libraries/algorithms/libTestAlgorithm_Basic_098765432_123456789.so"
ALGORITHM2="../test_libraries/compiled_libraries/algorithms/libTestAlgorithm_Offensive_098765432_123456789.so"
GAME_MAP="../examples/input_a.txt"
GM_FOLDER="../test_libraries/compiled_libraries/game_managers"

# Ensure we're in the build directory
cd "$BUILD_DIR"

echo "========================================================================"
echo "THREADING PERFORMANCE TESTS"
echo "========================================================================"
echo "Starting threading performance analysis..."
echo "Build directory: $(pwd)"
echo "Timestamp: $(date)"
echo ""

# ============================================================================
# COMPETITION MODE THREADING TESTS
# ============================================================================

echo "========================================================================"
echo "COMPETITION MODE THREADING TESTS"
echo "========================================================================"

echo "=== Test 1: Competition Mode - Single Thread (Default) ==="
echo "Starting single-threaded execution..."
time ./bin/tanks_game \
    game_maps_folder="$GAME_MAPS_FOLDER" \
    game_manager="$GAME_MANAGER" \
    algorithms_folder="$ALGORITHMS_FOLDER" \
    -competition -verbose
echo "Single-threaded execution completed."
echo ""

echo "=== Test 2: Competition Mode - Two Threads ==="
echo "Starting two-threaded execution..."
time ./bin/tanks_game \
    game_maps_folder="$GAME_MAPS_FOLDER" \
    game_manager="$GAME_MANAGER" \
    algorithms_folder="$ALGORITHMS_FOLDER" \
    num_threads=2 \
    -competition -verbose
echo "Two-threaded execution completed."
echo ""

echo "=== Test 3: Competition Mode - Four Threads ==="
echo "Starting four-threaded execution..."
time ./bin/tanks_game \
    game_maps_folder="$GAME_MAPS_FOLDER" \
    game_manager="$GAME_MANAGER" \
    algorithms_folder="$ALGORITHMS_FOLDER" \
    num_threads=4 \
    -competition -verbose
echo "Four-threaded execution completed."
echo ""

echo "=== Test 4: Competition Mode - Eight Threads ==="
echo "Starting eight-threaded execution..."
time ./bin/tanks_game \
    game_maps_folder="$GAME_MAPS_FOLDER" \
    game_manager="$GAME_MANAGER" \
    algorithms_folder="$ALGORITHMS_FOLDER" \
    num_threads=8 \
    -competition -verbose
echo "Eight-threaded execution completed."
echo ""

# ============================================================================
# COMPARATIVE MODE THREADING TESTS
# ============================================================================

echo "========================================================================"
echo "COMPARATIVE MODE THREADING TESTS"
echo "========================================================================"

echo "=== Test 5: Comparative Mode - Single Thread (Default) ==="
echo "Starting single-threaded comparative execution..."
time ./bin/tanks_game \
    game_map="$GAME_MAP" \
    game_managers_folder="$GM_FOLDER" \
    algorithm1="$ALGORITHM1" \
    algorithm2="$ALGORITHM2" \
    -comparative -verbose
echo "Single-threaded comparative execution completed."
echo ""

echo "=== Test 6: Comparative Mode - Four Threads ==="
echo "Starting four-threaded comparative execution..."
time ./bin/tanks_game \
    game_map="$GAME_MAP" \
    game_managers_folder="$GM_FOLDER" \
    algorithm1="$ALGORITHM1" \
    algorithm2="$ALGORITHM2" \
    num_threads=4 \
    -comparative -verbose
echo "Four-threaded comparative execution completed."
echo ""

# ============================================================================
# THREADING BEHAVIOR VALIDATION TESTS
# ============================================================================

echo "========================================================================"
echo "THREADING BEHAVIOR VALIDATION TESTS"
echo "========================================================================"

echo "=== Test 7: Validate Single Thread Behavior (num_threads=1) ==="
echo "Testing explicit single thread (num_threads=1)..."
time ./bin/tanks_game \
    game_maps_folder="$GAME_MAPS_FOLDER" \
    game_manager="$GAME_MANAGER" \
    algorithms_folder="$ALGORITHMS_FOLDER" \
    num_threads=1 \
    -competition -verbose
echo "Single thread validation completed."
echo ""

echo "=== Test 8: Validate Missing Thread Argument ==="
echo "Testing missing num_threads argument (should default to single thread)..."
time ./bin/tanks_game \
    game_maps_folder="$GAME_MAPS_FOLDER" \
    game_manager="$GAME_MANAGER" \
    algorithms_folder="$ALGORITHMS_FOLDER" \
    -competition -verbose
echo "Missing thread argument validation completed."
echo ""

# ============================================================================
# PERFORMANCE SUMMARY
# ============================================================================

echo "========================================================================"
echo "THREADING PERFORMANCE TESTS COMPLETED"
echo "========================================================================"
echo "All threading tests completed successfully!"
echo "Timestamp: $(date)"
echo ""
echo "Performance Analysis:"
echo "- Review the 'real' time values from each test"
echo "- Multi-threaded tests should show performance improvements"
echo "- Validate that threading behavior follows requirements:"
echo "  * Missing num_threads/num_threads=1: Single thread (main only)"
echo "  * num_threads=2: 3 total threads (2 workers + 1 main)"
echo "  * num_threads=4: 5 total threads (4 workers + 1 main)"
echo "  * num_threads=8: 9 total threads (8 workers + 1 main)"
echo ""
echo "Expected Results:"
echo "- Tests 1 & 8 should have identical performance (both single-threaded)"
echo "- Tests 2-4 should show decreasing execution times with more threads"
echo "- Test 7 should match Tests 1 & 8 (all single-threaded)"
echo "========================================================================"