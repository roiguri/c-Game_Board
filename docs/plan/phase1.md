# Phase 1: Project Restructuring - Implementation Plan

## Overview
This plan restructures the Tank Battle codebase from a single-project CMake build to the multi-project structure required by Assignment 3, using traditional Makefiles with static libraries.

## Step 1: Create Directory Structure
Create the 5 main folders and subdirectories maintaining current hierarchy:

```bash
mkdir -p Simulator/
mkdir -p Algorithm/players/basic/
mkdir -p Algorithm/players/offensive/
mkdir -p Algorithm/factories/
mkdir -p GameManager/objects/
mkdir -p GameManager/factories/
mkdir -p UserCommon/utils/
mkdir -p UserCommon/bonus/
# common/ already exists
```

**Test:** `find . -type d | sort` should show new structure

## Step 2: Move common/ Files (Assignment Provided)
The `common/` directory should already contain the assignment-provided interface files.

**Verify files exist:**
- `ActionRequest.h`, `BattleInfo.h`, `Player.h`, `SatelliteView.h`, `TankAlgorithm.h`, etc.

**Test:** `ls common/` shows all interface files

## Step 3: Move UserCommon Files (Shared Utilities + Bonus)
Combine src/include hierarchies into single project structure:

```bash
# Move utils (combine src and include)
mv include/utils/* UserCommon/utils/
mv src/utils/* UserCommon/utils/

# Move bonus features (combine src and include) 
mv include/bonus/* UserCommon/bonus/
mv src/bonus/* UserCommon/bonus/
```

**Result:** `UserCommon/utils/point.h`, `UserCommon/utils/point.cpp`, etc.

**Test:** 
```bash
cd UserCommon && find . -name "*.h" -o -name "*.cpp" | wc -l
# Should show all utils + bonus files
```

## Step 4: Move GameManager Core (Game Engine)
Move all game engine components, combining src/include:

```bash
# Core game files
mv include/game_manager.h GameManager/
mv src/game_manager.cpp GameManager/
mv include/game_board.h GameManager/
mv src/game_board.cpp GameManager/

# Game objects
mv include/objects/* GameManager/objects/
mv src/objects/* GameManager/objects/

# Supporting systems
mv include/collision_handler.h GameManager/
mv src/collision_handler.cpp GameManager/
mv include/file_loader.h GameManager/
mv src/file_loader.cpp GameManager/
mv include/satellite_view_impl.h GameManager/
mv src/satellite_view_impl.cpp GameManager/

# Factories (belong with game engine)
mv include/factories/basic_*_factory.h GameManager/factories/
mv src/factories/basic_*_factory.cpp GameManager/factories/
```

**Test:**
```bash
cd GameManager && find . -name "*.h" -o -name "*.cpp" | wc -l
# Should show all game engine files
```

## Step 5: Move Algorithm Files (Player Strategies)
Move player strategy implementations:

```bash
# Move players (maintain basic/offensive structure)
mv include/players/* Algorithm/players/
mv src/players/* Algorithm/players/
```

**Test:**
```bash
cd Algorithm && find . -name "*.h" -o -name "*.cpp" | wc -l
# Should show all player strategy files
```

## Step 6: Move Simulator Files (Main Executable)
```bash
mv src/main.cpp Simulator/main.cpp
```

**Test:** `ls Simulator/` shows `main.cpp`

## Step 7: Remove Old CMake (Since Directory Structure Changed)
```bash
rm -f CMakeLists.txt
rm -rf build/
```

**Test:** No CMake files remain in root

## Step 8: Update Include Paths (Project by Project)

### 8a: UserCommon
Update internal includes within utils/ and bonus/:
- Change `#include "utils/point.h"` → `#include "UserCommon/utils/point.h"`
- Change relative includes to absolute paths

**Test:** 
```bash
cd UserCommon && g++ -c utils/point.cpp -I.. 
# Should compile successfully
```

### 8b: GameManager  
Update includes to reference UserCommon and common:
- Change `#include "utils/point.h"` → `#include "UserCommon/utils/point.h"`
- Change `#include "Player.h"` → `#include "common/Player.h"`
- Update all game engine internal includes

**Test:**
```bash
cd GameManager && g++ -c game_board.cpp -I..
# Should compile successfully
```

### 8c: Algorithm
Update includes to reference UserCommon and common:
- Change `#include "TankAlgorithm.h"` → `#include "common/TankAlgorithm.h"`
- Change `#include "Player.h"` → `#include "common/Player.h"`
- Update utils references to UserCommon

**Test:**
```bash
cd Algorithm && g++ -c players/basic/basic_player.cpp -I..
# Should compile successfully
```

### 8d: Simulator
Update includes to reference all projects:
- Change `#include "game_manager.h"` → `#include "GameManager/game_manager.h"`
- Add includes for factories and other components

**Test:**
```bash
cd Simulator && g++ -c main.cpp -I..
# Should compile successfully
```

## Step 9: Create Minimal Makefiles

### 9a: UserCommon/Makefile
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

### 9b: GameManager/Makefile
```makefile
SOURCES := $(shell find . -name "*.cpp")
OBJECTS := $(SOURCES:.cpp=.o)

libgamemanager.a: $(OBJECTS)
	ar rcs $@ $(OBJECTS)

%.o: %.cpp
	g++ -c $< -o $@ -I.. -I../UserCommon

clean:
	rm -f $(OBJECTS) libgamemanager.a

.PHONY: clean
```

### 9c: Algorithm/Makefile
```makefile
SOURCES := $(shell find . -name "*.cpp")
OBJECTS := $(SOURCES:.cpp=.o)

libalgorithm.a: $(OBJECTS)
	ar rcs $@ $(OBJECTS)

%.o: %.cpp
	g++ -c $< -o $@ -I.. -I../UserCommon

clean:
	rm -f $(OBJECTS) libalgorithm.a

.PHONY: clean
```

### 9d: Simulator/Makefile
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

**Test After Each Makefile:**
```bash
cd [ProjectDir] && make && echo "SUCCESS: $PWD compiled"
```

## Step 10: Create Root Makefile
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

**Test:** `make clean && make all` should build everything

## Step 11: Final Validation
```bash
# Full clean build
make clean && make all

# Verify executable exists
ls -la Simulator/simulator

# Test basic execution (may show usage, but shouldn't crash)
./Simulator/simulator
```

---

## Validation Commands Summary
```bash
# After file moves
find . -name "*.cpp" -o -name "*.h" | wc -l  # Count total files

# After include updates  
find . -name "*.cpp" | xargs grep "#include.*/" | head -10  # Check paths

# After makefiles
make clean && make all  # Full build test

# Verify structure
tree -d  # Show directory structure
```

## Key Notes
- **No namespaces yet** - saving for Phase 2 after structure is stable
- **Minimal makefiles** - just compile and link, no bonus features
- **Static libraries first** - easier to debug before converting to .so files
- **Absolute include paths only** - `#include "UserCommon/utils/point.h"`
- **Test after each step** - catch issues early before they compound

## Success Criteria
✅ All 5 project directories exist with correct file organization  
✅ All projects compile individually with their Makefiles  
✅ Root Makefile builds everything in correct dependency order  
✅ Simulator executable is created and can run basic commands  
✅ No CMake dependencies remain

## Next steps - removed from phase 1:
- Adding namespace.
- Adding bonus support.