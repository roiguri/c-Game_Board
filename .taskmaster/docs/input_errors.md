# Input Validation Migration Plan: GameManager → Simulator

## Overview
Move input validation and error handling from GameManager to Simulator by enhancing FileSatelliteView with built-in validation and recovery capabilities.

## Step-by-Step Implementation

### Step 1: Add Validation Infrastructure to FileSatelliteView
**Objective**: Add validation members and methods without changing behavior

**Changes**:
- Add validation members to `FileSatelliteView` class
- Add getter methods for validation results
- No change to constructor or `getObjectAt()` logic yet

**Files Modified**:
- `Simulator/utils/file_satellite_view.h`
- `Simulator/utils/file_satellite_view.cpp`

**Implementation**:
```cpp
// In file_satellite_view.h - add to private section:
bool m_isValid = true;
std::string m_errorReason;
std::vector<std::string> m_warnings;

// Add to public section:
bool isValid() const { return m_isValid; }
const std::string& getErrorReason() const { return m_errorReason; }
const std::vector<std::string>& getWarnings() const { return m_warnings; }

// Add to private section:
void setError(const std::string& error);
void addWarning(const std::string& warning);
```

**Success Metrics**:
- [ ] All existing tests pass
- [ ] Code compiles without errors
- [ ] `FileSatelliteView` maintains existing behavior
- [ ] New validation methods return expected default values

**Testing**:
```bash
cd build
make && ./test/file_satellite_view_test
```

---

### Step 2: Implement Board Processing Logic
**Objective**: Add validation and recovery logic to constructor

**Changes**:
- Replace `m_boardData` with `m_processedBoard`
- Add `initializeAndRecoverBoard()` method
- Implement validation logic with early exit on errors
- Implement recovery for warnings (padding, character replacement)

**Files Modified**:
- `Simulator/utils/file_satellite_view.cpp`

**Implementation**:
```cpp
// Replace m_boardData with m_processedBoard in header
std::vector<std::string> m_processedBoard;

// In constructor:
FileSatelliteView::FileSatelliteView(const std::vector<std::string>& boardData, size_t rows, size_t cols)
    : m_rows(rows), m_cols(cols) {
    initializeAndRecoverBoard(boardData);
}

// Implement full validation and recovery logic
void FileSatelliteView::initializeAndRecoverBoard(const std::vector<std::string>& rawBoardData);
```

**Success Metrics**:
- [ ] All existing FileSatelliteView tests pass
- [ ] Invalid boards (no tanks) set `m_isValid = false`
- [ ] Valid boards with recoverable issues collect warnings
- [ ] Board recovery works (padding short rows, replacing invalid chars)
- [ ] `getObjectAt()` returns corrected characters

**Testing**:
```bash
# Test with various board configurations
./test/file_satellite_view_test
# Should show proper validation and recovery behavior
```

---

### Step 3: Update FileLoader::BoardInfo Interface
**Objective**: Expose validation results through FileLoader interface

**Changes**:
- Add validation methods to `BoardInfo` struct
- Methods delegate to underlying `FileSatelliteView`
- No change to loading logic yet

**Files Modified**:
- `Simulator/utils/file_loader.h`

**Implementation**:
```cpp
// Add to BoardInfo struct:
bool isValid() const { 
    return satelliteView && 
           static_cast<FileSatelliteView*>(satelliteView.get())->isValid(); 
}
std::string getErrorReason() const {
    return satelliteView ? 
           static_cast<FileSatelliteView*>(satelliteView.get())->getErrorReason() : 
           "Failed to load board";
}
std::vector<std::string> getWarnings() const {
    return satelliteView ? 
           static_cast<FileSatelliteView*>(satelliteView.get())->getWarnings() : 
           std::vector<std::string>{};
}
```

**Success Metrics**:
- [ ] All existing FileLoader tests pass
- [ ] `BoardInfo` exposes validation results correctly
- [ ] Invalid boards return `isValid() == false`
- [ ] Warning collection works through `BoardInfo` interface

**Testing**:
```bash
./test/file_loader_test
# Verify BoardInfo validation methods work correctly
```

---

### Step 4: Add Validation Tests
**Objective**: Comprehensive test coverage for validation logic

**Changes**:
- Add tests for validation scenarios
- Test error conditions (no tanks, empty boards)
- Test warning conditions (extra rows/cols, invalid chars)
- Test recovery behavior

**Files Modified**:
- `Simulator/utils/file_satellite_view_test.cpp`
- `Simulator/utils/file_loader_test.cpp`

**Implementation**:
```cpp
// Add test cases for:
TEST_F(FileSatelliteViewTest, ValidationWithNoTanks);
TEST_F(FileSatelliteViewTest, ValidationWithWarnings);
TEST_F(FileSatelliteViewTest, BoardRecovery);
TEST_F(FileLoaderTest, BoardInfoValidation);
```

**Success Metrics**:
- [ ] All validation test cases pass
- [ ] Error scenarios properly detected
- [ ] Warning scenarios properly collected
- [ ] Recovery behavior verified
- [ ] Edge cases covered

**Testing**:
```bash
./test/file_satellite_view_test
./test/file_loader_test
# All new validation tests should pass
```

---

### Step 5: Update Simulator to Use Validation
**Objective**: Simulator handles validation results and creates error files

**Changes**:
- Check `BoardInfo::isValid()` before proceeding
- Handle warnings by creating error files
- Silent exit on unrecoverable errors
- Move `saveErrorsToFile()` from GameManager to Simulator

**Files Modified**:
- Simulator main execution files
- Move error file creation logic

**Implementation**:
```cpp
// In simulator execution:
auto boardInfo = FileLoader::loadBoardWithSatelliteView(mapFile);

if (!boardInfo.isValid()) {
    // Silent exit on unrecoverable error
    return false;
}

// Handle warnings by creating error file
if (!boardInfo.getWarnings().empty()) {
    saveErrorsToFile(boardInfo.getWarnings());
}

// Continue with game execution
```

**Success Metrics**:
- [ ] Simulator properly validates boards before GameManager
- [ ] Invalid boards cause silent exit
- [ ] Warning files created for recoverable issues
- [ ] GameManager receives only valid boards

**Testing**:
```bash
# Test with invalid board file
./simulator invalid_board.txt gamemanager.so algo1.so algo2.so
# Should exit silently without error file

# Test with board containing warnings
./simulator warning_board.txt gamemanager.so algo1.so algo2.so
# Should create input_errors.txt and continue
```

---

### Step 6: Remove Validation from GameManager
**Objective**: Clean up GameManager by removing validation logic

**Changes**:
- Remove error collection from `GameBoard::initialize()`
- Remove `saveErrorsToFile()` method from GameManager
- Simplify `readSatelliteView()` to assume valid input
- Remove error-related member variables

**Files Modified**:
- `GameManager/game_manager.h`
- `GameManager/game_manager.cpp`
- `UserCommon/game_board.h`
- `UserCommon/game_board.cpp`

**Implementation**:
```cpp
// Simplify GameBoard::initialize() signature
bool initialize(const std::vector<std::string>& boardLines, 
                std::vector<std::pair<int, Point>>& tankPositions);
// Remove errors parameter

// Remove from GameManager:
bool saveErrorsToFile(const std::vector<std::string>& errors) const;
```

**Success Metrics**:
- [ ] All GameManager tests pass
- [ ] GameBoard no longer collects errors
- [ ] GameManager assumes valid input
- [ ] No functionality regression in game execution

**Testing**:
```bash
./test/game_manager_test
./test/game_board_test
# All tests should pass with simplified validation
```

---

### Step 7: Integration Testing and Validation
**Objective**: Ensure complete system works correctly

**Changes**:
- End-to-end testing with various board files
- Verify error file creation behavior
- Verify game execution continues normally
- Performance testing to ensure no regression

**Testing Scenarios**:
1. Valid boards → normal execution
2. Invalid boards → silent exit, no error file
3. Boards with warnings → error file created, game continues
4. Edge cases → proper handling

**Success Metrics**:
- [ ] All integration tests pass
- [ ] Error file behavior matches original GameManager
- [ ] Game execution performance unchanged
- [ ] No memory leaks or resource issues

**Testing**:
```bash
# Comprehensive test suite
./test/integration_tests

# Performance comparison
time ./simulator_old vs time ./simulator_new
# Should show similar performance
```

---

## Rollback Plan

If any step fails:

1. **Steps 1-4**: Simply revert changes, no functionality affected
2. **Step 5**: Revert simulator changes, keep existing GameManager validation
3. **Step 6**: If issues arise, temporarily restore GameManager validation
4. **Step 7**: Address specific issues without rolling back entire migration

## Final Validation Checklist

- [ ] Invalid boards cause silent exit (no error file)
- [ ] Boards with warnings create error files and continue
- [ ] Valid boards execute normally
- [ ] All existing functionality preserved
- [ ] Performance maintained
- [ ] Memory usage unchanged
- [ ] Thread safety maintained