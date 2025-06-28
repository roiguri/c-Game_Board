# GameManager Testing Mocks - Usage Guide

This guide covers the available mock components for testing GameManager functionality. **Use these mocks instead of creating hardcoded test data** to ensure consistency and maintainability.

## 📋 Available Mock Components

1. **Game Object Utilities** (`game_object_utilities.h`)
2. **Enhanced Mock Algorithm** (`enhanced_mock_algorithm.h`)
3. **Enhanced Mock Player** (`enhanced_mock_player.h`)  
4. **Mock Factories** (`mock_factories.h`)
5. **Mock Satellite View** (`scenario_mock_satellite_view.h`)
6. **Game Scenario Builder** (`game_scenario_builder.h`)
7. **Game Result Validator** (`game_result_validator.h`)

---

## 🎯 Game Object Utilities

**Purpose**: Simplified utilities for creating and manipulating game objects (shells, tanks) in tests with consistent patterns.

### Key Features:
- **Consolidated Shell Creation**: Single `createShell()` method with optional destruction parameter
- **Multi-Shell Creation**: Create multiple shells with same or different properties
- **Collection Verification**: Verify shell collections match expected properties
- **Shell Counting**: Count active, destroyed, or filtered shells

### Shell Creation Examples:

```cpp
// Basic shell creation (consolidated API)
Shell activeShell = GameObjectUtilities::createShell(1, Point(1, 1), Direction::Right);
Shell destroyedShell = GameObjectUtilities::createShell(2, Point(2, 2), Direction::Left, true);

// Convenience method for destroyed shells (backward compatibility)
Shell destroyedShell2 = GameObjectUtilities::createDestroyedShell(1, Point(3, 3), Direction::Up);

// Create multiple shells at different positions
std::vector<Point> positions = {Point(1, 1), Point(2, 2), Point(3, 3)};
auto shells = GameObjectUtilities::createShellsAtPositions(positions, 1, Direction::Right);

// Mix of active and destroyed shells
auto mixedShells = GameObjectUtilities::createShellsAtPositions(
    {Point(1, 1), Point(2, 2)}, 
    2, 
    Direction::Down, 
    false  // Active shells
);
// Add some destroyed shells
auto destroyedShells = GameObjectUtilities::createShellsAtPositions(
    {Point(4, 4), Point(5, 5)}, 
    1, 
    Direction::Up, 
    true   // Destroyed shells
);
```

### Test Integration Examples:

```cpp
TEST_F(GameManagerTest, RemoveDestroyedShells_UsesUtilities) {
    // Create test shells using simplified utilities
    Shell activeShell1 = GameObjectUtilities::createShell(1, Point(1, 1), Direction::Right);
    Shell destroyedShell = GameObjectUtilities::createShell(2, Point(2, 2), Direction::Left, true);
    Shell activeShell2 = GameObjectUtilities::createShell(1, Point(3, 3), Direction::Up);
    
    // Add shells directly to container
    auto& shells = GetShells();
    shells.insert(shells.end(), {activeShell1, destroyedShell, activeShell2});
    
    // Act: Remove destroyed shells
    RemoveDestroyedShells();
    
    // Assert: Verify only active shells remain
    std::vector<Shell> expectedRemaining = {activeShell1, activeShell2};
    GameObjectUtilities::verifyShellCollection(GetShells(), expectedRemaining);
}

TEST_F(GameManagerTest, ShellMovement_MultipleShells) {
    // Create shells at different positions using utility
    auto shells = GameObjectUtilities::createShellsAtPositions({
        Point(1, 1), Point(0, 0), Point(2, 2), Point(4, 4)
    }, 1, Direction::Right);
    
    // Customize specific shells
    shells[1] = GameObjectUtilities::createShell(2, Point(0, 0), Direction::Left);
    shells[2] = GameObjectUtilities::createShell(1, Point(2, 2), Direction::Up);
    shells[3] = GameObjectUtilities::createShell(2, Point(4, 4), Direction::Down);
    
    // Add to game and test movement
    GetShells().insert(GetShells().end(), shells.begin(), shells.end());
    MoveShellsOnce();
    
    // Verify new positions
    EXPECT_EQ(GetShells()[0].getPosition(), Point(2, 1));  // Right movement
    EXPECT_EQ(GetShells()[1].getPosition(), Point(4, 0));  // Left with wrap
    // ... more assertions
}
```

### Collection Operations:

```cpp
// Direct container operations (no wrapper methods needed)
auto& shells = GetShells();

// Add shells directly
shells.clear();
shells.insert(shells.end(), {shell1, shell2, shell3});

// Count different types of shells
size_t activeCount = GameObjectUtilities::countActiveShells(GetShells());
size_t destroyedCount = GameObjectUtilities::countDestroyedShells(GetShells());

// Custom counting with predicate
size_t player1Shells = GameObjectUtilities::countShells(GetShells(), 
    [](const Shell& s) { return s.getPlayerId() == 1; });

// Verify shell collections match expected
std::vector<Shell> expectedShells = {
    GameObjectUtilities::createShell(1, Point(1, 1), Direction::Right),
    GameObjectUtilities::createShell(2, Point(2, 2), Direction::Left, true)
};
GameObjectUtilities::verifyShellCollection(GetShells(), expectedShells);
```

### Utility Methods Reference:

```cpp
// Shell Creation
createShell(playerId, position, direction, isDestroyed=false)  // Main creation method
createDestroyedShell(playerId, position, direction)           // Convenience for destroyed shells
createShellsAtPositions(positions, playerId, direction, isDestroyed=false)  // Multiple shells

// Collection Verification
verifyShell(actual, expected, context)                        // Single shell verification
verifyShellCollection(actualContainer, expectedContainer)     // Collection verification

// Counting and Analysis
countShells(container, predicate)                             // Custom counting
countActiveShells(container)                                  // Count non-destroyed shells
countDestroyedShells(container)                              // Count destroyed shells
```

### Best Practices with GameObjectUtilities:

```cpp
// ✅ DO: Use direct container access with utilities
auto& shells = GetShells();
shells.insert(shells.end(), GameObjectUtilities::createShellsAtPositions(positions));

// ✅ DO: Use consolidated createShell with isDestroyed parameter
Shell shell = GameObjectUtilities::createShell(1, Point(1, 1), Direction::Right, true);

// ✅ DO: Use utility verification methods
GameObjectUtilities::verifyShellCollection(GetShells(), expectedShells);

// ❌ DON'T: Create wrapper methods in test fixtures
// Instead of: void AddShells(const std::vector<Shell>& shells)
// Use direct: GetShells().insert(GetShells().end(), shells.begin(), shells.end());

// ❌ DON'T: Manually create shell objects in loops
// Instead of: for(int i = 0; i < 5; ++i) shells.push_back(Shell(...));
// Use: auto shells = GameObjectUtilities::createShellsAtPositions(positions);
```

---

## 🤖 Enhanced Mock Algorithm

**Purpose**: Simulate tank algorithms with configurable behavior and state tracking.

### Key Features:
- **Constant Action Mode**: Always returns the same action
- **Action Sequence Mode**: Executes predefined action sequences with looping
- **Call Tracking**: Monitors `getAction()` and `updateBattleInfo()` calls
- **BattleInfo Capture**: Stores the last received battle information

### Usage Examples:

```cpp
// Constant action algorithm
auto passiveAlgo = std::make_unique<EnhancedMockAlgorithm>(ActionRequest::DoNothing);
auto aggressiveAlgo = std::make_unique<EnhancedMockAlgorithm>(ActionRequest::Shoot);

// Sequence-based algorithm
std::vector<ActionRequest> tacticalSequence = {
    ActionRequest::MoveForward,
    ActionRequest::RotateRight45,
    ActionRequest::Shoot,
    ActionRequest::MoveBackward
};
auto tacticalAlgo = std::make_unique<EnhancedMockAlgorithm>(tacticalSequence);

// Verify algorithm was called correctly
EXPECT_GT(algo->getActionCallCount(), 0);
EXPECT_GT(algo->updateBattleInfoCallCount(), 0);

// Inspect received battle info
const BattleInfo* info = algo->getLastBattleInfo();
ASSERT_NE(info, nullptr);
```

---

## 👤 Enhanced Mock Player

**Purpose**: Mock player behavior with GMock integration and call tracking.

### Key Features:
- **GMock Integration**: Full Google Mock expectation system
- **Custom Behavior**: Set lambda functions for `updateTankWithBattleInfo`
- **Call Tracking**: Monitor method invocations
- **Parameter Access**: Inspect passed TankAlgorithm and SatelliteView

### Usage Examples:

```cpp
// Basic expectation setting
auto mockPlayer = std::make_unique<ConfigurableMockPlayer>();
EXPECT_CALL(*mockPlayer, updateTankWithBattleInfo(_, _))
    .Times(::testing::AtLeast(1));

// Custom behavior with parameter inspection
mockPlayer->setUpdateBehavior([](TankAlgorithm& algo, SatelliteView& view) {
    // Custom logic here - inspect parameters, modify algorithm state, etc.
    auto* mockAlgo = dynamic_cast<EnhancedMockAlgorithm*>(&algo);
    if (mockAlgo) {
        mockAlgo->setConstantAction(ActionRequest::Shoot);
    }
});

// Simple call counting
auto simplePlayer = std::make_unique<SimpleMockPlayer>();
// ... run game ...
EXPECT_EQ(simplePlayer->getCallCount(), expectedCalls);
```

---

## 🏭 Mock Factories

**Purpose**: Create configurable algorithm factories with call tracking and pre-configuration.

### Built-in Factories:
- `mockFactoryDoNothing` - Creates passive algorithms
- `mockFactoryShooter` - Creates shooting algorithms

### Factory Creators:
- `createMockFactoryWithAction()` - Single constant action
- `createMockFactoryWithSequence()` - Complex action sequences

### Usage Examples:

```cpp
// Use built-in factories
gameManager->run(width, height, satelliteView, maxSteps, numShells,
                *player1, *player2, 
                mockFactoryDoNothing, mockFactoryShooter);

// Create custom factories
auto tacticalFactory = createMockFactoryWithSequence("tactical", {
    ActionRequest::MoveForward,
    ActionRequest::Shoot,
    ActionRequest::RotateLeft45
});

auto defensiveFactory = createMockFactoryWithAction("defensive", ActionRequest::MoveBackward);

// Verify factory calls
auto calls = MockFactoryConfigurer::getFactoryCalls("tactical");
EXPECT_EQ(calls.size(), 2); // Expect 2 calls
EXPECT_EQ(calls[0].first, 1); // First call for player 1

// Pre-configure specific algorithms
auto customAlgo = std::make_shared<EnhancedMockAlgorithm>(ActionRequest::Shoot);
MockFactoryConfigurer::setAlgorithmForFactory("tactical", 1, 0, customAlgo);

// Clean up between tests
MockFactoryConfigurer::resetAll();
```

---

## 🛰️ Mock Satellite View

**Purpose**: Create configurable game boards for testing various scenarios.

### Key Features:
- **Empty Board Creation**: `createEmpty(width, height)`
- **Dynamic Building**: Add tanks, walls, mines, shells at runtime
- **Board Manipulation**: Clear, modify, inspect board contents
- **Analysis Tools**: Find positions, count elements, visualize board

### Usage Examples:

```cpp
// Create empty board and build scenario
auto view = MockSatelliteView::createEmpty(8, 6);
view.addTank(1, 1, 1);        // Player 1 tank at (1,1)
view.addTank(6, 4, 2);        // Player 2 tank at (6,4)
view.addWall(3, 2);           // Wall at (3,2)
view.addMine(4, 3);           // Mine at (4,3)

// Walled arena scenario
auto arena = MockSatelliteView::createEmpty(7, 5);
// Add perimeter walls
for (size_t x = 0; x < 7; ++x) {
    arena.addWall(x, 0);      // Top wall
    arena.addWall(x, 4);      // Bottom wall
}
for (size_t y = 1; y < 4; ++y) {
    arena.addWall(0, y);      // Left wall
    arena.addWall(6, y);      // Right wall
}
arena.addTank(2, 2, 1);
arena.addTank(4, 2, 2);

// Board analysis
auto tankPositions = view.findPositions('1');  // Find all player 1 tanks
int wallCount = view.countCharacter('#');       // Count walls
std::string boardStr = view.toString();         // Visual debugging

// Custom grid from string vector
std::vector<std::string> customGrid = {
    "# # #",
    " 1 2 ",
    "# # #"
};
auto customView = MockSatelliteView(customGrid, 5, 3);
```

---

## 🏗️ Game Scenario Builder

**Purpose**: Fluent interface for building complex game test scenarios without hardcoded data.

### Key Features:
- **Fluent Builder Pattern**: Chain method calls for readable scenario construction
- **Flexible Board Setup**: Configure dimensions, max steps, shells
- **Dynamic Object Placement**: Add tanks, walls, mines, shells at specific positions
- **Player Configuration**: Set custom behaviors for mock players
- **Utility Methods**: Convenience methods for common patterns

### Usage Examples:

```cpp
// Basic two-player scenario
auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
    GameScenarioBuilder()
        .withBoardSize(7, 5)
        .withTwoPlayerSetup(Point(1, 1), Point(5, 3))
        .withMaxSteps(50)
        .withNumShells(10)
        .build();

// Complex scenario with obstacles
auto scenario = GameScenarioBuilder()
    .withBoardSize(12, 8)
    .withTank(Point(2, 2))                    // Player 1
    .withTank(Point(9, 5))                    // Player 2
    .withHorizontalWall(3, 2, 6)             // Wall line from (2,3) to (6,3)
    .withVerticalWall(5, 1, 4)               // Wall line from (5,1) to (5,4)
    .withBorderWalls()                        // Perimeter walls
    .withMines({Point(4, 4), Point(7, 2)})   // Multiple mines
    .withShells({Point(3, 3), Point(8, 6)})  // Pre-placed shells
    .withMaxSteps(100)
    .build();

// Arena-style scenario
auto arena = GameScenarioBuilder()
    .withBoardSize(15, 10)
    .withTwoPlayerSetup(Point(2, 2), Point(12, 7))
    .withBorderWalls()                        // Enclosed arena
    .withVerticalWall(7, 2, 7)               // Central divider
    .withMines({Point(5, 5), Point(9, 5)})   // Strategic mine placement
    .withMaxSteps(200)
    .withNumShells(15)
    .build();

// Multiple tanks scenario
auto multiTank = GameScenarioBuilder()
    .withBoardSize(20, 15)
    .withTanks({
        Point(3, 3), Point(5, 5), Point(7, 7),  // Player 1 tanks
        Point(15, 10), Point(17, 12)             // Player 2 tanks
    })
    .withMaxSteps(300)
    .build();

// Quick satellite view only (for simple tests)
auto quickView = GameScenarioBuilder()
    .withBoardSize(5, 5)
    .withTwoPlayerSetup(Point(1, 1), Point(3, 3))
    .buildSatelliteView();  // Returns only MockSatelliteView

// Clear and rebuild pattern
auto builder = GameScenarioBuilder()
    .withBoardSize(10, 10)
    .withTank(Point(1, 1));

// Test scenario A
auto scenarioA = builder
    .withTank(Point(8, 8))
    .withMaxSteps(50)
    .build();

// Reset and test scenario B
auto scenarioB = builder
    .clear()                                  // Clear previous setup
    .withTank(Point(5, 5))
    .withWalls({Point(3, 3), Point(7, 7)})
    .withMaxSteps(75)
    .build();
```

### Builder Methods Reference:

```cpp
// Board Configuration
.withBoard(width, height)           // Set board dimensions
.withBoardSize(width, height)       // Alias for withBoard()
.withMaxSteps(steps)                // Set maximum game steps
.withNumShells(shells)              // Set shells per player

// Tank Placement
.withTank(Point(x, y))              // Add single tank
.withTanks(vector<Point>)           // Add multiple tanks
.withTwoPlayerSetup(p1, p2)         // Quick 2-player setup

// Wall Construction
.withWall(Point(x, y))              // Single wall
.withWalls(vector<Point>)           // Multiple walls
.withHorizontalWall(y, startX, endX) // Horizontal wall line
.withVerticalWall(x, startY, endY)  // Vertical wall line
.withBorderWalls()                  // Perimeter walls

// Other Objects
.withMine(Point(x, y))              // Single mine
.withMines(vector<Point>)           // Multiple mines
.withShell(Point(x, y))             // Single shell
.withShells(vector<Point>)          // Multiple shells

// Player Behavior (Advanced)
.withPlayer1Behavior(lambda)        // Custom player 1 behavior
.withPlayer2Behavior(lambda)        // Custom player 2 behavior

// Utility
.clear()                            // Reset builder state
.build()                            // Build complete scenario
.buildSatelliteView()               // Build satellite view only
```

---

## ✅ Game Result Validator

**Purpose**: Comprehensive validation of GameResult structures with detailed error reporting.

### Key Features:
- **Structural Validation**: Verify GameResult completeness and consistency
- **Game State Analysis**: Validate final board state and object positions
- **Rounds/Steps Validation**: Check game duration and termination conditions
- **Custom Assertions**: Rich error messages for debugging
- **Position-Specific Checks**: Validate specific board locations

### Usage Examples:

```cpp
// Basic result validation
GameResult result = gameManager->run(/* ... */);

// Validate result structure
GameResultValidator::validateConsistency(result);
GameResultValidator::validateGameStateExists(result);

// Validate game termination
GameResultValidator::validateReason(result, GameResult::Reason::MAX_STEPS);
GameResultValidator::validateRounds(result, 50);  // Exactly 50 rounds
GameResultValidator::validateRoundsRange(result, 10, 100);  // Between 10-100 rounds

// Validate final game state
GameResultValidator::validateGameStateConsistentWithRemainingTanks(result, width, height);
GameResultValidator::validateGameStateNoTanks(result, width, height);
GameResultValidator::validateGameStateTotalTanks(result, 2, width, height);

// Position-specific validation
Point centerPoint(width/2, height/2);
GameResultValidator::validateGameStateEmpty(result, centerPoint);
GameResultValidator::validateGameStateWall(result, Point(0, 0));
GameResultValidator::validateGameStateObjectAt(result, Point(3, 3), '1');  // Tank '1'

// Complex validation scenarios
TEST_F(GameManagerTest, ValidateComplexEndState) {
    auto [view, p1, p2, w, h, maxSteps, shells] = 
        GameScenarioBuilder()
            .withBoardSize(8, 6)
            .withTwoPlayerSetup(Point(1, 1), Point(6, 4))
            .withMaxSteps(10)
            .build();

    GameResult result = gameManager->run(w, h, view, maxSteps, shells, 
                                       *p1, *p2, 
                                       mockFactoryDoNothing, mockFactoryDoNothing);

    // Comprehensive validation
    GameResultValidator::validateConsistency(result);
    GameResultValidator::validateGameStateExists(result);
    
    if (result.reason == GameResult::Reason::MAX_STEPS) {
        GameResultValidator::validateRounds(result, maxSteps);
        GameResultValidator::validateReason(result, GameResult::Reason::MAX_STEPS);
    }
    
    // Validate final state consistency
    GameResultValidator::validateGameStateConsistentWithRemainingTanks(result, w, h);
    
    // Check specific positions are accessible
    for (size_t x = 0; x < w; ++x) {
        for (size_t y = 0; y < h; ++y) {
            Point pos(x, y);
            char objectAt = result.gameState->getObjectAt(x, y);
            EXPECT_TRUE(objectAt == ' ' || objectAt == '#' || objectAt == '*' || 
                       (objectAt >= '1' && objectAt <= '9'))
                << "Invalid object at (" << x << "," << y << "): " << objectAt;
        }
    }
}
```

### Validation Methods Reference:

```cpp
// Structure Validation
validateConsistency(result)                    // Overall result consistency
validateGameStateExists(result)               // gameState is not null

// Termination Validation  
validateReason(result, expectedReason)         // Specific end reason
validateRounds(result, expectedRounds)         // Exact round count
validateRoundsRange(result, min, max)          // Round count range

// Game State Validation
validateGameStateConsistentWithRemainingTanks(result, w, h)  // State matches tank counts
validateGameStateNoTanks(result, w, h)                       // No tanks on board
validateGameStateTotalTanks(result, count, w, h)             // Specific tank count

// Position-Specific Validation
validateGameStateEmpty(result, point)          // Position is empty
validateGameStateWall(result, point)           // Position has wall
validateGameStateObjectAt(result, point, char) // Specific object at position
```

---

## 🧪 Complete Test Example

```cpp
TEST_F(InterfaceComplianceTest, CompleteScenarioWithGameObjectUtilities) {
    // 1. Set up mock factories
    MockFactoryConfigurer::resetAll();
    auto aggressiveFactory = createMockFactoryWithSequence("aggressive", {
        ActionRequest::MoveForward, ActionRequest::Shoot
    });
    auto defensiveFactory = createMockFactoryWithAction("defensive", ActionRequest::MoveBackward);

    // 2. Create board scenario using GameScenarioBuilder
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(10, 8)
            .withTwoPlayerSetup(Point(2, 2), Point(7, 5))
            .withWalls({Point(4, 3), Point(5, 4)})
            .withMaxSteps(50)
            .withNumShells(10)
            .build();

    // 3. Set up mock player expectations
    EXPECT_CALL(*player1, updateTankWithBattleInfo(_, _)).Times(::testing::AtLeast(1));
    EXPECT_CALL(*player2, updateTankWithBattleInfo(_, _)).Times(::testing::AtLeast(1));

    // 4. Run game
    GameResult result = gameManager->run(
        width, height, satelliteView, maxSteps, numShells,
        *player1, *player2,
        aggressiveFactory, defensiveFactory
    );

    // 5. Verify results using GameResultValidator
    GameResultValidator::validateConsistency(result);
    GameResultValidator::validateGameStateExists(result);
    EXPECT_TRUE(result.winner >= 0 && result.winner <= 2);
    
    // 6. Test shell management using GameObjectUtilities
    if (result.gameState) {
        // Create expected shells for validation
        auto expectedShells = GameObjectUtilities::createShellsAtPositions({
            Point(2, 3), Point(7, 6)  // Expected shell positions after game
        }, 1, Direction::Right);
        
        // Verify shell counts using utilities
        size_t totalShells = GameObjectUtilities::countActiveShells(GetShells());
        EXPECT_GE(totalShells, 0);  // At least 0 shells remain
    }
    
    // Verify factory calls
    auto aggressiveCalls = MockFactoryConfigurer::getFactoryCalls("aggressive");
    auto defensiveCalls = MockFactoryConfigurer::getFactoryCalls("defensive");
    EXPECT_FALSE(aggressiveCalls.empty());
    EXPECT_FALSE(defensiveCalls.empty());

    // Validate game termination
    if (result.reason == GameResult::Reason::MAX_STEPS) {
        GameResultValidator::validateRounds(result, maxSteps);
    }
}
```

---

## ⚠️ Best Practices

### ✅ DO:
- **Use GameObjectUtilities** for all shell creation and verification instead of manual construction
- **Use consolidated createShell()** with the optional `isDestroyed` parameter
- **Use GameScenarioBuilder** for all scenario construction instead of hardcoded data
- **Use GameResultValidator** for comprehensive result verification
- **Use direct container access** with utilities instead of wrapper methods
- **Reset factory state** between tests with `MockFactoryConfigurer::resetAll()`
- **Chain builder methods** for readable scenario construction
- **Use meaningful factory names** for tracking (e.g., "aggressivePlayer", "tacticalMover")
- **Verify mock interactions** with expectations and call tracking
- **Validate game termination** with appropriate GameResultValidator methods

### ❌ DON'T:
- **Create wrapper methods** in test fixtures for shell operations - use direct container access
- **Hardcode shell creation** - use GameObjectUtilities methods
- **Hardcode board layouts** - use GameScenarioBuilder instead
- **Skip result validation** - always use GameResultValidator for comprehensive checks
- **Create custom algorithm classes** - use EnhancedMockAlgorithm with sequences
- **Forget to reset factory state** between tests
- **Use generic names** for custom factories (makes debugging harder)
- **Manually validate GameResult** - use helper methods instead

---

## 🔧 Test Setup Template

```cpp
class MyGameManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        gameManager = std::make_unique<GameManager>(false);
        MockFactoryConfigurer::resetAll(); // Always reset factory state
    }

    void TearDown() override {
        MockFactoryConfigurer::resetAll(); // Clean up after test
    }

    std::unique_ptr<GameManager> gameManager;
};
```

This mock system provides everything needed for comprehensive GameManager testing. Use these components to create maintainable, readable, and reliable tests without duplicating functionality. 