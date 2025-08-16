# Input Error Handling Test Scenarios

This document outlines comprehensive test scenarios for validating the input error handling functionality in both Comparative and Competitive modes.

## Overview

The input error handling system distinguishes between:
- **Recoverable errors**: Logged to `input_errors.txt` but execution continues if minimum requirements are met
- **Fatal errors**: Cause usage message display and graceful exit when minimum requirements are not met

## Comparative Mode Test Scenarios

**Requirements**: 2+ GameManagers, 2 algorithms, 1 map

### Scenario 1: Recoverable GameManager Errors
**Setup**: 
- 3 GameManager .so files (1 corrupted, 2 valid)
- 2 valid Algorithm .so files
- 1 valid map file

**Expected Behavior**:
- ✅ Execution succeeds
- ✅ `input_errors.txt` created with GameManager error
- ✅ Comparative results generated using 2 working GameManagers

**Test Command**:
```bash
./bin/tanks_game -comparative \
    game_map=../test_libraries/common/game_maps/input_a.txt \
    game_managers_folder=../test_libraries/common/invalid/enough_game_managers \
    algorithm1=../test_libraries/common/algorithms/libTestAlgorithm_Basic_098765432_123456789.so \
    algorithm2=../test_libraries/common/algorithms/libTestAlgorithm_Offensive_098765432_123456789.so
```

**Test Files Used**:
- `test_libraries/common/invalid/enough_game_managers/invalid_game_manager.so` (corrupted)
- `test_libraries/common/invalid/enough_game_managers/libTestGameManager_Predictable_098765432_123456789.so` (valid)
- `test_libraries/common/invalid/enough_game_managers/libTestGameManager_Real_098765432_123456789.so` (valid)
- `test_libraries/common/algorithms/libTestAlgorithm_Basic_098765432_123456789.so` (valid)
- `test_libraries/common/algorithms/libTestAlgorithm_Offensive_098765432_123456789.so` (valid)
- `test_libraries/common/game_maps/input_a.txt` (valid)

### Scenario 2: Fatal GameManager Errors
**Setup**:
- 2 GameManager .so files (1 corrupted, 1 valid)
- 2 valid Algorithm .so files  
- 1 valid map file

**Expected Behavior**:
- ❌ Execution fails with usage message
- ✅ `input_errors.txt` created with GameManager error
- ❌ No comparative results generated

### Scenario 3: Fatal Algorithm Errors
**Setup**:
- 3 valid GameManager .so files
- 2 Algorithm .so files (1 corrupted, 1 valid)
- 1 valid map file

**Expected Behavior**:
- ❌ Execution fails with usage message
- ❌ No comparative results generated

### Scenario 4: Map Warnings (Recoverable)
**Setup**:
- 2 valid GameManager .so files
- 2 valid Algorithm .so files
- 1 map file with warnings (e.g., extra characters, formatting issues)

**Expected Behavior**:
- ✅ Execution succeeds
- ✅ `input_errors.txt` created with map warnings
- ✅ Comparative results generated

### Scenario 5: Fatal Map Errors
**Setup**:
- 2 valid GameManager .so files
- 2 valid Algorithm .so files
- 1 invalid map file (e.g., missing tanks, corrupted format)

**Expected Behavior**:
- ❌ Execution fails with error message
- ❌ No comparative results generated

## Competitive Mode Test Scenarios

**Requirements**: 1 GameManager, 2+ algorithms, 1+ maps

### Scenario 6: Recoverable Algorithm Errors
**Setup**:
- 1 valid GameManager .so file
- 4 Algorithm .so files (1 corrupted, 3 valid)
- 2 valid map files

**Expected Behavior**:
- ✅ Execution succeeds
- ✅ `input_errors.txt` created with algorithm error
- ✅ Competition results generated using 3 working algorithms

### Scenario 7: Fatal Algorithm Errors  
**Setup**:
- 1 valid GameManager .so file
- 2 Algorithm .so files (1 corrupted, 1 valid)
- 2 valid map files

**Expected Behavior**:
- ❌ Execution fails with usage message
- ✅ `input_errors.txt` created with algorithm error
- ❌ No competition results generated

### Scenario 8: Fatal GameManager Error
**Setup**:
- 1 corrupted GameManager .so file
- 3 valid Algorithm .so files
- 2 valid map files

**Expected Behavior**:
- ❌ Execution fails with usage message
- ❌ No competition results generated

### Scenario 9: Recoverable Map Errors
**Setup**:
- 1 valid GameManager .so file
- 3 valid Algorithm .so files
- 3 map files (1 corrupted, 2 valid)

**Expected Behavior**:
- ✅ Execution succeeds  
- ✅ `input_errors.txt` created with map error
- ✅ Competition results generated using 2 valid maps

### Scenario 10: Fatal Map Errors
**Setup**:
- 1 valid GameManager .so file
- 3 valid Algorithm .so files
- 2 map files (both corrupted)

**Expected Behavior**:
- ❌ Execution fails with usage message
- ✅ `input_errors.txt` created with map errors
- ❌ No competition results generated

## Edge Case Scenarios

### Scenario 11: Mixed Error Types
**Setup**: 
- 3 GameManager .so files (1 corrupted, 2 valid)
- 3 Algorithm .so files (1 corrupted, 2 valid) 
- 3 map files (1 with warnings, 2 valid)

**Mode**: Comparative
**Expected Behavior**:
- ✅ Execution succeeds
- ✅ `input_errors.txt` contains GameManager error, algorithm error, and map warnings
- ✅ Results generated with 2 GameManagers, 2 algorithms, 1 map

### Scenario 12: Empty Folders
**Setup**:
- Empty game_managers_folder
- Empty algorithms_folder  
- Empty game_maps_folder

**Expected Behavior**:
- ❌ Both modes fail with usage messages
- ❌ No results generated

### Scenario 13: Permission Errors
**Setup**:
- .so files with no read permissions
- Map files with no read permissions

**Expected Behavior**:
- Errors collected and logged
- Usage messages displayed if minimum requirements not met

## Test Data Creation Guide

### Creating Invalid .so Files

1. **Corrupted ELF Header**:
   ```bash
   echo "INVALID_ELF_HEADER" > GameManagerCorrupted.so
   ```

2. **Missing Symbols**:
   Create a .so file without required registration functions

3. **Permission Issues**:
   ```bash
   chmod 000 GameManagerNoPermission.so
   ```

### Creating Invalid Map Files

1. **Missing Tanks**:
   ```
   ####
   #  #  
   #  #
   ####
   ```

2. **Invalid Characters**:
   ```
   ####
   #1X#
   #2 #  
   ####
   ```

3. **Malformed Structure**:
   ```
   ###
   #1
   #2#
   ####
   ```

## Validation Criteria

For each test scenario, verify:

1. **Error Collection**: Check `input_errors.txt` contains expected error messages
2. **Usage Messages**: Verify appropriate usage displayed for fatal errors
3. **Execution Status**: Confirm success/failure matches expectations
4. **Output Generation**: Check results files created only on success
5. **Error Formatting**: Verify error messages follow standardized format:
   - `[Error] GameManager 'filename': description`
   - `[Error] Algorithm 'filename': description` 
   - `[Warning] Map 'name': description`

## Command Line Examples

### Comparative Mode Tests

```bash
# Scenario 1 - Recoverable GameManager Errors (should succeed with input_errors.txt)
./bin/tanks_game -comparative \
    game_map=../test_libraries/common/game_maps/input_a.txt \
    game_managers_folder=../test_libraries/common/invalid/enough_game_managers \
    algorithm1=../test_libraries/common/algorithms/libTestAlgorithm_Basic_098765432_123456789.so \
    algorithm2=../test_libraries/common/algorithms/libTestAlgorithm_Offensive_098765432_123456789.so

# Scenario 2 - Fatal GameManager Errors (should fail with usage)  
./bin/tanks_game -comparative \
    game_map=../test_libraries/common/game_maps/input_a.txt \
    game_managers_folder=../test_libraries/common/invalid/not_enough_game_managers \
    algorithm1=../test_libraries/common/algorithms/libTestAlgorithm_Basic_098765432_123456789.so \
    algorithm2=../test_libraries/common/algorithms/libTestAlgorithm_Offensive_098765432_123456789.so

# Scenario 3 - Fatal Algorithm Errors (should fail with usage)
./bin/tanks_game -comparative \
    game_map=../test_libraries/common/game_maps/input_a.txt \
    game_managers_folder=../test_libraries/common/game_managers \
    algorithm1=../test_libraries/common/invalid/invalid_algorithm.so \
    algorithm2=../test_libraries/common/algorithms/libTestAlgorithm_Offensive_098765432_123456789.so

# Scenario 4 - Map Warnings (should succeed with warnings in input_errors.txt)
./bin/tanks_game -comparative \
    game_map=../test_libraries/common/invalid/enough_maps/recoverable.txt \
    game_managers_folder=../test_libraries/common/game_managers \
    algorithm1=../test_libraries/common/algorithms/libTestAlgorithm_Basic_098765432_123456789.so \
    algorithm2=../test_libraries/common/algorithms/libTestAlgorithm_Offensive_098765432_123456789.so

# Scenario 5 - Valid Execution (baseline test)
./bin/tanks_game -comparative \
    game_map=../test_libraries/common/game_maps/input_a.txt \
    game_managers_folder=../test_libraries/common/game_managers \
    algorithm1=../test_libraries/common/algorithms/libTestAlgorithm_Basic_098765432_123456789.so \
    algorithm2=../test_libraries/common/algorithms/libTestAlgorithm_Offensive_098765432_123456789.so

# Scenario 6 - Map Error - invalid map
./bin/tanks_game -comparative \
    game_map=../test_libraries/common/invalid/not_enough_maps/invalid_map.txt \
    game_managers_folder=../test_libraries/common/game_managers \
    algorithm1=../test_libraries/common/algorithms/libTestAlgorithm_Basic_098765432_123456789.so \
    algorithm2=../test_libraries/common/algorithms/libTestAlgorithm_Offensive_098765432_123456789.so
```

### Competitive Mode Tests

```bash
# Scenario 6 - Recoverable Algorithm Errors (should succeed with input_errors.txt)
./bin/tanks_game -competition \
    game_maps_folder=../test_libraries/common/game_maps \
    game_manager=../test_libraries/common/game_managers/libTestGameManager_Real_098765432_123456789.so \
    algorithms_folder=../test_libraries/common/invalid/enough_algorithms

# Scenario 7 - Fatal Algorithm Errors (should fail with usage)
./bin/tanks_game -competition \
    game_maps_folder=../test_libraries/common/game_maps \
    game_manager=../test_libraries/common/game_managers/libTestGameManager_Real_098765432_123456789.so \
    algorithms_folder=../test_libraries/common/invalid/not_enough_algorithms

# Scenario 8 - Fatal GameManager Error (should fail with usage)
./bin/tanks_game -competition \
    game_maps_folder=../test_libraries/common/game_maps \
    game_manager=../test_libraries/common/invalid/invalid_game_manager.so \
    algorithms_folder=../test_libraries/common/algorithms

# Scenario 9 - Recoverable Map Errors (should succeed with input_errors.txt)
./bin/tanks_game -competition \
    game_maps_folder=../test_libraries/common/invalid/enough_maps \
    game_manager=../test_libraries/common/game_managers/libTestGameManager_Real_098765432_123456789.so \
    algorithms_folder=../test_libraries/common/algorithms

# Scenario 10 - Fatal Map Errors (should fail with usage)
./bin/tanks_game -competition \
    game_maps_folder=../test_libraries/common/invalid/not_enough_maps \
    game_manager=../test_libraries/common/game_managers/libTestGameManager_Real_098765432_123456789.so \
    algorithms_folder=../test_libraries/common/algorithms

# Scenario 11 - Valid Execution (baseline test)
./bin/tanks_game -competition \
    game_maps_folder=../test_libraries/common/game_maps \
    game_manager=../test_libraries/common/game_managers/libTestGameManager_Real_098765432_123456789.so \
    algorithms_folder=../test_libraries/common/algorithms
```

## Implementation Notes

- All test scenarios should be automated using shell scripts or test framework
- Error messages should be validated for exact content and formatting
- File cleanup should be performed between test runs
- Tests should verify both positive and negative cases
- Performance impact of error collection should be minimal