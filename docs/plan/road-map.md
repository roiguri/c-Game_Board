# Tank Battle Assignment 3 Implementation Plan

Looking at the new assignment 3 instructions, I can see this is a significant expansion that adds multithreading and dynamic library loading. Let me break down a comprehensive implementation plan:

## Phase 1: Project Restructuring (Foundation)
**Goal: Set up the new project structure and build system**

1. **Reorganize Directory Structure**
   [x] Create 5 main folders: `Simulator/`, `Algorithm/`, `GameManager/`, `Common/`, `UserCommon/`
   [x] Move existing code to appropriate folders
   [x] Update include paths throughout codebase
   [ ] Move game board and datellite view to UserCommon

2. **Update Build System**
   [ ] Create separate makefiles for each project (Simulator, Algorithm, GameManager)
   [ ] Configure shared library (.so) builds for Algorithm and GameManager
   [ ] Set up proper linking and dependencies
   [ ] Ensure unique naming with submitter IDs

3. **Namespace Implementation**
   [ ] Wrap Algorithm code in `Algorithm_<submitter_ids>` namespace
   [ ] Wrap GameManager code in `GameManager_<submitter_ids>` namespace
   [ ] Update all references and includes

## Phase 2: API Compliance (Interface Layer)
**Goal: Make existing code comply with the new API requirements**

4. **Abstract Interface Implementation**
   [ ] Implement `AbstractGameManager` interface in existing `GameManager`
   [ ] Adapt existing `GameManager::run()` method to match new signature
   [ ] Ensure `GameResult` struct matches specification exactly

5. **Factory Pattern Conversion**
   [ ] Convert existing factory classes to use `std::function` signatures
   [ ] Update `PlayerFactory` and `TankAlgorithmFactory` to match new API
   [ ] Test factory functionality with existing code

## Phase 3: Registration System (Auto-Discovery)
**Goal: Implement the automatic registration mechanism**

6. **Registration Infrastructure**
   [ ] Implement registration classes (.cpp files in Simulator project)
   [ ] Create registration mechanism (likely singleton-based)
   [ ] Implement the registration macros functionality

7. **Registration Integration**
   [ ] Add `REGISTER_*` macro calls to existing classes
   [ ] Test registration system with static linking first
   [ ] Verify factories are properly registered and discoverable

## Phase 4: Dynamic Loading (Core Simulator Logic)
**Goal: Implement .so file loading and management**

8. **Dynamic Library Manager**
   [ ] Implement .so file loading using `dlopen/dlsym/dlclose`
   [ ] Create error handling for failed loads
   [ ] Implement discovery of .so files in directories
   [ ] Add proper cleanup and unloading

9. **Command Line Interface**
   [ ] Implement argument parsing for both comparative and competitive modes
   [ ] Add input validation and error reporting
   [ ] Implement help/usage messages

## Phase 5: Game Execution Logic (Business Logic)
**Goal: Implement the core simulation algorithms**

10. **Comparative Mode Implementation**
    [ ] Implement logic to run all GameManagers with same map/algorithms
    [ ] Implement result comparison and grouping by identical outcomes
    [ ] Create output format exactly as specified

11. **Competitive Mode Implementation**
    [ ] Implement tournament pairing algorithm (complex rotation logic)
    [ ] Handle edge cases (odd number of algorithms, duplicate pairings)
    [ ] Implement scoring system (3 points win, 1 point tie, 0 points loss)

## Phase 6: Threading System (Performance Layer)
**Goal: Add multithreading support**

12. **Threading Infrastructure**
    [ ] Design thread pool or task-based threading system
    [ ] Implement thread-safe result collection
    [ ] Handle thread synchronization and cleanup

13. **Threading Integration**
    [ ] Integrate threading with comparative mode
    [ ] Integrate threading with competitive mode
    [ ] Optimize for requested number of threads
    [ ] Handle cases where fewer threads are beneficial

## Phase 7: File I/O and Results (Output Layer)
**Goal: Implement all file operations and result generation**

14. **Output File Generation**
    [ ] Implement timestamped file naming
    [ ] Implement comparative results formatting
    [ ] Implement competitive results formatting and sorting
    [ ] Handle file creation errors with fallback to console

15. **Integration Testing**
    [ ] Test entire pipeline with sample .so files
    [ ] Test error scenarios (missing files, bad .so files, etc.)
    [ ] Verify output formats match specification exactly

## Phase 8: Polish and Validation (Quality Assurance)
**Goal: Ensure robust, production-ready code**

16. **Error Handling**
    [ ] Comprehensive error handling for all failure modes
    [ ] Proper cleanup in error scenarios
    [ ] User-friendly error messages

17. **Performance Optimization**
    [ ] Optimize threading performance
    [ ] Minimize .so loading overhead
    [ ] Memory management verification

18. **Final Testing**
    [ ] Test with multiple algorithm implementations
    [ ] Test with different GameManager implementations
    [ ] Stress test with large numbers of games/threads

## Key Implementation Priorities:

1. **Start with Phase 1-2** to get basic structure working
2. **Phase 3-4 are critical** - the registration and dynamic loading are the heart of the system
3. **Phase 5 contains the most complex logic** - especially the competitive mode pairing algorithm
4. **Phase 6 adds significant complexity** - consider implementing single-threaded versions first
5. **Phases 7-8 are refinement** - focus on correctness and robustness

## Risk Areas to Watch:
- **Dynamic library loading** - platform-specific code, error-prone
- **Threading synchronization** - potential for deadlocks/race conditions  
- **Competitive mode pairing logic** - mathematically complex
- **Result comparison in comparative mode** - exact matching requirements
- **Memory management** - especially with dynamic loading/unloading

## Next Steps:
Would you like me to elaborate on any of these phases or discuss specific implementation challenges?

## Questions:
- Folder uppercase/lowercase
- Should GameManger create any output file?