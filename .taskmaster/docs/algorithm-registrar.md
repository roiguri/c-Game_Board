# Registration Infrastructure Implementation Plan

## Overview
This plan focuses solely on implementing the registration infrastructure to enable the existing Algorithm and Player implementations to work with the dynamic loading system. The existing BasicPlayer, OffensivePlayer, and BasicTankAlgorithm classes are already implemented and need registration support.

## Phase 1: Registration Infrastructure Setup

### Step 1: Registration Headers (No Changes Required)
**Current state:**
- `common/PlayerRegistration.h` - already exists and correct
- `common/TankAlgorithmRegistration.h` - already exists, header is complete
- Headers must be used as-is per assignment requirements
- Only .cpp implementations need to be created

### Step 2: AlgorithmRegistrar Core
**Files to create:**
- `simulator/AlgorithmRegistrar.h`
- `simulator/AlgorithmRegistrar.cpp`

**Key components:**
```cpp
class AlgorithmRegistrar {
    class AlgorithmAndPlayerFactories {
        std::string so_name;
        TankAlgorithmFactory tankAlgorithmFactory;
        PlayerFactory playerFactory;
        // methods for setting/getting factories
    };
    
    std::vector<AlgorithmAndPlayerFactories> algorithms;
    static AlgorithmRegistrar registrar; // singleton
    
public:
    static AlgorithmRegistrar& getAlgorithmRegistrar();
    void createAlgorithmFactoryEntry(const std::string& name);
    void addPlayerFactoryToLastEntry(PlayerFactory&& factory);
    void addTankAlgorithmFactoryToLastEntry(TankAlgorithmFactory&& factory);
    void validateLastRegistration();
    void removeLast();
    // iterator support for range-based loops
};
```

**Commit 1**: AlgorithmRegistrar singleton
- Implement singleton pattern with static instance
- Add factory storage and management methods
- Implement validation with BadRegistrationException
- Unit tests for registrar functionality (add/validate/remove)

### Step 3: Registration Implementation Files
**Files to create:**
- `simulator/PlayerRegistration.cpp`
- `simulator/TankAlgorithmRegistration.cpp`

**Implementation:**
```cpp
// PlayerRegistration.cpp
PlayerRegistration::PlayerRegistration(PlayerFactory factory) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    registrar.addPlayerFactoryToLastEntry(std::move(factory));
}

// TankAlgorithmRegistration.cpp
TankAlgorithmRegistration::TankAlgorithmRegistration(TankAlgorithmFactory factory) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    registrar.addTankAlgorithmFactoryToLastEntry(std::move(factory));
}
```

**Commit 2**: Registration implementation
- Connect registration structs to AlgorithmRegistrar
- Implement proper factory forwarding
- Unit tests verifying registration calls reach registrar

## Phase 2: Existing Algorithm Integration

### Step 4: Student ID Wrapper Classes
**Files to create:**
- `algorithm/TankAlgorithm_<id1>_<id2>.h`
- `algorithm/TankAlgorithm_<id1>_<id2>.cpp`
- `algorithm/Player_<id1>_<id2>.h`
- `algorithm/Player_<id1>_<id2>.cpp`

**Wrapper approach:**
Since existing implementations (BasicTankAlgorithm, BasicPlayer, OffensivePlayer) are already complete, create thin wrapper classes that:
- Inherit from existing implementations
- Are properly named with student IDs
- Live in required namespace
- Add registration macros

**Example structure:**
```cpp
// algorithm/TankAlgorithm_<id1>_<id2>.h
namespace Algorithm_<id1>_<id2> {
    class TankAlgorithm_<id1>_<id2> : public BasicTankAlgorithm {
    public:
        TankAlgorithm_<id1>_<id2>(int player_index, int tank_index);
        // Inherit all functionality from BasicTankAlgorithm
    };
}

// algorithm/Player_<id1>_<id2>.h  
namespace Algorithm_<id1>_<id2> {
    class Player_<id1>_<id2> : public BasicPlayer {
    public:
        Player_<id1>_<id2>(int player_index, size_t x, size_t y, 
                           size_t max_steps, size_t num_shells);
        // Inherit all functionality from BasicPlayer
    };
}
```

**Registration in .cpp files:**
```cpp
// Global scope in .cpp files
using namespace Algorithm_<id1>_<id2>;
REGISTER_TANK_ALGORITHM(TankAlgorithm_<id1>_<id2>);
REGISTER_PLAYER(Player_<id1>_<id2>);
```

**Commit 3**: Student ID wrapper classes
- Create wrapper classes with proper namespace isolation
- Inherit from existing BasicTankAlgorithm and BasicPlayer
- Add registration macros in .cpp files
- Unit tests for wrapper functionality

### Step 5: Alternative Algorithm Wrappers
**Optional files to create:**
- `algorithm/OffensiveTankAlgorithm_<id1>_<id2>.h/cpp` (if different tank algorithm needed)
- `algorithm/OffensivePlayer_<id1>_<id2>.h/cpp` (wrapper for OffensivePlayer)

**Implementation approach:**
- Create additional wrapper classes for OffensivePlayer if needed
- Allow registration of multiple algorithm variants
- Each variant gets its own registration

**Commit 4**: Additional algorithm variants
- Create wrappers for OffensivePlayer and other implementations
- Register multiple algorithm options
- Unit tests for all registered algorithms

## Phase 3: Registration Testing and Validation

### Step 6: Registration Integration Tests
**Test scenarios:**
- Successful registration of both Player and TankAlgorithm
- Failed registration scenarios (missing Player or TankAlgorithm)
- Factory functionality (create instances through factories)
- Multiple algorithm registration

**Test structure:**
```cpp
class RegistrationIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear registrar state
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    }
    
    void simulateRegistration(const std::string& name) {
        auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        registrar.createAlgorithmFactoryEntry(name);
        // Manually trigger registration macros
        using namespace Algorithm_<id1>_<id2>;
        REGISTER_TANK_ALGORITHM(TankAlgorithm_<id1>_<id2>);
        REGISTER_PLAYER(Player_<id1>_<id2>);
    }
};
```

**Commit 4**: Integration testing
- Comprehensive tests for registration flow
- Error handling and validation tests
- Factory creation and usage tests

### Step 7: Build System Integration
**Makefile/CMake updates:**
- Ensure proper compilation order (common → simulator → algorithm)
- Link registration implementation with algorithm project
- Handle shared library compilation flags (-fPIC)

**Commit 5**: Build system finalization
- Update build files for registration system
- Ensure clean compilation and linking
- Verify all tests pass in build system

## Key Implementation Guidelines

### Compilation Requirements
- Use C++17 standards
- Avoid smart pointers as specified
- Use `-fPIC` flag for shared library compatibility
- No use of `new`/`delete` operators

### Testing Standards
- Google Test framework for all unit tests
- Place object being tested as first argument in EXPECT_* macros
- Test file naming: `test/[class_name]_test.cpp`
- Create test fixtures when appropriate

### Code Quality Standards
- No namespaces (except for algorithm isolation)
- Parameter name hinting in function calls when necessary
- Minimal comments (only when absolutely necessary)
- Each commit must maintain working codebase

### Registration Flow Summary
1. AlgorithmRegistrar creates entry for algorithm name
2. Registration macros create factory functions
3. Registration structs forward factories to registrar
4. Validation ensures both Player and TankAlgorithm factories exist
5. Failed registrations are removed with error reporting

## Success Criteria
- Registration macros compile and execute correctly
- AlgorithmRegistrar properly stores and validates factories
- Algorithm classes can be instantiated through factories
- All unit tests pass
- Build system produces working shared libraries
- Registration system is ready for Simulator integration