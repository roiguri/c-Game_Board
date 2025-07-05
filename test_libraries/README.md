# Test Libraries for Game Mode Testing

This directory contains real algorithm and GameManager implementations that compile to actual .so files for comprehensive game mode testing.

## Overview

Instead of using mock .so files with just ELF headers, these are real implementations that can be loaded and executed by the game modes, providing much more realistic testing scenarios.

## Algorithms

### 1. TestAlgorithm_DoNothing_098765432_123456789.so
- **Behavior**: Tanks always request battle info and do nothing else
- **Use Case**: Testing idle/passive scenarios, baseline performance
- **Expected Result**: Games should timeout or reach max steps

### 2. TestAlgorithm_CircleMovement_098765432_123456789.so  
- **Behavior**: Tanks move in circular patterns (3 steps forward, turn right, repeat)
- **Use Case**: Testing predictable movement patterns, collision detection
- **Expected Result**: Tanks move in squares around the map

### 3. TestAlgorithm_Shooter_098765432_123456789.so
- **Behavior**: Tanks shoot immediately on first turn, then idle
- **Use Case**: Testing shell mechanics, immediate action scenarios
- **Expected Result**: Quick games with early shell firing

## GameManagers

### 1. TestGameManager_Predictable_098765432_123456789.so
- **Behavior**: Produces predictable results based on game count
  - Game 1, 4, 7...: Player 1 wins
  - Game 2, 5, 8...: Player 2 wins  
  - Game 3, 6, 9...: Tie (max steps reached)
- **Use Case**: Testing result consistency, multiple game scenarios
- **Rounds**: Varies between 5-14 rounds per game

### 2. TestGameManager_AlwaysTie_098765432_123456789.so
- **Behavior**: Always results in a tie due to max steps reached
- **Use Case**: Testing tie scenarios, timeout handling
- **Expected Result**: Always winner=0, reason=MAX_STEPS_REACHED

## Building

### Prerequisites
1. Main project must be built first to create UserCommon library:
   ```bash
   cd .. && mkdir -p build && cd build && cmake .. && make UserCommon
   ```

### Build Test Libraries
```bash
cd test_libraries
./build_test_libraries.sh
```

Or manually:
```bash
cd test_libraries
mkdir build && cd build
cmake ..
make all_test_libraries
```

### Output
Libraries are created in:
- `test_libraries/build/algorithms/` - Algorithm .so files
- `test_libraries/build/game_managers/` - GameManager .so files

## Integration with Tests

Update the test helpers to use these real libraries instead of mock ones:

```cpp
// In test helpers
std::string realAlgorithmPath = "test_libraries/build/algorithms/TestAlgorithm_DoNothing_098765432_123456789.so";
std::string realGameManagerPath = "test_libraries/build/game_managers/TestGameManager_Predictable_098765432_123456789.so";

// These will actually load and execute real game logic
```

## Namespace Organization

Each library is properly namespaced:
- **Algorithms**: `TestAlgorithm_[Type]_098765432_123456789`
- **GameManagers**: `TestGameManager_[Type]_098765432_123456789`

This follows the project's namespace conventions and ensures no conflicts with production code.

## Testing Scenarios

### Basic Game Mode
- Test with different algorithm combinations
- Verify library loading and symbol resolution
- Test error handling with real but failing libraries

### Comparative Runner
- Compare different GameManager implementations
- Test performance measurement with real execution
- Verify result grouping with actual game outcomes

### Competitive Runner  
- Run tournaments with predictable algorithms
- Test scoring systems with known algorithm behaviors
- Verify leaderboard generation with real results

## Advantages Over Mock Libraries

1. **Real Execution**: Actually runs game logic, not just interface calls
2. **Symbol Loading**: Tests real dynamic library loading
3. **Performance Testing**: Measures actual execution time
4. **Error Scenarios**: Can test real failure modes
5. **Result Validation**: Tests with actual GameResult objects
6. **Integration Testing**: Full end-to-end validation

## Debugging

Libraries include minimal logging for debugging:
- PredictableGameManager: Logs game start/end and results
- TieGameManager: Logs tie outcomes
- Algorithms: No logging to avoid spam

Set verbose=true in GameManager constructors to enable logging output.