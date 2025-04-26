# 💥 Tank Battle Game 💥

## Overview

A program simulating a two-tank battle on a 2D board. 
Features algorithmic strategies, walls, mines, and artillery with collision
mechanics.

## Input File Format

Specifies board layout:

* First line: `<width> <height>` (dimensions).
* Subsequent lines: board content (`#`: Wall (2 hits), `1`: Player 1 start, `2`: Player 2 start, `@`: Mine, ` `: Empty).
* In order to start the game the file must include dimensions and tanks.

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
```bash
./tanks_game <input_file>
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

### 1. GTest
If tests are enabled, run the tests with:
```bash
./bin/tests/tanks_game_tests
```

### Visualization
If visualization is enabled in the build, an HTML visualization will be generated alongside the output file.

### Board Generator
#### Usage:

Generate a board without running the game:
```bash
./tanks_game --only_generate [--config-path=<path>]
```

Generate a board and immediately run the game:
```bash
./tanks_game --run_generated [--config-path=<path>]
```

When running the generator without the --config-path flag, default values are
used.

#### Configuration File Format:

| Parameter | Value | Default Value |
|:---------:|:-----:|:-------------:|
| dimensions | \<width\> \<height\> | 15 10 |
| wall_density | 0.0-0.9 | 0.25 |
| mine_density | 0.0-0.5 | 0.25 |
| symmetry | none\|horizontal\|vertical\|diagonal | none |
| seed | \<number\> | no seed |