# Build Architecture Specification - Assignment 3 Phase 1

## Overview
This document specifies the build architecture for restructuring a monolithic C++ tanks game project into multiple independent projects as required by Assignment 3. The goal is to create a modular system where GameManager, Algorithm, and Simulator can be built as separate shared libraries and executables.

## Project Structure Requirements

### Target Directory Layout
```
project_root/
├── CMakeLists.txt                          # Root build system
├── build/                                  # All intermediate build artifacts
│   ├── GameManager/                        # GameManager build artifacts
│   │   ├── tests/                          # Test executables location
│   │   └── CMakeFiles/                     # CMake internal files
│   ├── Algorithm/                          # Future: Algorithm build artifacts
│   └── Simulator/                          # Future: Simulator build artifacts
├── third_party/                            # External dependencies
│   └── googletest/                         # Downloaded by root CMake
├── Common/                                 # Assignment-provided interfaces (no build)
│   ├── AbstractGameManager.h
│   ├── GameResult.h
│   ├── SatelliteView.h
│   ├── ActionRequest.h
│   ├── Player.h
│   ├── TankAlgorithm.h
│   └── BattleInfo.h
├── UserCommon/                             # Shared user code (no build)
│   ├── game_board.h/.cpp
│   ├── objects/
│   │   ├── tank.h/.cpp
│   │   ├── shell.h/.cpp
│   │   ├── point.h/.cpp
│   │   └── direction.h
│   ├── file_loader.h/.cpp
│   └── bonus/visualization/                # Optional visualization code
├── GameManager/                            # GameManager project
│   ├── CMakeLists.txt                      # GameManager build system
│   ├── GameManager_098765432_123456789.so # Output: shared library
│   ├── game_manager.h/.cpp
│   ├── collision_handler.h/.cpp
│   └── test/                               # GameManager unit tests
│       ├── game_manager_test.cpp
│       └── collision_handler_test.cpp
├── Algorithm/                              # Future: Algorithm project
└── Simulator/                              # Future: Simulator project
```

## Build System Architecture

### Root CMakeLists.txt Responsibilities

**Primary Functions:**
1. **Dependency Management**: Download and configure Google Test for all projects
2. **UserCommon Source Collection**: Gather all UserCommon source files for use by multiple projects
3. **Global Configuration**: Set C++17 standard, compiler flags, and build options
4. **Subdirectory Orchestration**: Add and coordinate all project subdirectories
5. **Convenience Targets**: Provide high-level build targets for entire project

**Specific Requirements:**
- Download Google Test to `third_party/googletest/` using FetchContent
- Collect all `UserCommon/*.cpp` files recursively into `USERCOMMON_SOURCES` variable
- Collect all `UserCommon/*.h` files recursively into `USERCOMMON_HEADERS` variable
- Make both variables available to all subdirectories via `PARENT_SCOPE`
- Handle conditional compilation for visualization features
- Set build artifact output to `build/` directory
- Provide common include directories: `Common/` and `UserCommon/`

**Build Options:**
- `ENABLE_TESTING=ON/OFF` (default: ON) - Controls test building across all projects
- `ENABLE_VISUALIZATION=ON/OFF` (default: OFF) - Controls visualization feature compilation

**Global Targets to Provide:**
- `all_projects` - Build all project libraries/executables
- `all_tests` - Build all test executables
- `run_all_tests` - Build and execute all tests using CTest

### GameManager CMakeLists.txt Responsibilities

**Primary Functions:**
1. **Shared Library Creation**: Build `GameManager_098765432_123456789.so`
2. **UserCommon Integration**: Compile UserCommon sources directly into GameManager library
3. **Test Executable**: Create separate test executable with proper dependencies
4. **Include Path Management**: Manage complex include dependencies
5. **Symbol Visibility**: Configure proper shared library symbol export

**Library Build Requirements:**
- Library name: `GameManager_098765432_123456789`
- Output location: `GameManager/GameManager_098765432_123456789.so` (NOT in build/)
- Include all `GameManager/*.cpp` source files
- Include all UserCommon sources from parent-provided `USERCOMMON_SOURCES`
- Exclude visualization sources if `ENABLE_VISUALIZATION=OFF`
- Set proper shared library properties (visibility, soname, etc.)

**Test Build Requirements:**
- Test executable name: `GameManager_tests`
- Output location: `build/GameManager/GameManager_tests`
- Include all `GameManager/test/*.cpp` source files
- Link against: GameManager library + Google Test libraries
- Register tests with CTest for discovery

**Include Path Strategy:**
- Public includes: `GameManager/`, `Common/`, `UserCommon/`
- Private includes: Google Test headers for tests
- Use target-based include directory management

**Targets to Provide:**
- `gamemanager` - Build only the GameManager shared library
- `gamemanager_tests` - Build only the GameManager test executable

## Dependency Architecture

### Dependency Chain
```
Root CMake
├── Downloads: Google Test → third_party/
├── Collects: UserCommon sources → USERCOMMON_SOURCES
└── Builds: GameManager/
    ├── Library: GameManager.so
    │   ├── Compiles: GameManager/*.cpp
    │   ├── Compiles: UserCommon sources (from parent)
    │   └── Links: Standard C++ libraries only
    └── Tests: GameManager_tests
        ├── Compiles: GameManager/test/*.cpp
        ├── Links: GameManager.so
        └── Links: Google Test libraries
```

### Namespace Strategy
- **UserCommon code**: Must remain in `UserCommon_318835816_211314471` namespace
- **GameManager code**: Must be in `GameManager_098765432_123456789` namespace
- **GameManager usage**: Use `using namespace UserCommon_318835816_211314471;` or explicit using declarations

### Include Path Conventions
```cpp
// In GameManager source files:
#include "common/AbstractGameManager.h"      // Assignment interfaces
#include "UserCommon/game_board.h"           // Shared user code
#include "GameManager/collision_handler.h"   // GameManager-specific

// In test files:
#include "GameManager/game_manager.h"        // Code under test
#include <gtest/gtest.h>                     // Google Test
```

## Build Behavior Specifications

### Compilation Requirements
- **C++ Standard**: C++17 (exactly, as specified in assignment)
- **Compiler Flags**: `-Wall -Wextra -Werror -pedantic -fPIC`
- **Position Independent Code**: Required for shared libraries
- **Symbol Visibility**: Hidden by default, explicit exports only

### Build Output Behavior
- **Intermediate Files**: All object files, CMake cache, etc. go to `build/`
- **Final Libraries**: Shared libraries (`.so`) stay in their project directories
- **Test Executables**: Go to `build/ProjectName/` subdirectories
- **Parallel Builds**: Support `make -j` for parallel compilation

### Error Handling
- **Missing Dependencies**: Clear error messages for missing UserCommon files
- **Include Failures**: Explicit error messages for failed includes
- **Test Failures**: Proper CTest integration with detailed failure reporting

### Clean Build Support
- `make clean` or `cmake --build . --target clean` removes all build artifacts
- Preserves downloaded dependencies (Google Test)
- Preserves final output libraries

## Success Metrics

### Milestone 1: GameManager Compilation Success
**Primary Success Criteria:**
1. ✅ **Library Creation**: `GameManager_098765432_123456789.so` successfully created in `GameManager/` directory
2. ✅ **Size Verification**: Library file size > 0 and contains expected symbols
3. ✅ **Symbol Export**: Library exports required AbstractGameManager interface symbols
4. ✅ **No External Dependencies**: Library only depends on C++17 standard library (verify with `ldd`)

**Build System Success Criteria:**
5. ✅ **Target Functionality**: `make gamemanager` builds only GameManager library
6. ✅ **Incremental Builds**: Subsequent `make gamemanager` skips unchanged files
7. ✅ **Clean Builds**: `make clean && make gamemanager` rebuilds everything correctly
8. ✅ **Parallel Builds**: `make -j4 gamemanager` completes without race conditions

**UserCommon Integration Success:**
9. ✅ **Source Inclusion**: All required UserCommon `.cpp` files compiled into library
10. ✅ **Header Access**: GameManager code can include and use UserCommon headers
11. ✅ **Namespace Preservation**: UserCommon classes accessible via their namespace
12. ✅ **Visualization Conditional**: Builds successfully with both ENABLE_VISUALIZATION=ON/OFF

**Testing Success Criteria:**
13. ✅ **Test Compilation**: `make gamemanager_tests` creates test executable in `build/GameManager/`
14. ✅ **Test Dependencies**: Test executable properly links to GameManager library and Google Test
15. ✅ **Test Execution**: `make run_all_tests` executes GameManager tests and reports results
16. ✅ **Test Discovery**: CTest discovers and can run individual test cases

### Verification Commands
```bash
# Build verification
make gamemanager
ls -la GameManager/GameManager_098765432_123456789.so
ldd GameManager/GameManager_098765432_123456789.so

# Test verification  
make gamemanager_tests
ls -la build/GameManager/GameManager_tests
./build/GameManager/GameManager_tests

# Integration verification
make all_projects
make run_all_tests
```

### Performance Expectations
- **Initial Build**: Complete in under 2 minutes on modern hardware
- **Incremental Build**: Complete in under 10 seconds for single file changes
- **Test Execution**: Complete in under 30 seconds
- **Memory Usage**: Peak build memory under 1GB

## Future Extensibility

### Algorithm Project Integration
When implementing Algorithm project:
- Should follow same pattern as GameManager
- Will produce `Algorithm_098765432_123456789.so`
- Will also use UserCommon sources
- Will use same Google Test setup

### Simulator Project Integration
When implementing Simulator project:
- Will be an executable, not shared library
- Will dynamically load GameManager and Algorithm libraries
- Will use same build pattern but different output type

### Cross-Project Dependencies
- Each project must be buildable independently
- Shared dependencies (UserCommon, Google Test) managed at root level
- No circular dependencies between projects
- Clear separation of concerns and interfaces