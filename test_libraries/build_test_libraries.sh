#!/bin/bash

# Build script for test libraries
# This script compiles the test algorithms and GameManagers as real .so files

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Building Test Libraries for Game Mode Testing${NC}"
echo "=============================================="

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}Error: Please run this script from the test_libraries directory${NC}"
    exit 1
fi

# Check if UserCommon library exists
USERCOMMON_LIB="../build/libUserCommon.a"
if [ ! -f "$USERCOMMON_LIB" ]; then
    echo -e "${YELLOW}Warning: UserCommon library not found at $USERCOMMON_LIB${NC}"
    echo "Please make sure the main project is built first:"
    echo "  cd .. && mkdir -p build && cd build && cmake .. && make UserCommon"
    echo ""
fi

# Create build directory
BUILD_DIR="build"
if [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Cleaning existing build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo -e "${YELLOW}Configuring CMake...${NC}"
cmake ..

echo -e "${YELLOW}Building all test libraries...${NC}"
make all_test_libraries

echo ""
echo -e "${GREEN}Build completed successfully!${NC}"
echo ""
echo "Test libraries created:"
echo "----------------------"

# List created libraries
if [ -d "algorithms" ]; then
    echo -e "${GREEN}Algorithms:${NC}"
    ls -la algorithms/*.so 2>/dev/null || echo "  No algorithm libraries found"
fi

if [ -d "game_managers" ]; then
    echo -e "${GREEN}GameManagers:${NC}"
    ls -la game_managers/*.so 2>/dev/null || echo "  No GameManager libraries found"
fi

echo ""
echo -e "${GREEN}Test libraries are ready for use in game mode tests!${NC}"
echo ""
echo "To use these libraries in tests:"
echo "  - Algorithm libraries: $(pwd)/algorithms/"
echo "  - GameManager libraries: $(pwd)/game_managers/"