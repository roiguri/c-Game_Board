#include <gtest/gtest.h>
#include "game_manager.h"
#include "test/helpers/game_object_utilities.h"
#include "test/helpers/game_scenario_builder.h"
#include "test/helpers/file_test_helper.h"
#include "test/mocks/scenario_mock_satellite_view.h"
#include "test/mocks/mock_factories.h"
#include "objects/shell.h"
#include "utils/point.h"
#include "utils/direction.h"
#include "common/SatelliteView.h"
#include <memory>
#include <fstream>
#include <sstream>
#include <cstdio> // for std::remove
#include <iostream>
#include <typeinfo>

namespace GameManager_098765432_123456789 { 

using namespace UserCommon_098765432_123456789;

// Test fixture for GameManager
class GameManagerTest : public ::testing::Test {
protected:
    std::unique_ptr<GameManager> manager;
    std::unique_ptr<FileTestHelper> fileHelper;

    // ==================== GAMEMANAGER MEMBER ACCESS (SETTERS/GETTERS) ====================
    
    std::vector<Shell>& GetShells() {
        return manager->m_shells;
    }
    std::vector<Tank>& GetTanks() {
        return manager->m_tanks;
    }
    std::vector<GameManager::TankWithAlgorithm>& GetTankControllers() {
        return manager->m_tankControllers;
    }
    GameBoard& GetBoard() {
        return manager->m_board;
    }
    std::vector<std::string>& GetGameLog() {
        return manager->m_gameLog;
    }
    bool GetVerbose() {
        return manager->m_verbose;
    }
    std::string& GetGameResult() {
        return manager->m_gameResult;
    }
    bool GetIsClassic2PlayerGame() {
        return manager->m_isClassic2PlayerGame;
    }
    void SetIsClassic2PlayerGame(bool value) {
        manager->m_isClassic2PlayerGame = value;
    }
    int GetCurrentStep() {
        return manager->m_currentStep;
    }
    void SetCurrentStep(int step) {
        manager->m_currentStep = step;
    }
    int GetRemainingSteps() {
        return manager->m_remaining_steps;
    }
    void SetRemainingSteps(int steps) {
        manager->m_remaining_steps = steps;
    }
    void SetMaxSteps(int maxSteps) {
        manager->m_maximum_steps = maxSteps;
    }

    // ==================== GAMEMANAGER METHOD WRAPPERS ====================
    
    void CreateTanks(const std::vector<std::pair<int, Point>>& positions) {
        manager->createTanks(positions);
    }
    void CreateTankAlgorithms(TankAlgorithmFactory factory1 = nullptr, TankAlgorithmFactory factory2 = nullptr) {
        if (factory1 == nullptr) {
            factory1 = mockFactoryDoNothing;
        }
        if (factory2 == nullptr) {
            factory2 = mockFactoryDoNothing;
        }
        manager->createTankAlgorithms(factory1, factory2);
    }
    void ApplyAction(GameManager::TankWithAlgorithm& controller) {
        manager->applyAction(controller);
    }
    void MoveShellsOnce() {
        manager->moveShellsOnce();
    }
    void RemoveDestroyedShells() {
        manager->removeDestroyedShells();
    }
    void LogAction() {
        manager->logAction();
    }
    std::string CallGenerateOutputFilePath(const std::string& player1Name, const std::string& player2Name) {
        return manager->generateOutputFilePath(player1Name, player2Name);
    }
    std::string CallCleanFilename(const std::string& name) {
        return manager->cleanFilename(name);
    }
    std::vector<std::string> CallReadSatelliteView(const SatelliteView& view,
                                                  size_t width, size_t height) {
        return manager->readSatelliteView(view, width, height);
    }
    bool CallCheckGameOver() {
        return manager->checkGameOver();
    }
    void CallProcessStep() {
        manager->processStep();
    }
    std::string ActionToString(ActionRequest action) {
        return manager->actionToString(action);
    }

    // ==================== TEST UTILITIES AND HELPERS ====================
    
    // Tank controller utilities
    void SetWasKilledInPreviousStep(size_t controllerIndex, bool value) {
        auto& controllers = GetTankControllers();
        ASSERT_LT(controllerIndex, controllers.size());
        controllers[controllerIndex].wasKilledInPreviousStep = value;
    }

    void SetUp() override {
        manager = std::make_unique<GameManager>(/*verbose=*/false);
        fileHelper = std::make_unique<FileTestHelper>();
        // Initialize the board to a 5x5 empty board
        GetBoard() = GameBoard(5, 5);
    }

    void TearDown() override {
        // Clean up after test
        MockFactoryConfigurer::resetAll();
        // FileTestHelper destructor will clean up test files
    }
};

// ===================================================================== //
// Utility Methods
// ===================================================================== //
// 1. actionToString(ActionRequest action)
// 2. getInitialDirection(int playerId)
// 3. readSatelliteView(const SatelliteView& satellite_view, size_t map_width, size_t map_height
// 4. isClassic2PlayerGame(const GameBoard& board)
// ===================================================================== //

TEST_F(GameManagerTest, ReadSatelliteViewBasic) {
    std::vector<std::string> expectedGrid = {
        "# 1",
        " @ ",
        "2  "
    };
    MockSatelliteView mockView(expectedGrid, 3, 3);
    
    std::vector<std::string> result = CallReadSatelliteView(mockView, 3, 3);
    
    // Verify the result matches the expected grid
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "# 1");
    EXPECT_EQ(result[1], " @ ");
    EXPECT_EQ(result[2], "2  ");
}

TEST_F(GameManagerTest, ReadSatelliteViewEmptyBoard) {
    // Create a 2x2 empty board
    std::vector<std::string> expectedGrid = {
        "  ",
        "  "
    };
    
    MockSatelliteView mockView(expectedGrid, 2, 2);
    
    std::vector<std::string> result = CallReadSatelliteView(mockView, 2, 2);
    
    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], "  ");
    EXPECT_EQ(result[1], "  ");
}

TEST_F(GameManagerTest, ReadSatelliteViewSingleCell) {
    // Create a 1x1 board with a wall
    std::vector<std::string> expectedGrid = {"#"};
    
    MockSatelliteView mockView(expectedGrid, 1, 1);
    
    std::vector<std::string> result = CallReadSatelliteView(mockView, 1, 1);
    
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], "#");
}

TEST_F(GameManagerTest, ReadSatelliteViewZeroDimensions) {
    // Test edge case with zero dimensions
    std::vector<std::string> expectedGrid = {};
    
    MockSatelliteView mockView(expectedGrid, 0, 0);
    
    std::vector<std::string> result = CallReadSatelliteView(mockView, 0, 0);
    
    EXPECT_EQ(result.size(), 0);
}

// TEST_F(GameManagerTest, IsClassic2PlayerGame_DetectionTests) {
//     // Test 1: Classic 2-player game (players 1 and 2 only)
//     {
//         // Create a simple board with only players 1 and 2
//         std::string boardContent = R"(Test Board
// MaxSteps = 100
// NumShells = 10
// Rows = 3
// Cols = 3
// 1 2
   
//    )";
        
//         GameManager testManager(playerFactory, algoFactory);
//         EXPECT_TRUE(CallReadBoardFromContent(testManager, boardContent));
//         EXPECT_TRUE(GetIsClassic2PlayerGame(testManager));
//     }
    
//     // Test 2: Multi-player game (players 1, 2, and 3)
//     {
//         std::string boardContent = R"(Multi Player Board
// MaxSteps = 100
// NumShells = 10
// Rows = 3
// Cols = 3
// 1 2
// 3  
//    )";
        
//         GameManager testManager(playerFactory, algoFactory);
//         EXPECT_TRUE(CallReadBoardFromContent(testManager, boardContent));
//         EXPECT_FALSE(GetIsClassic2PlayerGame(testManager));
//     }
    
//     // Test 3: Only player 1 (should be false)
//     {
//         std::string boardContent = R"(Single Player Board
// MaxSteps = 100
// NumShells = 10
// Rows = 3
// Cols = 3
// 1  
   
//    )";
        
//         GameManager testManager(playerFactory, algoFactory);
//         EXPECT_TRUE(CallReadBoardFromContent(testManager, boardContent));
//         EXPECT_FALSE(GetIsClassic2PlayerGame(testManager));
//     }
    
//     // Test 4: Players 2 and 3 (missing player 1, should be false)
//     {
//         std::string boardContent = R"(No Player 1 Board
// MaxSteps = 100
// NumShells = 10
// Rows = 3
// Cols = 3
// 2 3
   
//    )";
        
//         GameManager testManager(playerFactory, algoFactory);
//         EXPECT_TRUE(CallReadBoardFromContent(testManager, boardContent));
//         EXPECT_FALSE(GetIsClassic2PlayerGame(testManager));
//     }
    
//     // Test 5: 9-player game (should be false)
//     {
//         std::string boardContent = R"(Max Players Board
// MaxSteps = 100
// NumShells = 10
// Rows = 3
// Cols = 9
// 123456789)";
        
//         GameManager testManager(playerFactory, algoFactory);
//         EXPECT_TRUE(CallReadBoardFromContent(testManager, boardContent));
//         EXPECT_FALSE(GetIsClassic2PlayerGame(testManager));
//     }
    
//     // Test 6: Default initialization (should be false)
//     {
//         GameManager testManager(playerFactory, algoFactory);
//         EXPECT_FALSE(GetIsClassic2PlayerGame(testManager));
//     }
// }

// ===================================================================== //
// Constructor and Basic Setup
// ===================================================================== //
// 1. GameManager(bool verbose)
// 2. generateOutputFilePath(const std::string& player1Name, const std::string& player2Name)
// 3. cleanFilename(const std::string& name)
// ===================================================================== //

TEST_F(GameManagerTest, Constructor_VerboseFlagTrue) {
    manager = std::make_unique<GameManager>(true);
    EXPECT_TRUE(GetVerbose());
}

TEST_F(GameManagerTest, Constructor_VerboseFlagFalse) {
    manager = std::make_unique<GameManager>(false);
    EXPECT_FALSE(GetVerbose());
}

TEST_F(GameManagerTest, GenerateOutputFilePath_BasicPlayerNames) {
    std::string result = CallGenerateOutputFilePath("BasicPlayer", "OffensivePlayer");
    
    // Check format: game_BasicPlayer_vs_OffensivePlayer_[timestamp].txt
    EXPECT_TRUE(result.find("game_BasicPlayer_vs_OffensivePlayer_") == 0);
    EXPECT_TRUE(result.find(".txt") == result.length() - 4);
    EXPECT_GT(result.length(), 40); // Should have timestamp
}

TEST_F(GameManagerTest, GenerateOutputFilePath_MangledNames) {
    std::string result = CallGenerateOutputFilePath("11BasicPlayer", "15OffensivePlayer");
    
    // Should preserve mangled names
    EXPECT_TRUE(result.find("game_11BasicPlayer_vs_15OffensivePlayer_") == 0);
    EXPECT_TRUE(result.find(".txt") == result.length() - 4);
}

TEST_F(GameManagerTest, GenerateOutputFilePath_UniqueTimestamps) {
    std::string result1 = CallGenerateOutputFilePath("Player1", "Player2");
    std::string result2 = CallGenerateOutputFilePath("Player1", "Player2");
    
    EXPECT_TRUE(result1.find("game_Player1_vs_Player2_") == 0);
    EXPECT_TRUE(result2.find("game_Player1_vs_Player2_") == 0);
    EXPECT_NE(result1, result2);
}

TEST_F(GameManagerTest, CleanFilename_ValidName) {
    std::string result = CallCleanFilename("BasicPlayer");
    EXPECT_EQ(result, "BasicPlayer");
}

TEST_F(GameManagerTest, CleanFilename_InvalidCharacters) {
    std::string result = CallCleanFilename("Basic\\Player:Name*Test?");
    EXPECT_EQ(result, "Basic_Player_Name_Test_");
}

TEST_F(GameManagerTest, CleanFilename_AllInvalidCharacters) {
    std::string result = CallCleanFilename("\\/:*?\"<>|");
    EXPECT_EQ(result, "_________");
}

TEST_F(GameManagerTest, CleanFilename_EmptyString) {
    std::string result = CallCleanFilename("");
    EXPECT_EQ(result, "");
}

// ===================================================================== //
// Game Initialization Methods
// ===================================================================== //
// 1. createTanks(const std::vector<std::pair<int, Point>>& tankPositions)
// 2. createTankAlgorithms(TankAlgorithmFactory player1_factory, TankAlgorithmFactory player2_factory)
// 3. readBoard(const SatelliteView& satellite_view, ...)
// ===================================================================== //

TEST_F(GameManagerTest, CreateTanks_CreatesTanksAtCorrectPositions) {
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)},
        {2, Point(4, 4)},
        {1, Point(2, 2)}
    };
    CreateTanks(positions);
    
    std::vector<Tank> expectedTanks = GameObjectUtilities::createTanksFromPlayerPositions(positions);
    GameObjectUtilities::verifyTankCollection(GetTanks(), expectedTanks);
}

TEST_F(GameManagerTest, CreateTankAlgorithms_AssociatesAlgorithmsWithTanks) {
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)},
        {2, Point(4, 4)},
        {1, Point(2, 2)}
    };
    CreateTanks(positions);
    CreateTankAlgorithms();
    auto& controllers = GetTankControllers();
    ASSERT_EQ(controllers.size(), 3u);
    for (const auto& controller : controllers) {
        EXPECT_NE(controller.algorithm, nullptr);
    }
}

// ===================================================================== //
// Game State Management
// ===================================================================== //
// 1. removeDestroyedShells()
// 2. moveShellsOnce()
// 3. populateGameResult(int winner, GameResult::Reason reason, const std::vector<size_t>& remainingTanks)
// ===================================================================== //

TEST_F(GameManagerTest, RemoveDestroyedShells_RemovesOnlyDestroyed) {
    // Arrange: Create test shells using utilities
    Shell activeShell1 = GameObjectUtilities::createShell(1, Point(1, 1), Direction::Right);
    Shell destroyedShell = GameObjectUtilities::createDestroyedShell(2, Point(2, 2), Direction::Left);
    Shell activeShell2 = GameObjectUtilities::createShell(1, Point(3, 3), Direction::Up);
    
    // Add shells directly to container
    auto& shells = GetShells();
    shells.insert(shells.end(), {activeShell1, destroyedShell, activeShell2});
    
    // Act: Remove destroyed shells
    RemoveDestroyedShells();
    
    // Assert: Verify only active shells remain using utility
    std::vector<Shell> expectedRemaining = {activeShell1, activeShell2};
    GameObjectUtilities::verifyShellCollection(GetShells(), expectedRemaining);
}

TEST_F(GameManagerTest, MoveShellsOnce_MovesAllShellsCorrectly) {
    // Arrange: Create test shells using utilities
    Shell shellRight = GameObjectUtilities::createShell(1, Point(1, 1), Direction::Right);
    Shell shellLeft = GameObjectUtilities::createShell(2, Point(0, 0), Direction::Left);
    Shell shellUp = GameObjectUtilities::createShell(1, Point(2, 2), Direction::Up);
    Shell shellDown = GameObjectUtilities::createShell(2, Point(4, 4), Direction::Down);
    
    // Add shells directly to container
    auto& shells = GetShells();
    shells.clear();
    shells.insert(shells.end(), {shellRight, shellLeft, shellUp, shellDown});

    // Act: Move shells once
    MoveShellsOnce();

    // Assert: Check new positions using utilities
    std::vector<Shell> expectedShells = {
        GameObjectUtilities::createShell(1, Point(2, 1), Direction::Right),  // Right: (1,1) -> (2,1)
        GameObjectUtilities::createShell(2, Point(4, 0), Direction::Left),   // Left: (0,0) -> (4,0) (wrap)
        GameObjectUtilities::createShell(1, Point(2, 1), Direction::Up),     // Up: (2,2) -> (2,1)
        GameObjectUtilities::createShell(2, Point(4, 0), Direction::Down)    // Down: (4,4) -> (4,0) (wrap)
    };
    GameObjectUtilities::verifyShellCollection(GetShells(), expectedShells);
}

// ===================================================================== //
// Action Processing
// ===================================================================== //
// 1. applyAction(TankWithAlgorithm& controller)
// 2. logAction()
// ===================================================================== //

TEST_F(GameManagerTest, ApplyAction_MoveForward_Success) {
    // Arrange: Create tank using utilities
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.setDirection(Direction::Right);
    controller.nextAction = ActionRequest::MoveForward;
    
    // Act
    ApplyAction(controller);
    
    // Assert: Verify tank moved to expected position
    Tank expectedTank = GameObjectUtilities::createTank(1, Point(2, 1));
    expectedTank.setDirection(Direction::Right);
    GameObjectUtilities::verifyTank(controller.tank, expectedTank, "Tank after MoveForward");
}

TEST_F(GameManagerTest, ApplyAction_MoveForward_BlockedByWall) {
    // Arrange: Set up board with wall and tank using utilities
    GetBoard() = GameBoard(5, 5);
    GetBoard().setCellType(1, 0, GameBoard::CellType::Wall);
    
    std::vector<std::pair<int, Point>> positions = { {1, Point(0, 0)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.setDirection(Direction::Right);
    controller.nextAction = ActionRequest::MoveForward;
    
    // Act
    ApplyAction(controller);
    
    // Assert: Verify tank didn't move due to wall
    Tank expectedTank = GameObjectUtilities::createTank(1, Point(0, 0));
    expectedTank.setDirection(Direction::Right);
    GameObjectUtilities::verifyTank(controller.tank, expectedTank, "Tank blocked by wall");
}

TEST_F(GameManagerTest, ApplyAction_RotateLeft45_UpdatesDirection) {
    // Arrange: Create tank using utilities
    std::vector<std::pair<int, Point>> positions = { {1, Point(2, 2)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.setDirection(Direction::Right);
    controller.nextAction = ActionRequest::RotateLeft45;
    
    // Act
    ApplyAction(controller);
    
    // Assert: Verify tank rotated correctly
    Tank expectedTank = GameObjectUtilities::createTank(1, Point(2, 2));
    expectedTank.setDirection(Direction::UpRight);
    GameObjectUtilities::verifyTank(controller.tank, expectedTank, "Tank after RotateLeft45");
}

TEST_F(GameManagerTest, ApplyAction_Shoot_Success) {
    // Arrange: Create tank using utilities
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.setDirection(Direction::Down);
    controller.nextAction = ActionRequest::Shoot;
    int shellsBefore = GetShells().size();
    
    // Act
    ApplyAction(controller);
    
    // Assert: Verify shell was created with correct properties
    int shellsAfter = GetShells().size();
    EXPECT_EQ(shellsAfter, shellsBefore + 1);
    
    Shell expectedShell = GameObjectUtilities::createShell(1, Point(1, 1), Direction::Down);
    GameObjectUtilities::verifyShell(GetShells().back(), expectedShell, "Created shell");
}

TEST_F(GameManagerTest, ApplyAction_Shoot_FailsOnCooldown) {
    // Arrange: Create tank and put it on cooldown
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.setDirection(Direction::Down);
    controller.tank.shoot(); // Put tank on cooldown
    controller.nextAction = ActionRequest::Shoot;
    int shellsBefore = GetShells().size();
    
    // Act
    ApplyAction(controller);
    
    // Assert: Verify no new shell was created due to cooldown
    int shellsAfter = GetShells().size();
    EXPECT_EQ(shellsAfter, shellsBefore);
}

TEST_F(GameManagerTest, ApplyAction_GetBattleInfo_DoesNotCrash) {
    // Arrange: Create tank using utilities
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.nextAction = ActionRequest::GetBattleInfo;
    
    // Act & Assert: Verify GetBattleInfo action doesn't crash
    EXPECT_NO_THROW(ApplyAction(controller));
    
    // Tank should remain at same position
    Tank expectedTank = GameObjectUtilities::createTank(1, Point(1, 1));
    GameObjectUtilities::verifyTank(controller.tank, expectedTank, "Tank after GetBattleInfo");
}

TEST_F(GameManagerTest, ApplyAction_DoNothing_OnlyStateUpdated) {
    // Arrange: Create tank using utilities
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.nextAction = ActionRequest::DoNothing;
    
    // Act & Assert: Verify DoNothing action doesn't crash and doesn't change state
    EXPECT_NO_THROW(ApplyAction(controller));
    
    Tank expectedTank = GameObjectUtilities::createTank(1, Point(1, 1));
    GameObjectUtilities::verifyTank(controller.tank, expectedTank, "Tank after DoNothing");
}

TEST_F(GameManagerTest, ApplyAction_DestroyedTank_NoAction) {
    // Arrange: Create and destroy tank using utilities
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.destroy();
    controller.nextAction = ActionRequest::MoveForward;
    
    // Act
    ApplyAction(controller);
    
    // Assert: Verify destroyed tank doesn't move
    Tank expectedTank = GameObjectUtilities::createDestroyedTank(1, Point(1, 1));
    GameObjectUtilities::verifyTank(controller.tank, expectedTank, "Destroyed tank should not move");
}

TEST_F(GameManagerTest, LogAction_AllValidActions) {
    // Arrange: Create tanks using utilities
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}, {1, Point(2, 0)}
    };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controllers = GetTankControllers();
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[2].nextAction = ActionRequest::RotateLeft90;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = true;
    controllers[2].actionSuccess = true;
    
    // Act
    LogAction();
    
    // Assert: Verify log contains all valid actions
    ASSERT_FALSE(GetGameLog().empty());
    EXPECT_EQ(GetGameLog().back(), "MoveForward, Shoot, RotateLeft90");
}

TEST_F(GameManagerTest, LogAction_SomeActionsIgnored) {
    // Arrange: Create tanks and set some actions to be ignored
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}, {1, Point(2, 0)}
    };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controllers = GetTankControllers();
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[2].nextAction = ActionRequest::RotateLeft90;
    controllers[0].actionSuccess = false;
    controllers[1].actionSuccess = true;
    controllers[2].actionSuccess = false;
    
    // Act
    LogAction();
    
    // Assert: Verify log shows ignored actions
    ASSERT_FALSE(GetGameLog().empty());
    EXPECT_EQ(GetGameLog().back(), "MoveForward (ignored), Shoot, RotateLeft90 (ignored)");
}

TEST_F(GameManagerTest, LogAction_KilledThisStep) {
    // Arrange: Create tanks and destroy one this step
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}
    };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controllers = GetTankControllers();
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = true;
    controllers[0].tank.destroy();
    SetWasKilledInPreviousStep(0, false);
    
    // Act
    LogAction();
    
    // Assert: Verify log shows tank killed this step
    ASSERT_FALSE(GetGameLog().empty());
    EXPECT_EQ(GetGameLog().back(), "MoveForward (killed), Shoot");
}

TEST_F(GameManagerTest, LogAction_AlreadyDead) {
    // Arrange: Create tanks with one already dead from previous step
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}
    };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controllers = GetTankControllers();
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = true;
    controllers[0].tank.destroy();
    SetWasKilledInPreviousStep(0, true);
    
    // Act
    LogAction();
    
    // Assert: Verify log shows tank was already killed
    ASSERT_FALSE(GetGameLog().empty());
    EXPECT_EQ(GetGameLog().back(), "Killed, Shoot");
}

TEST_F(GameManagerTest, LogAction_KilledAndIgnored) {
    // Arrange: Create tanks with one killed and one action ignored
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}
    };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controllers = GetTankControllers();
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = false;
    controllers[0].tank.destroy();
    SetWasKilledInPreviousStep(0, false);
    
    // Act
    LogAction();
    
    // Assert: Verify log shows both killed and ignored actions
    ASSERT_FALSE(GetGameLog().empty());
    EXPECT_EQ(GetGameLog().back(), "MoveForward (killed), Shoot (ignored)");
}

TEST_F(GameManagerTest, LogAction_MultipleKilled) {
    // Arrange: Create tanks with multiple killed this step
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}, {1, Point(2, 0)}
    };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controllers = GetTankControllers();
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[2].nextAction = ActionRequest::RotateLeft90;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = true;
    controllers[2].actionSuccess = true;
    controllers[0].tank.destroy();
    controllers[1].tank.destroy();
    SetWasKilledInPreviousStep(0, false);
    SetWasKilledInPreviousStep(1, false);
    
    // Act
    LogAction();
    
    // Assert: Verify log shows multiple killed tanks
    ASSERT_FALSE(GetGameLog().empty());
    EXPECT_EQ(GetGameLog().back(), "MoveForward (killed), Shoot (killed), RotateLeft90");
}

TEST_F(GameManagerTest, LogAction_AllKilled) {
    // Arrange: Create tanks with all killed from previous steps
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}, {1, Point(2, 0)}
    };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controllers = GetTankControllers();
    for (size_t i = 0; i < controllers.size(); ++i) {
        controllers[i].tank.destroy();
        SetWasKilledInPreviousStep(i, true);
    }
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[2].nextAction = ActionRequest::RotateLeft90;
    
    // Act
    LogAction();
    
    // Assert: Verify log shows all tanks were killed
    ASSERT_FALSE(GetGameLog().empty());
    EXPECT_EQ(GetGameLog().back(), "Killed, Killed, Killed");
}

TEST_F(GameManagerTest, LogAction_AllDoNothing) {
    // Arrange: Create tanks with all doing nothing
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}, {1, Point(2, 0)}
    };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controllers = GetTankControllers();
    for (auto& c : controllers) {
        c.nextAction = ActionRequest::DoNothing;
        c.actionSuccess = true;
    }
    
    // Act
    LogAction();
    
    // Assert: Verify log shows all tanks doing nothing
    ASSERT_FALSE(GetGameLog().empty());
    EXPECT_EQ(GetGameLog().back(), "DoNothing, DoNothing, DoNothing");
}

TEST_F(GameManagerTest, LogAction_MixedKilledIgnoredValid) {
    // Arrange: Create tanks with mixed outcomes (killed, ignored, valid)
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}, {1, Point(2, 0)}
    };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controllers = GetTankControllers();
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[2].nextAction = ActionRequest::RotateLeft90;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = false;
    controllers[2].actionSuccess = true;
    controllers[0].tank.destroy();
    SetWasKilledInPreviousStep(0, false);
    
    // Act
    LogAction();
    
    // Assert: Verify log shows mixed outcomes
    ASSERT_FALSE(GetGameLog().empty());
    EXPECT_EQ(GetGameLog().back(), "MoveForward (killed), Shoot (ignored), RotateLeft90");
}

TEST_F(GameManagerTest, LogAction_OrderMatchesTanksOnBoard) {
    // Arrange: Create tanks in specific order
    // Tanks in order: (2,0), (0,0), (1,0)
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(2, 0)}, {2, Point(0, 0)}, {1, Point(1, 0)}
    };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controllers = GetTankControllers();
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[2].nextAction = ActionRequest::RotateLeft90;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = true;
    controllers[2].actionSuccess = true;
    
    // Act
    LogAction();
    
    // Assert: Verify log order matches tank controller order
    ASSERT_FALSE(GetGameLog().empty());
    // The log should match the order of tanks as found in m_tankControllers
    EXPECT_EQ(GetGameLog().back(), "MoveForward, Shoot, RotateLeft90");
}

// ===================================================================== //
// Game Flow Control
// ===================================================================== //
// 1. checkGameOver() - Test all win/loss/tie conditions
// 2. processStep() - Test single step processing (integration of multiple components)
// ===================================================================== //
TEST_F(GameManagerTest, CheckGameOver_Player1Wins) {
    // Arrange: Create tanks only for player 1 using utilities
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {1, Point(1,0)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    // Act
    bool over = CallCheckGameOver();
    
    // Assert: Game should end with player 1 victory
    EXPECT_TRUE(over);
    EXPECT_EQ(GetGameResult(), "Player 1 won with 2 tanks still alive");
}

TEST_F(GameManagerTest, CheckGameOver_Player2Wins) {
    // Arrange: Create tanks only for player 2 using utilities
    std::vector<std::pair<int, Point>> positions = { {2, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    // Act
    bool over = CallCheckGameOver();
    
    // Assert: Game should end with player 2 victory
    EXPECT_TRUE(over);
    EXPECT_EQ(GetGameResult(), "Player 2 won with 2 tanks still alive");
}

TEST_F(GameManagerTest, CheckGameOver_TieZeroTanks) {
    // Arrange: Create tanks for both players and destroy them all
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    SetIsClassic2PlayerGame(true); // Set for test environment
    
    // Destroy all tanks using utilities
    auto& tanks = GetTanks();
    for (auto& t : tanks) {
        t.destroy();
    }
    
    // Act
    bool over = CallCheckGameOver();
    
    // Assert: Game should end with tie (zero tanks)
    EXPECT_TRUE(over);
    EXPECT_EQ(GetGameResult(), "Tie, both players have zero tanks");
}

TEST_F(GameManagerTest, CheckGameOver_TieMaxSteps) {
    // Arrange: Create tanks for both players and reach max steps
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    SetIsClassic2PlayerGame(true); // Set for test environment
    SetMaxSteps(5);
    SetCurrentStep(5);
    
    // Act
    bool over = CallCheckGameOver();
    
    // Assert: Game should end with tie (max steps reached)
    EXPECT_TRUE(over);
    EXPECT_EQ(GetGameResult(), "Tie, reached max steps = 5, player 1 has 1 tanks, player 2 has 1 tanks");
}

TEST_F(GameManagerTest, CheckGameOver_GameContinues) {
    // Arrange: Create tanks for both players with game not yet over
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    SetMaxSteps(10);
    SetCurrentStep(3);
    
    // Act
    bool over = CallCheckGameOver();
    
    // Assert: Game should continue (not over yet)
    EXPECT_FALSE(over);
}

TEST_F(GameManagerTest, CheckGameOver_TieZeroShellsForExactly40Steps) {
    // Arrange: Create tanks for both players with zero shells
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    SetIsClassic2PlayerGame(true); // Set for test environment
    
    // Drain all shells from tanks using utilities
    auto& tanks = GetTanks();
    for (auto& tank : tanks) {
        for (int i = 0; i < Tank::INITIAL_SHELLS; i++) {
            tank.decrementShells();
        }
        EXPECT_EQ(tank.getRemainingShells(), 0);
    }
    
    // Set remaining steps to exactly 0 (the condition for tie)
    SetRemainingSteps(0);
    SetMaxSteps(1000); // High enough to not trigger max steps condition
    SetCurrentStep(100); // Far from max steps
    
    // Act: Check if game ends when remaining steps reaches 0
    bool over = CallCheckGameOver();
    
    // Assert: Game should end with tie message mentioning exactly 40 steps
    EXPECT_TRUE(over);
    EXPECT_EQ(GetGameResult(), "Tie, both players have zero shells for " + std::to_string(GameManager::DEFAULT_NO_SHELLS_STEPS) + " steps");
}

TEST_F(GameManagerTest, CheckGameOver_ZeroShellsButStillRemaining39Steps) {
    // Arrange: Create tanks for both players with zero shells
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    // Drain all shells from tanks using utilities
    auto& tanks = GetTanks();
    for (auto& tank : tanks) {
        for (int i = 0; i < Tank::INITIAL_SHELLS; i++) {
            tank.decrementShells();
        }
        EXPECT_EQ(tank.getRemainingShells(), 0);
    }
    
    // Set remaining steps to 1 (still has steps left before 40 step limit)
    SetRemainingSteps(1);
    SetMaxSteps(1000); // High enough to not trigger max steps condition
    SetCurrentStep(100); // Far from max steps
    
    // Act: Check if game continues when still has remaining steps
    bool over = CallCheckGameOver();
    
    // Assert: Game should continue
    EXPECT_FALSE(over);
}

TEST_F(GameManagerTest, ProcessStep_OneTankMoves_UpdatesPositionAndLogs) {
    // Arrange: Create tank with mock algorithm
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.setDirection(Direction::Right);
    auto* mockAlgo = dynamic_cast<EnhancedMockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::MoveForward);
    
    // Act
    CallProcessStep();
    
    // Assert: Tank should move and log the action
    EXPECT_EQ(controller.tank.getPosition(), Point(2, 1));
    EXPECT_EQ(GetGameLog().back(), "MoveForward");
}

TEST_F(GameManagerTest, ProcessStep_OneTankShoots_AddsShellAndLogs) {
    // Arrange: Create tank with mock algorithm for shooting
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.setDirection(Direction::Down);
    auto* mockAlgo = dynamic_cast<EnhancedMockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::Shoot);
    int shellsBefore = GetShells().size();
    
    // Act
    CallProcessStep();
    
    // Assert: Shell should be added and action logged
    int shellsAfter = GetShells().size();
    EXPECT_EQ(shellsAfter, shellsBefore + 1);
    EXPECT_EQ(GetShells().back().getPosition(), Point(1, 2));
    EXPECT_EQ(GetShells().back().getDirection(), Direction::Down);
    EXPECT_EQ(GetGameLog().back(), "Shoot");
}

TEST_F(GameManagerTest, ProcessStep_ShellHitsTank_TankDestroyedAndLogKilled) {
    // Arrange: Create two tanks where one shoots the other
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)}, {2, Point(3, 1)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& shooter = GetTankControllers()[0];
    auto& target = GetTankControllers()[1];
    shooter.tank.setDirection(Direction::Right);
    auto* mockAlgoShooter = dynamic_cast<EnhancedMockAlgorithm*>(shooter.algorithm.get());
    auto* mockAlgoTarget = dynamic_cast<EnhancedMockAlgorithm*>(target.algorithm.get());
    ASSERT_NE(mockAlgoShooter, nullptr);
    ASSERT_NE(mockAlgoTarget, nullptr);
    mockAlgoShooter->setActionSequence({ActionRequest::Shoot, ActionRequest::DoNothing});
    mockAlgoTarget->setConstantAction(ActionRequest::DoNothing);
    
    // Step 1: shoot, shell moves 4 cells right (from (1,1) to (5,1) with wrap)
    CallProcessStep();
    // Target not yet destroyed
    EXPECT_FALSE(target.tank.isDestroyed());
    
    // Step 2: shell moves again, should hit the target at (5,1)
    CallProcessStep();
    EXPECT_TRUE(target.tank.isDestroyed());
    // Log should show DoNothing (killed) for target
    EXPECT_EQ(GetGameLog().back(), "DoNothing, DoNothing (killed)");
}

TEST_F(GameManagerTest, ProcessStep_TankOnCooldown_ShootIgnoredAndLogs) {
    // Arrange: Create tank on cooldown trying to shoot
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.setDirection(Direction::Right);
    auto* mockAlgo = dynamic_cast<EnhancedMockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::Shoot);
    // Simulate tank just shot (cooldown active)
    controller.tank.shoot();
    
    // Act
    CallProcessStep();
    
    // Assert: Shoot should be ignored due to cooldown
    EXPECT_EQ(GetGameLog().back(), "Shoot (ignored)");
}

TEST_F(GameManagerTest, ProcessStep_TankKilledThisStep_LogsKilledThisStep) {
    // Arrange: Create two tanks where one shoots and kills the other immediately
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)}, {2, Point(2, 1)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& shooter = GetTankControllers()[0];
    auto& target = GetTankControllers()[1];
    shooter.tank.setDirection(Direction::Right);
    auto* mockAlgoShooter = dynamic_cast<EnhancedMockAlgorithm*>(shooter.algorithm.get());
    auto* mockAlgoTarget = dynamic_cast<EnhancedMockAlgorithm*>(target.algorithm.get());
    ASSERT_NE(mockAlgoShooter, nullptr);
    ASSERT_NE(mockAlgoTarget, nullptr);
    mockAlgoShooter->setConstantAction(ActionRequest::Shoot);
    mockAlgoTarget->setConstantAction(ActionRequest::DoNothing);
    
    // Step 1: shoot, shell moves 4 cells right (from (1,1) to (5,1)), hits target at (5,1)
    CallProcessStep();
    EXPECT_TRUE(target.tank.isDestroyed());
    EXPECT_EQ(GetGameLog().back(), "Shoot, DoNothing (killed)");
}

TEST_F(GameManagerTest, ProcessStep_AlreadyDeadTank_LogsKilled) {
    // Arrange: Create already dead tank from previous step
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.destroy();
    controller.wasKilledInPreviousStep = true;
    auto* mockAlgo = dynamic_cast<EnhancedMockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::MoveForward);
    
    // Act
    CallProcessStep();
    
    // Assert: Should log as "Killed" since tank was already dead
    EXPECT_EQ(GetGameLog().back(), "Killed");
}

TEST_F(GameManagerTest, ProcessStep_MixedActions_LogsAllScenarios) {
    // Arrange: Create three tanks with mixed scenarios (killed, normal, killed)
    std::vector<std::pair<int, Point>> positions = {
        {2, Point(0, 0)}, {2, Point(1, 0)}, {2, Point(2, 0)}
    };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& c0 = GetTankControllers()[0];
    auto& c1 = GetTankControllers()[1];
    auto& c2 = GetTankControllers()[2];
    auto* mockAlgo0 = dynamic_cast<EnhancedMockAlgorithm*>(c0.algorithm.get());
    auto* mockAlgo1 = dynamic_cast<EnhancedMockAlgorithm*>(c1.algorithm.get());
    auto* mockAlgo2 = dynamic_cast<EnhancedMockAlgorithm*>(c2.algorithm.get());
    ASSERT_NE(mockAlgo0, nullptr);
    ASSERT_NE(mockAlgo1, nullptr);
    ASSERT_NE(mockAlgo2, nullptr);
    mockAlgo0->setConstantAction(ActionRequest::MoveForward);
    mockAlgo1->setConstantAction(ActionRequest::Shoot);
    mockAlgo2->setConstantAction(ActionRequest::RotateLeft90);
    c0.tank.destroy();
    c0.wasKilledInPreviousStep = false;
    
    // Act
    CallProcessStep();
    
    // Assert: Should log mixed scenario with killed tank
    EXPECT_EQ(GetGameLog().back(), "DoNothing (killed), Shoot, RotateLeft90 (killed)");
}

TEST_F(GameManagerTest, ProcessStep_AllShellsDestroyed_RemovesShells) {
    // Arrange: Add two destroyed shells using utilities
    Shell shell1 = GameObjectUtilities::createDestroyedShell(1, Point(1, 1), Direction::Right);
    Shell shell2 = GameObjectUtilities::createDestroyedShell(2, Point(2, 2), Direction::Left);
    auto& shells = GetShells();
    shells.push_back(shell1);
    shells.push_back(shell2);
    
    // Add a tank so processStep can run
    std::vector<std::pair<int, Point>> positions = { {1, Point(0, 0)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    auto* mockAlgo = dynamic_cast<EnhancedMockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::DoNothing);
    
    // Act
    CallProcessStep();
    
    // Assert: All destroyed shells should be removed
    EXPECT_TRUE(GetShells().empty());
}

TEST_F(GameManagerTest, ProcessStep_MoveBackward_OnlyMovesOnThirdStep) {
    // Arrange: Create tank that will move backward (3-step delay mechanic)
    std::vector<std::pair<int, Point>> positions = { {1, Point(2, 2)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.setDirection(Direction::Up);
    auto* mockAlgo = dynamic_cast<EnhancedMockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::MoveBackward);
    
    // Step 1: Request backward movement (starts counter)
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), Point(2, 2));
    EXPECT_EQ(GetGameLog().back(), "MoveBackward");
    
    // Step 2: Still waiting to move
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), Point(2, 2));
    EXPECT_EQ(GetGameLog().back(), "MoveBackward (ignored)");
    
    // Step 3: Finally executes backward movement
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), Point(2, 3));
    EXPECT_EQ(GetGameLog().back(), "MoveBackward (ignored)");
    
    // Step 4: Continuous backward movement
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), Point(2, 4));
    EXPECT_EQ(GetGameLog().back(), "MoveBackward");
}

TEST_F(GameManagerTest, ProcessStep_MoveForward_cancelMoveBackward) {
    // Arrange: Create tank that will request backward then forward (cancellation test)
    Point initialPosition = Point(2, 2);
    std::vector<std::pair<int, Point>> positions = { {1, initialPosition} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.setDirection(Direction::Up);
    auto* mockAlgo = dynamic_cast<EnhancedMockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setActionSequence({ActionRequest::MoveBackward, ActionRequest::MoveForward, ActionRequest::MoveForward});
    
    // Step 1: Request backward movement
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), initialPosition);
    EXPECT_EQ(GetGameLog().back(), "MoveBackward");
    
    // Step 2: Forward movement cancels backward
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), initialPosition);
    EXPECT_EQ(GetGameLog().back(), "MoveForward");
    
    // Step 3: Normal forward movement
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), Point(2, 1));
    EXPECT_EQ(GetGameLog().back(), "MoveForward");
}

TEST_F(GameManagerTest, ProcessStep_Shoot_CooldownResetsAfter4Steps) {
    // Arrange: Create tank on large board to test cooldown mechanics
    std::vector<std::pair<int, Point>> positions = { {1, Point(0, 0)} };
    GetBoard() = GameBoard(20, 20);

    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.setDirection(Direction::Right);
    auto* mockAlgo = dynamic_cast<EnhancedMockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::Shoot);
    
    // Step 1: Shoot (should succeed)
    CallProcessStep();
    EXPECT_EQ(GetGameLog().back(), "Shoot");
    
    // Steps 2-4: Shoot (should be ignored due to cooldown)
    for (int i = 0; i < Tank::SHOOT_COOLDOWN; ++i) {
        CallProcessStep();
        EXPECT_EQ(GetGameLog().back(), "Shoot (ignored)");
    }
    
    // Step 5: Shoot (should succeed again)
    CallProcessStep();
    EXPECT_EQ(GetGameLog().back(), "Shoot");
}

TEST_F(GameManagerTest, ProcessStep_Shoot_CannotShootMoreThanMaxShells) {
    // Arrange: Create tank and test shell exhaustion
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    int maxShells = Tank::INITIAL_SHELLS;
    int shootCooldown = Tank::SHOOT_COOLDOWN;
    GetBoard() = GameBoard(2 * maxShells * (shootCooldown + 1), 1);

    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.setDirection(Direction::Right);
    auto* mockAlgo = dynamic_cast<EnhancedMockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::Shoot);
    
    // Shoot until all shells are exhausted
    for (int i = 0; i < maxShells * (shootCooldown + 1); ++i) {
        CallProcessStep();
        if (i % (shootCooldown + 1) == 0) {
            EXPECT_EQ(GetGameLog().back(), "Shoot");
        } else {
            EXPECT_EQ(GetGameLog().back(), "Shoot (ignored)");
        }
    }
    
    // After exhausting all shells, shoot should be ignored
    CallProcessStep();
    EXPECT_EQ(GetGameLog().back(), "Shoot (ignored)");
}

TEST_F(GameManagerTest, ProcessStep_GetBattleInfoIgnoredDuringBackwardMovement) {
    // Arrange: Create tank that will test GetBattleInfo during backward movement
    std::vector<std::pair<int, Point>> positions = { {1, Point(2, 2)} };
    CreateTanks(positions);
    CreateTankAlgorithms();
    
    auto& controller = GetTankControllers()[0];
    controller.tank.setDirection(Direction::Up);
    
    // Setup mock algorithm to request backward movement then GetBattleInfo
    auto* mockAlgo = dynamic_cast<EnhancedMockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    
    std::vector<ActionRequest> actions = {
        ActionRequest::MoveBackward,    // Step 1: Request backward movement
        ActionRequest::GetBattleInfo,   // Step 2: Try to get battle info (should be ignored)
        ActionRequest::GetBattleInfo,   // Step 3: Try again (should be ignored, backward executes)
        ActionRequest::DoNothing        // Step 4: Do nothing
    };
    mockAlgo->setActionSequence(actions);
    
    // Track initial state
    Point initialPos = controller.tank.getPosition();
    
    // Step 1: Request backward movement
    CallProcessStep();
    EXPECT_TRUE(controller.tank.isMovingBackward());
    EXPECT_EQ(controller.tank.getBackwardCounter(), 1);
    EXPECT_EQ(controller.tank.getPosition(), initialPos); // No movement yet
    EXPECT_TRUE(controller.actionSuccess); // Backward request accepted
    
    // Step 2: Try GetBattleInfo during backward movement (should be ignored)
    CallProcessStep();
    EXPECT_TRUE(controller.tank.isMovingBackward());
    EXPECT_EQ(controller.tank.getBackwardCounter(), 2);
    EXPECT_EQ(controller.tank.getPosition(), initialPos); // Still no movement
    EXPECT_FALSE(controller.actionSuccess); // GetBattleInfo was ignored
    
    // Step 3: Try GetBattleInfo again (should be ignored, backward should execute)
    CallProcessStep();
    EXPECT_FALSE(controller.tank.isMovingBackward()); // Backward movement completed
    EXPECT_EQ(controller.tank.getBackwardCounter(), 0);
    
    // Tank should have moved backward (down one position since facing Up)
    Point expectedPos = Point(initialPos.getX(), initialPos.getY() + 1);
    EXPECT_EQ(controller.tank.getPosition(), expectedPos);
    EXPECT_FALSE(controller.actionSuccess); // GetBattleInfo was ignored
    
    // Verify the logs show the ignored actions
    auto& gameLog = GetGameLog();
    EXPECT_GE(gameLog.size(), 3u);
    EXPECT_EQ(gameLog[gameLog.size()-3], "MoveBackward"); // Step 1
    EXPECT_EQ(gameLog[gameLog.size()-2], "GetBattleInfo (ignored)"); // Step 2
    EXPECT_EQ(gameLog[gameLog.size()-1], "GetBattleInfo (ignored)"); // Step 3
}

// ===================================================================== //
// Output and Results
// ===================================================================== //
// 1. saveResults(const std::string& outputFilePath) - Test file output functionality
// ===================================================================== //

// TEST_F(GameManagerTest, SaveResults_WritesAllLogLines) {
//     std::string testFile = "test_output.txt";
//     // Fill log with known lines
//     GetGameLog(*manager).clear();
//     GetGameLog(*manager).push_back("Step 1 completed");
//     GetGameLog(*manager).push_back("Step 2 completed");
//     GetGameLog(*manager).push_back("Game ended after 2 steps");
//     GetGameLog(*manager).push_back("Result: Player 1 won with 1 tanks still alive");

//     // Call saveResults
//     CallSaveResults(testFile);

//     // Read file back
//     std::ifstream in(testFile);
//     ASSERT_TRUE(in.is_open());
//     std::vector<std::string> lines;
//     std::string line;
//     while (std::getline(in, line)) {
//         lines.push_back(line);
//     }
//     in.close();

//     // Check all lines
//     ASSERT_EQ(lines.size(), 4u);
//     EXPECT_EQ(lines[0], "Step 1 completed");
//     EXPECT_EQ(lines[1], "Step 2 completed");
//     EXPECT_EQ(lines[2], "Game ended after 2 steps");
//     EXPECT_EQ(lines[3], "Result: Player 1 won with 1 tanks still alive");

//     // Clean up
//     std::remove(testFile.c_str());
// }

// ===================================================================== //
// Full Integration
// ===================================================================== //
// 1. run(...)
// ===================================================================== //

TEST_F(GameManagerTest, Run_DoesNotExceedMaximumSteps_Integration) {
    // Arrange: Create scenario with two tanks and small max steps using GameScenarioBuilder
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(3, 3)
            .withTwoPlayerSetup(Point(0, 0), Point(2, 0))
            .withMaxSteps(5) // Set a small maximum for the test
            .withNumShells(10)
            .build();

    // Act: Run the game with new API
    GameResult result = manager->run(
        width, height,
        satelliteView,
        "test_map",
        maxSteps, numShells,
        *player1, "TestPlayer1",
        *player2, "TestPlayer2",
        mockFactoryDoNothing, mockFactoryDoNothing
    );

    // Assert: Game should end due to max steps and be a tie
    EXPECT_EQ(result.winner, 0); // Tie
    EXPECT_EQ(result.reason, GameResult::Reason::MAX_STEPS);
    EXPECT_EQ(result.remaining_tanks.size(), 2u);
    EXPECT_EQ(result.remaining_tanks[0], 1u); // Player 1 has 1 tank remaining
    EXPECT_EQ(result.remaining_tanks[1], 1u); // Player 2 has 1 tank remaining
}

TEST_F(GameManagerTest, Run_TieAfter40StepsWithZeroShells_Integration) {
    // Arrange: Create scenario with tanks that will shoot at walls until out of shells
    size_t numShells = 5; // Use fewer shells for faster test
    size_t boardWidth = 20;
    size_t boardHeight = 20;
    
    // Create walls in front of each tank to absorb all their shots
    std::vector<Point> wallPositions;
    // Walls in front of tank 1 (at 19,19 facing Left)
    for (int i = 0; i < 16; ++i) {
        wallPositions.push_back(Point(18-i, 19));
    }
    // Walls in front of tank 2 (at 0,0 facing Right)
    for (int i = 0; i < 16; ++i) {
        wallPositions.push_back(Point(1+i, 0));
    }
    
    auto [satelliteView, player1, player2, width, height, maxSteps, _] = 
        GameScenarioBuilder()
            .withBoardSize(boardWidth, boardHeight)
            .withTwoPlayerSetup(Point(19, 19), Point(0, 0))
            .withWalls(wallPositions)
            .withNumShells(numShells)
            .withMaxSteps(1000) // High max steps to ensure zero shells condition triggers first
            .build();

    // Act: Run the game with shooting algorithms
    GameResult result = manager->run(
        width, height,
        satelliteView,
        "test_map",
        maxSteps, numShells,
        *player1, "TestPlayer1",
        *player2, "TestPlayer2",
        mockFactoryShooter, mockFactoryShooter
    );

    // Assert: Game should end with zero shells tie condition
    EXPECT_EQ(result.winner, 0); // Tie
    EXPECT_EQ(result.reason, GameResult::Reason::ZERO_SHELLS);
    EXPECT_EQ(result.remaining_tanks.size(), 2u);
    EXPECT_EQ(result.remaining_tanks[0], 1u); // Player 1 has 1 tank remaining
    EXPECT_EQ(result.remaining_tanks[1], 1u); // Player 2 has 1 tank remaining
    
    // Verify both tanks are still alive (not destroyed)
    EXPECT_GT(result.remaining_tanks[0], 0u);
    EXPECT_GT(result.remaining_tanks[1], 0u);
}

// ===================================================================== //
// Verbose Output Functionality
// ===================================================================== //
// Test that verbose flag controls file output behavior
// ===================================================================== //

TEST_F(GameManagerTest, Run_VerboseFalse_NoFileOutput) {
    // Arrange: Create verbose=false manager
    manager = std::make_unique<GameManager>(false);
    EXPECT_FALSE(GetVerbose());
    
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(3, 3)
            .withTwoPlayerSetup(Point(0, 0), Point(2, 0))
            .withMaxSteps(5)
            .withNumShells(10)
            .build();

    // Act: Run the game (should not create output files)
    GameResult result = manager->run(
        width, height,
        satelliteView,
        "test_map",
        maxSteps, numShells,
        *player1, "TestPlayer1",
        *player2, "TestPlayer2",
        mockFactoryDoNothing, mockFactoryDoNothing
    );
    
    // Assert: Game should complete normally but not create output files
    EXPECT_EQ(result.winner, 0); // Tie due to max steps
    EXPECT_EQ(result.reason, GameResult::Reason::MAX_STEPS);
    
    // Verify no new game_*.txt files were created
    std::vector<std::string> newFiles = fileHelper->getNewGameFiles();
    EXPECT_TRUE(newFiles.empty()) << "Expected no files, but found: " << 
        (newFiles.empty() ? "none" : newFiles[0]);
}

TEST_F(GameManagerTest, Run_VerboseTrue_CreatesFileOutput) {
    // Arrange: Create verbose=true manager  
    manager = std::make_unique<GameManager>(true);
    EXPECT_TRUE(GetVerbose());
    
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(3, 3)
            .withTwoPlayerSetup(Point(0, 0), Point(2, 0))
            .withMaxSteps(5)
            .withNumShells(10)
            .build();

    // Act: Run the game (should create output files)
    GameResult result = manager->run(
        width, height,
        satelliteView,
        "test_map",
        maxSteps, numShells,
        *player1, "TestPlayer1",
        *player2, "TestPlayer2",
        mockFactoryDoNothing, mockFactoryDoNothing
    );
    
    // Assert: Game should complete normally and create output files
    EXPECT_EQ(result.winner, 0); // Tie due to max steps
    EXPECT_EQ(result.reason, GameResult::Reason::MAX_STEPS);
    
    // Verify exactly one new game_*.txt file was created
    std::vector<std::string> newFiles = fileHelper->getNewGameFiles();
    ASSERT_EQ(newFiles.size(), 1u) << "Expected exactly 1 file, but found " << newFiles.size();
    
    // Verify the filename format includes player class names and timestamp
    std::string filename = newFiles[0];
    
    // Get the actual player class names using typeid (should match what GameManager uses)
    std::string player1Name = typeid(*player1).name();
    std::string player2Name = typeid(*player2).name();
    
    // Clean the names the same way GameManager does
    player1Name = FileTestHelper::cleanFilename(player1Name);
    player2Name = FileTestHelper::cleanFilename(player2Name);
    
    // Verify filename format: game_[player1]_vs_[player2]_[timestamp].txt
    std::string expectedPrefix = "game_" + player1Name + "_vs_" + player2Name + "_";
    EXPECT_TRUE(FileTestHelper::startsWith(filename, expectedPrefix)) << 
        "Expected filename to start with '" << expectedPrefix << "', but got: " << filename;
    EXPECT_TRUE(FileTestHelper::endsWith(filename, ".txt")) << 
        "Expected filename to end with '.txt', but got: " << filename;
    
    // Verify the file exists and has content
    std::ifstream file(filename);
    EXPECT_TRUE(file.is_open()) << "Could not open file: " << filename;
    std::string line;
    bool hasContent = false;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            hasContent = true;
            break;
        }
    }
    EXPECT_TRUE(hasContent) << "Expected file to have game log content";
}

} // namespace GameManager_098765432_123456789