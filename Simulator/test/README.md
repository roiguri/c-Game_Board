# Simulator Test Infrastructure

This directory contains testing infrastructure for the Simulator component.

## Directory Structure

### `/mocks/`
Contains mock implementations of core interfaces for testing:
- **MockGameManager.h** - Mock implementation of AbstractGameManager
- **MockPlayer.h** - Mock implementation of Player interface  
- **MockTankAlgorithm.h** - Mock implementation of TankAlgorithm interface
- **MockFactories.h** - Factory functions for creating mock objects

### `/helpers/`
Contains reusable test helper classes organized by functionality:

#### **BoardInfoTestHelpers** (`board_info_test_helpers.h`)
Utilities for creating BoardInfo objects for testing:
- `createValidBoardInfo()` - Creates standard valid BoardInfo for testing
- `createInvalidBoardInfo()` - Creates invalid BoardInfo for error testing  
- `createCustomBoardInfo(rows, cols, maxSteps, numShells)` - Creates BoardInfo with custom parameters

#### **RegistrarTestHelpers** (`registrar_test_helpers.h`)
Utilities for managing registrars in tests:
- `clearAllRegistrars()` - Cleans up all registrars between tests
- `setupMockRegistrars()` - Sets up complete mock registrar data (GameManager + 2 algorithms)
- `setupIncompleteAlgorithm()` - Creates incomplete algorithm for error testing
- `setupGameManagerRegistrar(name)` - Sets up GameManager registrar only
- `setupCompleteAlgorithm(name)` - Sets up single complete algorithm

### `/integration/`
Contains integration tests that test multiple components together:
- **RegistrationIntegration_test.cpp** - Tests registration system integration

## Usage Guidelines

### For Unit Tests
- Use mocks from `/mocks/` to isolate components under test
- Use helpers from `/helpers/` for common test setup
- Co-locate test files with their source files (e.g., `game_runner_test.cpp` next to `game_runner.cpp`)

### For Integration Tests  
- Place in `/integration/` directory
- Test interactions between multiple real components
- Use helpers for common setup but test real behavior

### Test Isolation
- Always call helper cleanup functions (`clearAllRegistrars()`) in SetUp/TearDown
- Each test should start with clean state
- Use existing patterns from other test files

## Example Usage

```cpp
#include "test/helpers/board_info_test_helpers.h"
#include "test/helpers/registrar_test_helpers.h"
#include "test/mocks/MockFactories.h"

class MyComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegistrarTestHelpers::clearAllRegistrars();
    }
    
    void TearDown() override {
        RegistrarTestHelpers::clearAllRegistrars();
    }
};

TEST_F(MyComponentTest, SomeTest) {
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    RegistrarTestHelpers::setupMockRegistrars();
    
    // Test your component...
}

TEST_F(MyComponentTest, CustomBoardTest) {
    auto boardInfo = BoardInfoTestHelpers::createCustomBoardInfo(20, 15, 500, 10);
    RegistrarTestHelpers::setupGameManagerRegistrar("CustomGM");
    RegistrarTestHelpers::setupCompleteAlgorithm("Algo1");
    RegistrarTestHelpers::setupCompleteAlgorithm("Algo2");
    
    // Test with custom setup...
}
```

## Adding New Helpers

When adding new helper functions:
1. Add to appropriate header in `/helpers/`
2. Create corresponding test file to validate helpers work correctly
3. Use inline functions for simple utilities
4. Document purpose and usage in comments
5. Update this README if adding new categories