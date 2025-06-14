# 💥 Tank Battle Game 💥

## Overview

A program simulating multi-player tank battles on a 2D board supporting 2-9 players. 
Features algorithmic strategies, walls, mines, and artillery with collision
mechanics. Player assignment follows modulo 2 logic (odd players = Basic/Defensive, 
even players = Offensive).

## Input File Format

Specifies board layout:

* First five lines (header):
  1. Map name/description (string)
  2. MaxSteps = <NUM>
  3. NumShells = <NUM>
  4. Rows = <NUM>
  5. Cols = <NUM>
* Subsequent lines: board content (`#`: Wall (2 hits), `1`-`9`: Player tanks, `@`: Mine, ` `: Empty).
* In order to start the game the file must include all header lines.
* Supports 2-9 players (must include players 1 and 2 at minimum).

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

**Multi-Player Battle (up to 9 players):**
```
A great map
MaxSteps = 500
NumShells = 20
Rows = 15
Cols = 15
   @ @    # ##@
   3         # 
 # #     # @  #
    @ #        
@     2#@ @    
4     # #   @#
      #     # #
    @ ##@@ #   
 @ #    2  @  #
  #@#@   1  2# 
 ###   #       
 @@   # @  #@ @
 ##  ## 1  #@# 
#     #   3   #
1 56789   ## #
```

## Output Format

Two output files are generated:

1. **input_errors.txt** - Documents all handled input errors

2. **output_<game_board_input_file>.txt**
  * Contains:
    * All player steps (including invalid ones)
    * Final result (win/tie) and reason

## Building the Project

### Standard build:

```bash
mkdir build && cd build
cmake ..
make
```

### Running the Game
From the build directory:
```bash
./bin/tanks_game <input_file>
```
Output will be generated in: `<input_file_path>/output_<input_file>.txt.`


## Bonus Features
More details in docs/bonus.txt

### Build with bonus features
```bash
mkdir build && cd build
cmake -DENABLE_TESTING=ON -DENABLE_VISUALIZATION=ON ..
make
```


### 1. CLI System
Command-line interface with argument parsing and validation.
Centralizes parsing arguments for all bonus features.

#### Usage: 
```bash
# Show help
./bin/tanks_game --help

# All logging options and board generation options described below
```

### 2. GTest
If tests are enabled, run the tests with:
```bash
./bin/tests/tanks_game_tests
```

### 3. Visualization
If visualization is enabled in the build, an HTML visualization will be generated alongside the output file.

### 4. Board Generator
#### Usage:

Generate a board without running the game:
```bash
./bin/tanks_game --only_generate [--config_path <path>]
```

Generate a board and immediately run the game:
```bash
./bin/tanks_game --run_generated [--config_path <path>]
```

When running the generator without the --config-path flag, default values are
used.

#### Configuration File Format:

| Parameter   | Value                                 | Default Value   |
|:-----------:|:-------------------------------------:|:--------------:|
| dimensions  | <width> <height>                      | 15 10          |
| wall_density| 0.0-0.9                               | 0.25           |
| mine_density| 0.0-0.5                               | 0.25           |
| symmetry    | none|horizontal|vertical|diagonal      | none           |
| seed        | <number>                              | no seed        |
| max_steps   | <number>                              | 1000           |
| num_shells  | <number>                              | 10             |
| map_name    | <string>                              | Generated Map  |

### 5. Logger System
A lightweight, configurable logging system that helps with debugging and monitoring game execution.

#### Usage:
```bash
# Enable logging
./bin/tanks_game ... --enable_logging

# Configure log level, destination, and file path
./bin/tanks_game ... --enable_logging --log_level=debug --log_to_file --log_file=game.log
```

The logger supports multiple log levels (debug, info, warning, error) and can output to console, file, or both. When disabled (default), it has no impact on the game.

### Player Analysis Tool
Automated testing framework for comparing algorithm performance across multiple game dimensions.

#### Usage:
```bash
# Run analysis with default configuration
./bin/analysis_tool

# Run with custom configuration
./bin/analysis_tool analysis_config.json
```

#### Analysis Configuration
example:
```json
{
  "boardSizes": [10, 15, 20],
  "wallDensities": [0.1, 0.25, 0.4],
  "mineDensities": [0.05, 0.1, 0.15],
  "numSamples": 10,
  "numShells": [16, 20, 24],
  "numTanksPerPlayer": [1, 2, 3]
}
```