# GameManager Refactoring PRD

## Overview

The GameManager class needs to be refactored to comply with Assignment 3's AbstractGameManager interface while maintaining backward compatibility during transition. This refactoring will separate concerns between file I/O (moving to Simulator) and core game execution (staying in GameManager), enabling the multi-threaded simulation architecture required for the assignment.

The current GameManager handles everything from file loading to output generation. The refactored version will focus solely on game execution, receiving all necessary data through method parameters and returning structured results rather than writing files directly.

## Core Features

### AbstractGameManager Interface Compliance
**What it does**: Implements the required `run()` method signature from AbstractGameManager interface
**Why it's important**: Enables dynamic loading and polymorphic usage by the Simulator
**How it works**: Single entry point that receives all game parameters and returns GameResult

### SatelliteView Integration
**What it does**: Parses SatelliteView input to create internal game state (GameBoard, tanks)
**Why it's important**: Decouples GameManager from file I/O, enabling flexible input sources
**How it works**: Converts SatelliteView character grid to internal game objects using existing parsing logic

### Factory-Based Algorithm Creation
**What it does**: Uses provided TankAlgorithmFactory functions to create tank algorithms
**Why it's important**: Enables dynamic algorithm loading and per-player algorithm assignment
**How it works**: Calls factory functions for each tank with appropriate player_index and tank_index

### Structured Result Return
**What it does**: Returns GameResult struct instead of writing output files
**Why it's important**: Separates game execution from output formatting, enables result aggregation
**How it works**: Converts internal game state and outcome to standardized GameResult format

### Two-Player Constraint
**What it does**: Restricts operation to exactly 2 players (players 1 and 2)
**Why it's important**: Simplifies interface compliance and initial implementation
**How it works**: Filters tank positions and ignores players 3-9 during parsing

## User Experience

### Developer Personas
- **Simulator Developer**: Needs clean interface to run games with different algorithms/maps
- **Algorithm Developer**: Expects consistent factory-based instantiation
- **Assignment Grader**: Requires exact AbstractGameManager interface compliance

### Key Integration Flows
1. **Simulator → GameManager**: Loads map file → creates SatelliteView → calls GameManager.run()
2. **GameManager Internal**: Parses SatelliteView → creates game objects → runs game loop → returns result
3. **Result Processing**: Simulator receives GameResult → formats output → handles file I/O

### Interface Considerations
- Maintain existing game mechanics and collision detection
- Preserve visualization integration points
- Keep error handling robust but return errors rather than printing

## Technical Architecture

### System Components
```
GameManager (Refactored)
├── readSatelliteView() - Parse input to game state
├── run() - AbstractGameManager interface implementation  
├── runGameLoop() - Core game execution (unchanged)
├── checkGameOver() - Modified to return GameResult
└── Helper Methods - Factory integration, parsing utilities

Dependencies:
├── AbstractGameManager (interface)
├── SatelliteView (input)
├── GameResult (output)
├── TankAlgorithmFactory (algorithm creation)
└── Player (game coordination)
```

### Data Models
```cpp
// Input Model
SatelliteView& map + parameters → internal parsing

// Internal Models (existing)
GameBoard, std::vector<Tank>, std::vector<Shell>

// Output Model  
GameResult {
  int winner;
  Reason reason;
  std::vector<size_t> remaining_tanks;
  size_t rounds;
}
```

### Key Interfaces
- **AbstractGameManager**: Primary interface compliance
- **SatelliteView**: Input abstraction layer
- **TankAlgorithmFactory**: Function type for algorithm creation
- **Player**: Game coordination interface

### Infrastructure Requirements
- Maintain existing collision detection system
- Preserve game loop timing and mechanics
- Keep visualization hooks functional
- Ensure thread-safety for future multi-threading

## Development Roadmap

### Phase 1: Interface Foundation (MVP)
**Scope**: Basic AbstractGameManager compliance with minimal functionality
- Add AbstractGameManager inheritance to GameManager
- Implement skeleton `run()` method that calls existing logic
- Create `readSatelliteView()` method stub
- Maintain all existing functionality through old interface
- **Deliverable**: GameManager compiles with new interface, old functionality intact

### Phase 2: SatelliteView Integration
**Scope**: Replace file loading with SatelliteView parsing
- Implement SatelliteView → board lines conversion
- Adapt existing GameBoard::initialize() to work with parsed data
- Extract tank positions from SatelliteView characters
- Remove dependency on FileLoader in GameManager
- **Deliverable**: GameManager can initialize from SatelliteView input

### Phase 3: Factory Integration
**Scope**: Replace internal factories with parameter-based factories
- Remove member factory variables from GameManager
- Implement factory-based algorithm creation
- Handle player indexing correctly (player 1 & 2 only)
- Map tank instances to correct algorithm factories
- **Deliverable**: Algorithms created via provided factories

### Phase 4: Result Standardization
**Scope**: Convert output generation to GameResult return
- Modify checkGameOver() to populate GameResult struct
- Add winner determination logic
- Count remaining tanks per player
- Calculate game end reasons correctly
- **Deliverable**: Structured result return instead of file output

### Phase 5: Interface Cleanup
**Scope**: Remove legacy methods and finalize interface
- Remove old readBoard() and run() methods
- Update all internal method signatures
- Clean up unused member variables
- Add comprehensive error handling
- **Deliverable**: Clean, single-interface GameManager

### Phase 6: Validation and Testing
**Scope**: Ensure compliance and functionality
- Test with various SatelliteView implementations
- Validate GameResult accuracy
- Verify algorithm factory integration
- Performance testing for simulation use
- **Deliverable**: Production-ready refactored GameManager

## Logical Dependency Chain

### Foundation First (Phase 1)
- AbstractGameManager interface compliance is prerequisite for all other work
- Must maintain existing functionality to avoid breaking current tests
- Skeleton implementation allows parallel development of other components

### Input Processing (Phase 2)
- SatelliteView parsing is independent of factory changes
- Can reuse existing GameBoard::initialize() logic
- Must complete before factory integration to have valid game state

### Algorithm Creation (Phase 3)
- Depends on Phase 2 for valid tank creation
- Factory integration requires understanding of player/tank mapping
- Critical for multi-algorithm simulation capability

### Output Standardization (Phase 4)
- Can be developed in parallel with Phase 3
- Requires understanding of all possible game end conditions
- Foundation for Simulator result aggregation

### Integration Testing (Phases 5-6)
- Depends on completion of all core functionality
- Validates end-to-end compatibility
- Ensures production readiness

## Risks and Mitigations

### Technical Challenges

**Risk**: SatelliteView parsing introduces bugs in game state creation
**Mitigation**: Reuse existing GameBoard::initialize() logic, add comprehensive unit tests for parsing

**Risk**: Factory integration breaks algorithm instantiation
**Mitigation**: Implement gradual replacement, maintain fallback to existing factories during development

**Risk**: GameResult conversion loses important game state information
**Mitigation**: Analyze all current output formats, ensure GameResult captures equivalent data

### MVP Definition

**Risk**: Scope creep leading to over-engineering
**Mitigation**: Focus on minimal interface compliance first, iterate functionality in subsequent phases

**Core MVP**: GameManager implements AbstractGameManager interface and can run a basic 2-player game from SatelliteView input

### Resource Constraints

**Risk**: Complex refactoring destabilizes existing functionality
**Mitigation**: Maintain backward compatibility until final phase, comprehensive regression testing

**Risk**: Interface requirements conflict with existing architecture
**Mitigation**: Phased approach allows architecture adaptation, fallback to wrapper patterns if needed

## Appendix

### Current GameManager Responsibilities
- File I/O via FileLoader
- Game object creation (tanks, shells, board)
- Player and algorithm management
- Game loop execution
- Collision detection
- Output file generation
- Visualization integration

### Post-Refactor Responsibilities
- SatelliteView parsing to game state
- Game loop execution (unchanged)
- Collision detection (unchanged)
- Algorithm creation via factories
- GameResult generation
- Visualization integration (unchanged)

### Interface Compliance Requirements
```cpp
virtual GameResult run(
    size_t map_width, size_t map_height,
    SatelliteView& map,
    size_t max_steps, size_t num_shells,
    Player& player1, Player& player2,
    TankAlgorithmFactory player1_tank_algo_factory,
    TankAlgorithmFactory player2_tank_algo_factory) = 0;
```

### Key Success Metrics
- All existing game mechanics preserved
- Interface compliance verified
- Performance maintained for simulation use
- Clean separation of concerns achieved
- Backward compatibility during transition