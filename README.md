# 💥 Tank Battle Game 💥

## Overview

A program simulating a two-tank battle on a 2D board. 
Features algorithmic strategies, walls, mines, and artillery with collision
mechanics.

## Input File Format

Specifies board layout:

* First line: `<width> <height>` (dimensions).
* Subsequent lines: board content (`#`: Wall (2 hits), `1`: Player 1 start, `2`: Player 2 start, `@`: Mine, ` `: Empty).

## Output Format

Two output files are generated:

1. **input_errors.txt** - Documents all handled input errors

2. **output_<game_board_input_file>.txt**
  * Contains:
    * All player steps (including invalid ones)
    * Final result (win/tie) and reason

## Building the Project

### Standard Build (no bonus tests or visualization):

```bash
mkdir build && cd build
cmake ..
make
```

## Complete build
```bash
mkdir build && cd build
cmake -DENABLE_TESTING=ON -DENABLE_VISUALIZATION=ON ..
make
```

## Running the Game
```bash
./tanks_game <input_file>
```
Output will be generated in in: <input_file_path>/output_<input_file>.txt.

## Algorithms
Algorithm Options:
* Chase: Pursues enemy using pathfinding.
* Defensive: Avoids danger, shoots when safe.
Both algorithms avoid mines and react to shells.

## Bonus Features
Details in docs/bonus.txt

### Running Tests
If tests are enabled in the build
```bash
./bin/tests/tanks_game_tests
```

### Visualization
If visualization is enabled in the build, an HTML visualization will be generated alongside the output file.