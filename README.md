# 💥 Tank Battle Game 💥

## Overview

A tank battle simulation framework supporting 2 players with modular architecture. The system uses shared libraries for algorithms and game managers, enabling dynamic strategy loading and comparison. Player assignment follows modulo 2 logic (odd players = Basic/Defensive, even players = Offensive).

**Key Features:**
- 2-player tank battles
- Dynamic algorithm loading via shared libraries
- Web-based UI for game configuration and execution
- Real-time visualization with step-by-step replay

## Project Architecture

### Directory Structure

```
📁 c-Game_Board/
├── 📁 Algorithm/              # Player algorithm implementations (shared library)
├── 📁 GameManager/           # Game logic and management (shared library)  
├── 📁 Simulator/             # Main simulation engine and entry point
├── 📁 UserCommon/           # Shared utilities and game objects
│   ├── 📁 objects/          # Game entities (Tank, Shell, etc.)
│   ├── 📁 utils/            # Utility classes (Point, Direction, etc.)
│   └── 📁 bonus/            # Bonus features
│       ├── 📁 ui_server/    # Web-based UI server
│       └── 📁 visualization/ # Game visualization system
└── 📁 common/               # Interface definitions and contracts
```

### Shared Libraries System

The project uses modular shared libraries (.so files) for algorithms and game management, enabling dynamic loading of different player strategies and game rules.

### Execution Modes

1. **Basic Mode**: Single game between two algorithms using one game manager
2. **Comparative Mode**: Test two algorithms across multiple game managers  
3. **Competition Mode**: Tournament between algorithms across multiple maps

## Input File Format

Specifies board layout:

* First five lines (header):
  1. Map name/description (string)
  2. MaxSteps = <NUM>
  3. NumShells = <NUM>
  4. Rows = <NUM>
  5. Cols = <NUM>
* Subsequent lines: board content (`#`: Wall (2 hits), `1`/`2`: Player tanks, `@`: Mine, ` `: Empty).
* In order to start the game the file must include all header lines.

### Example Board Files

**2-Player Battle:**
```
Complex Battle Arena
MaxSteps = 1000
NumShells = 15
Rows = 15
Cols = 15
  #    #@       
  #  2             
          #  #   
   ## #           
 1 @ @#@    #    
   @     @   2     
            # @ 
       1@ @      
 @@@ @#   @   #
  @    @#@      
   @ #        #
    @      #  #  
 1     @@#  @#  
       #          
      # 2      
```


## Output Format

The game generates output files in the same directory as the input file:

1. **output_<input_file>.txt** - Complete game log with player moves and final results
2. **output_<input_file>_visualization.html** - Interactive game replay (if visualization enabled)
3. **input_errors.txt** - Error handling log (if input errors occur)

## Building the Project

### Build Options

The project supports multiple build configurations using CMake options:

```bash
# Standard build (basic functionality only)
mkdir build && cd build
cmake ..
make

# Full build with all features enabled
cmake -DENABLE_TESTING=ON -DENABLE_VISUALIZATION=ON -DENABLE_UI=ON ..
make
```

**Available Build Options:**
- `DENABLE_TESTING=ON`: Builds unit tests using Google Test framework
- `DENABLE_VISUALIZATION=ON`: Enables HTML visualization generation
- `DENABLE_UI=ON`: Builds the web-based UI server

### Running the Game

The simulator supports three execution modes, each with specific command-line arguments and output formats:

#### 1. Basic Mode
Single game between two algorithms using one game manager.

**Command:**
```bash
./simulator_318835816_211314471 -basic \
  game_map=<input_file.txt> \
  game_manager=../GameManager/GameManager_318835816_211314471.so \
  algorithm1=../Algorithm/Algorithm_318835816_211314471.so \
  algorithm2=<path_to_second_algorithm.so> \
  [-verbose]
```

**Output:** Game results printed to console, full game log if verbose flag is set.

#### 2. Comparative Mode  
Compare multiple game managers with the same algorithms on a single map.

**Command:**
```bash
./simulator_318835816_211314471 -comparative \
  game_map=<input_file.txt> \
  game_managers_folder=<path_to_gm_folder> \
  algorithm1=<algorithm1.so> \
  algorithm2=<algorithm2.so> \
  [num_threads=<num>] [-verbose]
```

**Output:** `comparative_results_<timestamp>.txt` in the game managers folder, containing:
- Input parameters (map, algorithms)  
- Results grouped by identical outcomes
- Final game state for each result group

#### 3. Competition Mode
Tournament between multiple algorithms across multiple maps.

**Command:**
```bash
./simulator_318835816_211314471 -competition \
  game_maps_folder=<maps_folder> \
  game_manager=<game_manager.so> \
  algorithms_folder=<algorithms_folder> \
  [num_threads=<num>] [-verbose]
```

**Output:** `competition_<timestamp>.txt` in the algorithms folder, containing:
- Tournament configuration
- Final leaderboard sorted by score (3 pts/win, 1 pt/tie, 0 pts/loss)

#### Web UI Usage
```bash
# Start web server
./tank_simulator_ui

# Open browser to http://localhost:8080
# Configure and run any mode through the web interface
```


## Bonus Features

The project includes several advanced features beyond the basic requirements. For complete technical details, see `docs/bonus.txt`.

### 1. Web-Based UI Server 🌐

A complete browser-based interface for game configuration, execution, and monitoring.

**Features:**
- **Multi-Mode Support**: Basic, Comparative, and Competition game modes
- **Real-Time Execution**: Live console output streaming and process monitoring
- **Interactive Configuration**: Dynamic forms with file path validation
- **Professional Results Display**: Parsed results with leaderboards and visualizations
- **RESTful API**: Backend provides endpoints for simulation control and result retrieval

**Usage:**
```bash
# Build with UI enabled
cmake -DENABLE_UI=ON ..
make tank_simulator_ui

# Start the web server
./tank_simulator_ui

# Open browser to: http://localhost:8080
```

**Architecture:**
- **Backend**: C++ HTTP server using cpp-httplib 
- **Frontend**: Modern HTML5/CSS3/JavaScript with responsive design
- **Process Management**: Real-time command execution with output capture

### 2. Interactive Visualization System

HTML-based game visualization with step-by-step replay capabilities.

**Features:**
- **Step-by-Step Replay**: Interactive controls for game progression
- **2-Player Display**: Visual identifiers for both players
- **Rich Game State**: Tank positions, directions, shells, mines, walls, and health status
- **Player Statistics**: Remaining ammunition and active/destroyed tank status
- **Action Descriptions**: Detailed explanations of each player's chosen actions

**Compilation Requirements:**
```bash
# Build with visualization enabled
cmake -DENABLE_VISUALIZATION=ON ..
make
```

**Usage:**
```bash
# Command-line usage (requires -verbose flag for visualization generation)
# Game manager should be compiled with -DENABLE_VISUALIZATION=ON
./simulator_318835816_211314471 -basic \
  game_map=<input_file.txt> \
  game_manager=<game_manager.so> \  
  algorithm1=<algorithm1.so> \
  algorithm2=<algorithm2.so> \
  -verbose

# Open the generated visualization file in browser
# File: output_<input_file>_visualization.html
```

**Important Notes:**
- **Verbose Mode Required**: Visualization is only generated when the `-verbose` flag is used or verbose mode is enabled in the web UI
- **Compilation Dependency**: Visualization features require `DENABLE_VISUALIZATION=ON` during build

### 3. Comprehensive Testing Framework

Unit testing using Google Test with extensive coverage:

- **Component Tests**: GameBoard, Tank, Shell, Algorithms, Collision mechanics
- **Edge Case Testing**: Board wrapping, collision detection, game rules validation
- **Mock Objects**: Isolated testing of algorithmic behavior
- **Test Fixtures**: Complex scenario testing with reproducible conditions

**Usage:**
```bash
# Build with testing enabled
cmake -DENABLE_TESTING=ON ..
make

# Run all tests
./build/tests/usercommon_tests        # UserCommon library tests
./build/tests/algorithm_tests         # Algorithm implementation tests  
./build/tests/GameManager_tests       # Game manager tests
./build/tests/Simulator_tests         # Simulator engine tests
```