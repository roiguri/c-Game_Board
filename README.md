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

Steps:

```bash
mkdir build && cd build
cmake ..
make
```

## Running the Game
```bash
./tank_battle <game_board_input_file>
Output in: output_<game_board_input_file>.txt.
```

## Algorithms
Algorithm Options:
* Chase: Pursues enemy using pathfinding.
* Defensive: Avoids danger, shoots when safe.
Both algorithms avoid mines and react to shells.

## Bonus Features
Details in bonus.txt:
- Interactive HTML visualization.
- Comprehensive Google Test suite.