# Phase 1 Implementation Guide: Complete Project Restructuring

## Overview
This guide restructures the Tank Battle codebase to meet Assignment 3 requirements. You will transform from a single CMake project to 5 separate projects with proper interfaces, directory structure, and build system. **Refer to instructions.md for Assignment 3 specifications when indicated.**

---

## Phase 1A: Interface Alignment (Foundation)

### Step 1A.1: Add Missing Interface Headers

**Objective:** Create the interface headers required by Assignment 3 that are currently missing.

**Context:** Refer to instructions.md section "API and Abstract Base Classes" for exact specifications.

**Actions:**
1. Create `common/GameResult.h`:
```cpp
#pragma once
#include <vector>

struct GameResult {
    int winner; // 0 = tie
    enum Reason { ALL_TANKS_DEAD, MAX_STEPS, ZERO_SHELLS };
    Reason reason;
    std::vector<size_t> remaining_tanks; // index 0 = player 1, etc.
};
```

2. Create `common/AbstractGameManager.h`:
```cpp
#pragma once
#include "GameResult.h"
#include "SatelliteView.h"
#include "Player.h"
#include "TankAlgorithm.h"
#include <memory>
#include <functional>

using GameManagerFactory = std::function<std::unique_ptr<AbstractGameManager>()>;

class AbstractGameManager {
public:
    virtual ~AbstractGameManager() {}
    virtual GameResult run(
        size_t map_width, size_t map_height,
        SatelliteView& map, // <= assume it is a snapshot, NOT updated
        size_t max_steps, size_t num_shells,
        Player& player1, Player& player2,
        TankAlgorithmFactory player1_tank_algo_factory,
        TankAlgorithmFactory player2_tank_algo_factory) = 0;
};
```

**Success Criteria:**
- [x] `common/GameResult.h` exists with exact struct from instructions.md
- [x] `common/AbstractGameManager.h` exists with exact interface from instructions.md
- [x] Both headers compile independently: `g++ -c common/GameResult.h -o /dev/null`

**End State:** Assignment-required interface headers exist and compile.

---

### Step 1A.2: Update Existing Interface Headers

**Objective:** Modify current interface headers to match Assignment 3 specifications exactly.

**Context:** Refer to instructions.md "API and Abstract Base Classes" for required factory signatures.

**Actions:**
1. Update `common/TankAlgorithm.h`:
```cpp
#pragma once
#include "BattleInfo.h"
#include "ActionRequest.h"
#include <memory>
#include <functional>

class TankAlgorithm {
public:
    virtual ~TankAlgorithm() {}
    virtual ActionRequest getAction() = 0;
    virtual void updateBattleInfo(BattleInfo& info) = 0;
};

using TankAlgorithmFactory = 
    std::function<std::unique_ptr<TankAlgorithm>(int player_index, int tank_index)>;
```

2. Update `common/Player.h`:
```cpp
#pragma once
#include "TankAlgorithm.h"
#include "SatelliteView.h"
#include <memory>
#include <functional>

class Player {
public:
    virtual ~Player() {}
    virtual void updateTankWithBattleInfo
        (TankAlgorithm& tank, SatelliteView& satellite_view) = 0;
};

using PlayerFactory = 
    std::function<std::unique_ptr<Player>
    (int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)>;
```

**Success Criteria:**
- [x] `TankAlgorithm.h` includes required `using TankAlgorithmFactory` declaration
- [x] `Player.h` includes required `using PlayerFactory` declaration
- [x] `Player.h` contains only pure virtual methods (no constructor)
- [x] All updated headers compile: `g++ -c common/*.h`

**End State:** Interface headers match Assignment 3 specifications exactly.

---

### Step 1A.3: Create Registration Header Stubs

**Objective:** Create registration header structures required by Assignment 3 (implementation will be added in later phases).

**Context:** Refer to instructions.md "Automatic Registration" section for macro specifications.

**Actions:**
1. Create `common/PlayerRegistration.h`:
```cpp
#pragma once
#include "Player.h"

struct PlayerRegistration {
    PlayerRegistration(PlayerFactory);
};

#define REGISTER_PLAYER(class_name) \
PlayerRegistration register_me_##class_name \
    ( [] (int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) { \
        return std::make_unique<class_name>(player_index, x, y, max_steps, num_shells); \
    } );
```

2. Create `common/TankAlgorithmRegistration.h`:
```cpp
#pragma once
#include "TankAlgorithm.h"

struct TankAlgorithmRegistration {
    TankAlgorithmRegistration(TankAlgorithmFactory);
};

#define REGISTER_TANK_ALGORITHM(class_name) \
TankAlgorithmRegistration register_me_##class_name \
    ( [](int player_index, int tank_index) { \
        return std::make_unique<class_name>(player_index, tank_index); \
    } );
```

3. Create `common/GameManagerRegistration.h`:
```cpp
#pragma once
#include "AbstractGameManager.h"

struct GameManagerRegistration {
    GameManagerRegistration(GameManagerFactory);
};

#define REGISTER_GAME_MANAGER(class_name) \
GameManagerRegistration register_me_##class_name \
    ( [] () { return std::make_unique<class_name>(); } );
```

**Success Criteria:**
- [x] All 3 registration headers exist in `common/`
- [x] Headers contain exact macro definitions from instructions.md
- [x] Headers compile without implementation: `g++ -c common/*Registration.h`

**End State:** Registration infrastructure headers exist (stubs only, no implementation).

---

### Step 1A.4: Test Interface Compilation

**Objective:** Verify all interface headers compile correctly together.

**Actions:**
1. Test all common headers compile:
```bash
cd common && g++ -c *.h
```
2. Create simple test file to verify includes work:
```cpp
// test_interfaces.cpp
#include "common/ActionRequest.h"
#include "common/BattleInfo.h"
#include "common/Player.h"
#include "common/TankAlgorithm.h"
#include "common/SatelliteView.h"
#include "common/GameResult.h"
#include "common/AbstractGameManager.h"
#include "common/PlayerRegistration.h"
#include "common/TankAlgorithmRegistration.h"
#include "common/GameManagerRegistration.h"

int main() { return 0; }
```
3. Compile test: `g++ test_interfaces.cpp -o test && rm test`

**Success Criteria:**
- [x] All headers compile individually without errors
- [x] All headers can be included together
- [x] No circular dependency issues
- [x] Test compilation succeeds

**End State:** Complete interface layer is functional and ready for implementation.

---

## Phase 1B: Directory Restructure (Structure)

### Step 1B.1: Create Project Directory Structure

**Objective:** Establish the 5-folder structure mandated by Assignment 3.

**Context:** Refer to instructions.md for exact folder requirements and organization.

**Actions:**
1. Create main project directories:
```bash
mkdir -p Simulator/
mkdir -p Algorithm/players/basic/
mkdir -p Algorithm/players/offensive/
mkdir -p GameManager/objects/
mkdir -p GameManager/factories/
mkdir -p UserCommon/utils/
mkdir -p UserCommon/bonus/
```
2. Verify `common/` directory exists with interface headers

**Success Criteria:**
- [x] All 5 main directories exist: `Simulator/`, `Algorithm/`, `GameManager/`, `common/`, `UserCommon/`
- [x] Subdirectories follow logical organization
- [x] `tree -d` shows complete directory structure

**End State:** Directory structure matches Assignment 3 requirements.

---

### Step 1B.2: Move UserCommon Files

**Objective:** Relocate shared utilities and bonus features to UserCommon, combining src/include/test hierarchies.

**Actions:**
1. Move utility classes:
```bash
mv include/utils/* UserCommon/utils/ 2>/dev/null || true
mv src/utils/* UserCommon/utils/ 2>/dev/null || true
```
2. Move bonus features:
```bash
mv include/bonus/* UserCommon/bonus/ 2>/dev/null || true
mv src/bonus/* UserCommon/bonus/ 2>/dev/null || true
```
3. Remove empty directories:
```bash
rmdir include/utils src/utils include/bonus src/bonus 2>/dev/null || true
```

**Success Criteria:**
- [x] `UserCommon/utils/` contains Point, Direction, MidPoint classes (.h, .cpp, and test files)
- [x] `UserCommon/bonus/` contains visualization, logging, CLI, analysis, board generator (with test files)
- [x] No utility or bonus files remain in original `src/` or `include/`
- [x] File count matches expected: `find UserCommon -name "*.cpp" -o -name "*.h" | wc -l`

**End State:** All shared utilities and bonus features are in UserCommon.

---

### Step 1B.3: Move GameManager Files

**Objective:** Relocate game engine components to GameManager project.

**Actions:**
1. Move core game files:
```bash
mv include/game_manager.h GameManager/ 2>/dev/null || true
mv src/game_manager.cpp GameManager/ 2>/dev/null || true
mv include/game_board.h GameManager/ 2>/dev/null || true
mv src/game_board.cpp GameManager/ 2>/dev/null || true
```
2. Move game objects:
```bash
mv include/objects/* GameManager/objects/ 2>/dev/null || true
mv src/objects/* GameManager/objects/ 2>/dev/null || true
```
3. Move supporting systems:
```bash
mv include/collision_handler.h GameManager/ 2>/dev/null || true
mv src/collision_handler.cpp GameManager/ 2>/dev/null || true
mv include/file_loader.h GameManager/ 2>/dev/null || true
mv src/file_loader.cpp GameManager/ 2>/dev/null || true
mv include/satellite_view_impl.h GameManager/ 2>/dev/null || true
mv src/satellite_view_impl.cpp GameManager/ 2>/dev/null || true
```
4. Move factories:
```bash
mv include/factories/* GameManager/factories/ 2>/dev/null || true
mv src/factories/* GameManager/factories/ 2>/dev/null || true
```

**Success Criteria:**
- [x] `GameManager/` contains game_manager, game_board, collision_handler, file_loader, satellite_view_impl
- [x] `GameManager/objects/` contains tank, shell, game_object classes
- [x] `GameManager/factories/` contains all factory implementations
- [x] No game engine files remain in original directories

**End State:** All game engine components are in GameManager project.

---

### Step 1B.4: Move Algorithm Files

**Objective:** Relocate player strategy implementations to Algorithm project.

**Actions:**
1. Move player strategies:
```bash
mv include/players/* Algorithm/players/ 2>/dev/null || true
mv src/players/* Algorithm/players/ 2>/dev/null || true
```

**Success Criteria:**
- [x] `Algorithm/players/basic/` contains BasicPlayer and BasicTankAlgorithm
- [x] `Algorithm/players/offensive/` contains OffensivePlayer and OffensiveTankAlgorithm
- [x] No player files remain in original directories

**End State:** All player strategy implementations are in Algorithm project.

---

### Step 1B.5: Move Simulator Files

**Objective:** Relocate main executable to Simulator project.

**Actions:**
1. Move main file:
```bash
mv src/main.cpp Simulator/main.cpp
```
2. Remove old build system:
```bash
rm -f CMakeLists.txt
rm -rf build/
```

**Success Criteria:**
- [x] `Simulator/main.cpp` exists
- [ ] No CMake files remain (will remove after creating new build system)
- [x] Original `src/` and `include/` directories removed, test mocks moved to `UserCommon/utils/testing/`

**End State:** Main executable is in Simulator, old build system removed.

---

### Step 1B.6: Update Include Paths

**Objective:** Fix all include statements to use absolute paths from project root.

**Actions:**
1. Update UserCommon includes:
   - Change `#include "point.h"` → `#include "UserCommon/utils/point.h"`
   - Update all internal UserCommon references

2. Update GameManager includes:
   - Change `#include "utils/point.h"` → `#include "UserCommon/utils/point.h"`
   - Change `#include "Player.h"` → `#include "common/Player.h"`
   - Update all GameManager internal references

3. Update Algorithm includes:
   - Change `#include "TankAlgorithm.h"` → `#include "common/TankAlgorithm.h"`
   - Change `#include "utils/point.h"` → `#include "UserCommon/utils/point.h"`

4. Update Simulator includes:
   - Change `#include "game_manager.h"` → `#include "GameManager/game_manager.h"`
   - Add factory includes as needed

**Success Criteria:**
- [x] All includes use absolute paths from project root
- [x] No relative paths (`../`) remain in any files
- [x] Each project compiles individually: `cd [Project] && g++ -c *.cpp *.h -I..`
- [x] All header files (.h) follow standardized include structure:
  - #pragma once
  - blank line
  - all system includes (#include <...>) grouped together and alphabetically sorted
  - blank line
  - all local includes (#include "...") grouped together and alphabetically sorted
- [x] All CPP files (.cpp) follow standardized include structure:
  - all system includes (#include <...>) grouped together and alphabetically sorted
  - blank line
  - all local includes (#include "...") grouped together and alphabetically sorted

**End State:** All include paths use absolute references, follow standardized structure, and compile correctly.

---

## Phase 1C: Build System Creation (Infrastructure)

### Step 1C.1: Create Individual Project Makefiles

**Objective:** Establish build system for each project as static libraries.

**Actions:**
1. Create `UserCommon/Makefile`:
```makefile
SOURCES := $(shell find . -name "*.cpp" | grep -v bonus)
OBJECTS := $(SOURCES:.cpp=.o)

libusercommon.a: $(OBJECTS)
	ar rcs $@ $(OBJECTS)

%.o: %.cpp
	g++ -c $< -o $@ -I..

clean:
	rm -f $(OBJECTS) libusercommon.a

.PHONY: clean
```

2. Create `GameManager/Makefile`:
```makefile
SOURCES := $(shell find . -name "*.cpp")
OBJECTS := $(SOURCES:.cpp=.o)

libgamemanager.a: $(OBJECTS)
	ar rcs $@ $(OBJECTS)

%.o: %.cpp
	g++ -c $< -o $@ -I..

clean:
	rm -f $(OBJECTS) libgamemanager.a

.PHONY: clean
```

3. Create `Algorithm/Makefile`:
```makefile
SOURCES := $(shell find . -name "*.cpp")
OBJECTS := $(SOURCES:.cpp=.o)

libalgorithm.a: $(OBJECTS)
	ar rcs $@ $(OBJECTS)

%.o: %.cpp
	g++ -c $< -o $@ -I..

clean:
	rm -f $(OBJECTS) libalgorithm.a

.PHONY: clean
```

4. Create `Simulator/Makefile`:
```makefile
simulator: main.o
	g++ -o $@ $< -L../UserCommon -L../GameManager -L../Algorithm \
		-lusercommon -lgamemanager -lalgorithm

main.o: main.cpp
	g++ -c $< -o $@ -I..

clean:
	rm -f main.o simulator

.PHONY: clean
```

**Success Criteria:**
- [ ] Each project builds successfully: `cd [Project] && make`
- [ ] Static libraries are created: `ls */lib*.a`
- [ ] No compilation errors or warnings

**End State:** Each project has working Makefile and builds independently.

---

### Step 1C.2: Create Root Makefile

**Objective:** Provide coordinated build system respecting dependencies.

**Actions:**
1. Create root `Makefile`:
```makefile
all: usercommon gamemanager algorithm simulator

usercommon:
	$(MAKE) -C UserCommon

gamemanager: usercommon
	$(MAKE) -C GameManager

algorithm: usercommon
	$(MAKE) -C Algorithm

simulator: usercommon gamemanager algorithm
	$(MAKE) -C Simulator

clean:
	$(MAKE) -C UserCommon clean
	$(MAKE) -C GameManager clean
	$(MAKE) -C Algorithm clean
	$(MAKE) -C Simulator clean

.PHONY: all usercommon gamemanager algorithm simulator clean
```

**Success Criteria:**
- [ ] `make all` builds everything in correct order
- [ ] `make clean` cleans all projects
- [ ] Dependencies are respected (UserCommon built first)

**End State:** Complete build system works from root directory.

---

### Step 1C.3: Test Full Build System

**Objective:** Verify complete system builds and basic functionality works.

**Actions:**
1. Test full build:
```bash
make clean && make all
```
2. Verify artifacts:
```bash
ls -la UserCommon/libusercommon.a
ls -la GameManager/libgamemanager.a
ls -la Algorithm/libalgorithm.a
ls -la Simulator/simulator
```
3. Test basic execution:
```bash
./Simulator/simulator
```

**Success Criteria:**
- [ ] Full build completes without errors
- [ ] All libraries and executable are created
- [ ] Simulator runs (may show usage, but no crashes)

**End State:** Complete build system functional, ready for interface refactoring.

---

## Phase 1D: GameManager Interface Refactoring (Core Logic)

### Step 1D.1: Add GameResult Support

**Objective:** Update current GameManager to use GameResult struct for game outcomes.

**Context:** Current game result handling needs to match GameResult struct from instructions.md.

**Actions:**
1. Update `GameManager/game_manager.h`:
   - Add `#include "common/GameResult.h"`
   - Change return type of result methods to use GameResult
   - Add private method to convert current result format to GameResult

2. Update `GameManager/game_manager.cpp`:
   - Implement GameResult population logic
   - Map current win/tie detection to GameResult::winner and GameResult::reason
   - Populate remaining_tanks vector with current tank counts

**Success Criteria:**
- [ ] GameManager includes and uses GameResult struct
- [ ] Game ending logic populates GameResult correctly
- [ ] Current game functionality still works end-to-end
- [ ] `make gamemanager` compiles without errors

**End State:** GameManager uses GameResult for all game outcome reporting.

---

### Unfinished tasks:
- create namespaces

### Step 1D.2: Implement AbstractGameManager Interface

**Objective:** Make current GameManager implement the AbstractGameManager interface required by Assignment 3.

**Context:** Refer to instructions.md AbstractGameManager interface specification.

**Actions:**
1. Update `GameManager/game_manager.h`:
   - Add `#include "common/AbstractGameManager.h"`
   - Make GameManager inherit from AbstractGameManager
   - Declare the run() method with exact signature from instructions.md

2. Update `GameManager/game_manager.cpp`:
   - Implement the run() method with required signature
   - Adapt current run logic to work with new parameters
   - Create SatelliteView snapshot from current board state
   - Use provided Player references and factory functions

**Success Criteria:**
- [ ] GameManager inherits from AbstractGameManager
- [ ] run() method has exact signature from instructions.md
- [ ] Implementation uses provided players and factories correctly
- [ ] `make gamemanager` compiles without errors

**End State:** GameManager fully implements Assignment 3 interface requirements.

---

### Step 1D.3: Test Refactored GameManager

**Objective:** Verify refactored GameManager works with existing code.

**Actions:**
1. Create simple test to verify GameManager functionality:
```cpp
// test_gamemanager.cpp
#include "GameManager/game_manager.h"
#include "common/AbstractGameManager.h"

int main() {
    // Basic test that GameManager can be created and interface is correct
    // (Full testing will happen after factory refactoring)
    return 0;
}
```
2. Compile test: `g++ test_gamemanager.cpp -I. -o test && rm test`

**Success Criteria:**
- [ ] GameManager compiles with new interface
- [ ] Basic instantiation works
- [ ] Interface inheritance is correct

**End State:** Refactored GameManager is ready for factory integration.

---

## Phase 1E: Factory System Refactoring (Integration)

### Step 1E.1: Remove Factory Class Inheritance

**Objective:** Convert existing factory classes from inheritance-based to standalone classes.

**Context:** Assignment 3 requires function-based factories, not class inheritance.

**Actions:**
1. Update `GameManager/factories/basic_player_factory.h`:
   - Remove inheritance from PlayerFactory interface
   - Keep the create() method but make class standalone
   - Remove any virtual inheritance

2. Update `GameManager/factories/basic_tank_algorithm_factory.h`:
   - Remove inheritance from TankAlgorithmFactory interface
   - Keep the create() method but make class standalone
   - Remove any virtual inheritance

**Success Criteria:**
- [ ] Factory classes no longer inherit from interface classes
- [ ] Factory classes retain their create() methods
- [ ] `make gamemanager` compiles successfully

**End State:** Factory classes are standalone, ready for function conversion.

---

### Step 1E.2: Create Function-Based Factory Integration

**Objective:** Update main.cpp to use function-based factories while leveraging existing factory classes.

**Actions:**
1. Update `Simulator/main.cpp`:
   - Create function-based factories that use existing factory classes:
```cpp
#include "GameManager/factories/basic_player_factory.h"
#include "GameManager/factories/basic_tank_algorithm_factory.h"

// Create function-based factories
PlayerFactory createPlayerFactory() {
    static BasicPlayerFactory factory;
    return [&factory](int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) {
        return factory.create(player_index, x, y, max_steps, num_shells);
    };
}

TankAlgorithmFactory createTankAlgorithmFactory() {
    static BasicTankAlgorithmFactory factory;
    return [&factory](int player_index, int tank_index) {
        return factory.create(player_index, tank_index);
    };
}
```
2. Update main() to use function-based factories with AbstractGameManager

**Success Criteria:**
- [ ] main.cpp creates and uses function-based factories
- [ ] Function factories utilize existing factory classes
- [ ] AbstractGameManager receives proper factory functions
- [ ] `make simulator` compiles successfully

**End State:** Complete system uses Assignment 3 function-based factory pattern.

---

### Step 1E.3: Test Complete Factory System

**Objective:** Verify end-to-end functionality with new factory system.

**Actions:**
1. Build complete system:
```bash
make clean && make all
```
2. Test basic game functionality:
```bash
./Simulator/simulator [test_board_file]
```
3. Verify factory functions create correct objects

**Success Criteria:**
- [ ] Complete system builds without errors
- [ ] Simulator executable runs games successfully
- [ ] Factory functions create working Player and TankAlgorithm objects
- [ ] Game results use GameResult struct correctly

**End State:** Complete Phase 1 - system matches Assignment 3 requirements and is ready for Phase 2 (namespaces and shared libraries).

---

## Phase 1 Completion Checklist

**Interface Compliance:**
- [ ] All Assignment 3 interfaces exist in `common/`
- [ ] GameManager implements AbstractGameManager
- [ ] Function-based factories replace class-based factories
- [ ] GameResult struct used for all game outcomes

**Directory Structure:**
- [ ] 5 projects: Simulator/, Algorithm/, GameManager/, common/, UserCommon/
- [ ] Files properly organized according to Assignment 3
- [ ] No files remain in old src/ or include/ directories

**Build System:**
- [ ] Each project has working Makefile
- [ ] Root Makefile coordinates builds with dependencies
- [ ] Static libraries build successfully
- [ ] Complete system builds: `make clean && make all`

**Functionality:**
- [ ] Simulator executable runs games
- [ ] All include paths use absolute references
- [ ] System ready for Phase 2 (namespaces and .so conversion)