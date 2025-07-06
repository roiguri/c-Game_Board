**New Files**:
```
Simulator/
├── competitive_runner.h/cpp          # Competition mode implementation
└── test/competitive_test.cpp         # Competition mode tests
```

**Updated Files**:
- `command_line_parser.cpp` - Support both modes
- `main.cpp` - Route to appropriate mode# Revised Simulator Implementation Plan - Incremental Development

## Overview
Build the simulator incrementally, starting with the most basic functionality and gradually adding complexity. This approach prioritizes getting both modes working sequentially first, then adding threading as an optimization to both modes.

## Development Philosophy
**Logic First, Performance Second**: Implement both comparative and competition modes sequentially, validate the business logic, then add threading as a performance optimization. This separates complexity concerns and makes debugging easier.

## Revised Implementation Phases

### **Phase 1: MVP - Simple Game Execution** 🎯
**Goal**: Run a single game using dynamic .so loading

**What we'll build**:
- `LibraryManager` - Load .so files and trigger registration
- `SimpleGameRunner` - Run one game using loaded GameManager and algorithms
- Basic main.cpp that loads .so files and runs a game

**Target Output**: 
```bash
./basic_simulator map.txt gamemanager.so algorithm1.so algorithm2.so
# Loads the .so files, runs one game, shows result
```

**Files to Create**:
```
Simulator/
├── library_manager.h/cpp             # Dynamic .so loading
├── simple_game_runner.h/cpp          # Single game execution
├── basic_main.cpp                    # Entry point for Phase 1
└── test/simple_game_test.cpp         # Basic tests
```

**Note**: We'll use existing Player/Algorithm implementations from loaded .so files, not create test ones.

---

### **Phase 2: Command Line Interface** 🎯
**Goal**: Add proper command line argument parsing

**What we'll add**:
- `CommandLineParser` - Parse command line arguments properly
- Enhanced `main.cpp` with argument validation
- Better error handling and usage messages

**Target Output**:
```bash
./simulator map.txt gamemanager.so algorithm1.so algorithm2.so [--verbose]
# Same functionality as Phase 1, but with proper argument parsing
```

**New Files**:
```
Simulator/
├── command_line_parser.h/cpp         # Proper argument parsing
└── test/command_line_test.cpp        # Argument parsing tests
```

**Updated Files**:
- `main.cpp` - Enhanced with argument validation

---

### **Phase 3: Add Comparative Mode** 🎯
**Goal**: Implement comparative mode (sequential first)

**What we'll add**:
- `GameRunner` - Enhanced game execution with multiple GameManagers
- `ComparativeRunner` - Sequential comparative mode execution
- Update command line parser for comparative mode arguments

**Target Output**:
```bash
./simulator -comparative game_map=map.txt game_managers_folder=./gms algorithm1=a1.so algorithm2=a2.so
# Output: comparative_results_<timestamp>.txt file
```

**New Files**:
```
Simulator/
├── game_runner.h/cpp                 # Enhanced game execution  
├── comparative_runner.h/cpp          # Sequential comparative mode
└── test/comparative_test.cpp         # Comparative mode tests
```

**Updated Files**:
- `command_line_parser.cpp` - Add comparative mode arguments
- `main.cpp` - Route to comparative mode

---

### **Phase 4: Add Threading** 🎯
**Goal**: Make comparative mode multi-threaded

**What we'll add**:
- `ThreadPool` - Basic thread management and task execution
- Update `ComparativeRunner` to use ThreadPool for parallel execution
- Add `num_threads` parameter support

**Target Output**:
```bash
./simulator -comparative ... num_threads=4
# Output: Same results as Phase 3, but executed in parallel
```

**New Files**:
```
Simulator/
├── thread_pool.h/cpp                 # Thread management
└── test/threading_test.cpp           # Threading validation
```

**Updated Files**:
- `comparative_runner.cpp` - Add parallel execution
- `command_line_parser.cpp` - Add num_threads parameter

---

### **Phase 5: Add Competition Mode** 🎯
**Goal**: Both modes working with full functionality

**What we'll add**:
- `CompetitiveRunner` - Tournament logic with threading
- Update `CommandLineParser` to support both modes
- Algorithm pairing formula implementation
- Competition scoring system (3/1/0 points)

**Target Output**:
```bash
./simulator -competition game_maps_folder=./maps game_manager=gm.so algorithms_folder=./algos num_threads=4
# Output: competition_<timestamp>.txt file with rankings
```

---

### **Phase 6: Production Polish** 🎯
**Goal**: Robust, production-ready simulator

**What we'll add**:
- `ErrorHandler` - Comprehensive error management
- `OutputGenerator` - Exact format compliance and file handling
- Final integration and performance optimization
- Comprehensive testing and validation

**Target Output**: Production-ready simulator meeting all assignment requirements

**New Files**:
```
Simulator/
├── error_handler.h/cpp               # Error management
├── output_generator.h/cpp            # Output formatting
├── utilities.h/cpp                   # Helper functions
└── test/integration_test.cpp         # End-to-end tests
```

---

## Detailed Phase 1 Implementation Plan

### **Step 1.1: Library Manager**
**Purpose**: Load .so files and trigger automatic registration

```cpp
class LibraryManager {
public:
    bool loadLibrary(const std::string& libraryPath);
    std::vector<std::string> getLoadedLibraryNames() const;
    void unloadAllLibraries();
    std::string getLastError() const;
};
```

**Implementation**:
1. Use `dlopen()` to load .so files
2. Registration happens automatically via REGISTER_* macros
3. Track loaded libraries for cleanup
4. Basic error handling with `dlerror()`

### **Step 1.2: Simple Game Runner**
**Purpose**: Execute one game using registered factories

```cpp
class SimpleGameRunner {
public:
    static GameResult runSingleGame(
        const std::string& mapFilePath, 
        const std::string& gameManagerName,
        const std::string& algorithm1Name,
        const std::string& algorithm2Name,
        bool verbose = false);
    static void printGameResult(const GameResult& result, const std::string& mapPath);
};
```

**Implementation**:
1. Use existing `FileLoader::loadBoardWithSatelliteView()`
2. Get GameManager factory from existing registrar
3. Get Algorithm factories from existing registrar  
4. Create Player instances using factories
5. Call `GameManager::run()` method
6. Display results

### **Step 1.3: Basic Main**
**Purpose**: Load .so files and run one game

```cpp
int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cout << "Usage: " << argv[0] << " <map_file> <gamemanager.so> <algo1.so> <algo2.so>" << std::endl;
        return 1;
    }
    
    LibraryManager libManager;
    
    // Load .so files (this triggers registration)
    libManager.loadLibrary(argv[2]); // GameManager
    libManager.loadLibrary(argv[3]); // Algorithm 1  
    libManager.loadLibrary(argv[4]); // Algorithm 2
    
    // Run the game using registered factories
    GameResult result = SimpleGameRunner::runSingleGame(
        argv[1], "GameManager", "Algorithm1", "Algorithm2");
    
    SimpleGameRunner::printGameResult(result, argv[1]);
    return 0;
}
```

---

## Commit Strategy

### **Commit 1: Phase 1 MVP**
- ✅ Can load .so files and trigger registration
- ✅ Can run a single game with loaded GameManager and algorithms
- ✅ Basic library management with cleanup
- ✅ Working executable

**Validation**: 
```bash
make basic_simulator
./basic_simulator test_map.txt gamemanager.so algorithm1.so algorithm2.so
# Should load .so files, run game, and output results
```

### **Commit 2: Phase 2 CLI**
- ✅ Proper command line argument parsing
- ✅ Better error handling and validation
- ✅ Clean usage messages
- ✅ Enhanced user experience

**Validation**: 
```bash
./simulator map.txt gamemanager.so algorithm1.so algorithm2.so --verbose
# Should have better argument validation and error messages
```

### **Commit 3: Phase 3 Comparative Mode**
- ✅ Comparative mode implementation (sequential)
- ✅ Load multiple GameManagers from folder
- ✅ Generate comparative results output file
- ✅ Group results by identical outcomes

**Validation**: 
```bash
./simulator -comparative game_map=map.txt game_managers_folder=./gms algorithm1=a1.so algorithm2=a2.so
# Should generate comparative_results_<time>.txt
```

### **Commit 4: Phase 4 Competition Mode** 
- ✅ Competition mode implementation (sequential)
- ✅ Algorithm pairing and scoring logic
- ✅ Tournament execution and ranking
- ✅ Both modes working sequentially

**Validation**: 
```bash
./simulator -competition game_maps_folder=./maps game_manager=gm.so algorithms_folder=./algos
# Should generate competition_<time>.txt with rankings
```

### **Commit 5: Phase 5 Threading**
- ✅ Multi-threaded execution for both modes
- ✅ Thread pool management
- ✅ Parallel game execution
- ✅ Performance improvement

**Validation**: 
```bash
./simulator -comparative ... num_threads=4
./simulator -competition ... num_threads=4
# Should run faster with multiple threads
```

### **Commit 6: Phase 6 Production Polish**
- ✅ Comprehensive error handling
- ✅ Exact format compliance
- ✅ Production-ready quality

**Validation**: Final testing and validation of all requirements

---

## Benefits of This Approach

### ✅ **Incremental Validation**
- Each phase produces working software
- Can test integration with existing components immediately
- Early detection of design issues

### ✅ **Logic Before Optimization**
- Get both modes working sequentially first
- Validate business logic without threading complexity
- Then add threading as performance optimization to both modes

### ✅ **Risk Reduction**
- Start with known working components (existing GameManager)
- Add complexity gradually
- Each commit is a stable checkpoint

### ✅ **Early Feedback**
- Validate assumptions early
- Test with real .so files as soon as Phase 1
- Both modes working by Phase 4, threading is just optimization

### ✅ **Parallel Development Enablement**
- Once Phase 1 works, can develop/test Algorithm implementations
- Can work with different teams' GameManager implementations
- Clear interfaces allow independent work

### ✅ **Better Testing Strategy**
- Test mode logic separately from threading logic
- Easier to debug issues when they're isolated
- Threading bugs vs. business logic bugs are separate

---

## Getting Started

**Next Steps**:
1. ✅ Implement `LibraryManager` for dynamic .so loading
2. ✅ Implement `SimpleGameRunner` that uses registrar factories  
3. ✅ Create basic main.cpp for Phase 1
4. ✅ Test with existing .so files (GameManager and Algorithm)
5. ✅ Validate registration system integration

**Expected Timeline**:
- Phase 1: 1 day (Basic game execution with .so loading)
- Phase 2: 1 day (Command line interface)
- Phase 3: 2 days (Comparative mode - sequential)
- Phase 4: 2 days (Competition mode - sequential)
- Phase 5: 1 day (Add threading to both modes)
- Phase 6: 1 day (Polish and testing)

**Total**: ~1 week for fully functional simulator

This approach ensures we have working software at every step and can validate our design decisions early!