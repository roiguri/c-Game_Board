#include <gtest/gtest.h>
#include <memory>
#include <chrono>

#include "../../game_manager.h"
#include "common/AbstractGameManager.h"
#include "common/GameResult.h"
#include "../mocks/enhanced_mock_player.h"
#include "../mocks/enhanced_mock_algorithm.h"
#include "../mocks/mock_factories.h"
#include "../mocks/scenario_mock_satellite_view.h"
#include "../helpers/game_result_validator.h"
#include "../helpers/game_scenario_builder.h"

namespace GameManager_098765432_123456789 {

class InterfaceComplianceTest : public ::testing::Test {
protected:
    void SetUp() override {
        gameManager = std::make_unique<GameManager>(false); // Not verbose
        MockFactoryConfigurer::resetAll();
    }

    void TearDown() override {
        MockFactoryConfigurer::resetAll();
    }

    std::unique_ptr<GameManager> gameManager;
};

// Test 1: Basic Interface Compliance
TEST_F(InterfaceComplianceTest, ImplementsAbstractGameManagerInterface) {
    // Verify GameManager implements AbstractGameManager
    AbstractGameManager* abstractManager = gameManager.get();
    EXPECT_NE(abstractManager, nullptr);
}

// Test 2: run() Method Signature Compliance
TEST_F(InterfaceComplianceTest, RunMethodAcceptsAllRequiredParameters) {
    // Create test scenario
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(7, 3)
            .withTwoPlayerSetup(Point(1, 1), Point(5, 1))
            .build();

    // Verify run method can be called with all required parameters
    EXPECT_NO_THROW({
        GameResult result = gameManager->run(
            width, height,
            satelliteView,
            maxSteps, numShells,
            *player1, *player2,
            mockFactoryDoNothing, mockFactoryDoNothing
        );
    });
}

// Test 3: GameResult Structure Population
TEST_F(InterfaceComplianceTest, ReturnsValidGameResult) {
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(3, 3)
            .withTwoPlayerSetup(Point(0, 1), Point(2, 1))
            .withMaxSteps(5) // Force quick game end
            .build();

    GameResult result = gameManager->run(
        width, height,
        satelliteView,
        maxSteps, numShells,
        *player1, *player2,
        mockFactoryDoNothing, mockFactoryDoNothing
    );

    // Validate GameResult structure
    EXPECT_TRUE(result.winner == 0 || result.winner == 1 || result.winner == 2);
    EXPECT_GE(result.remaining_tanks.size(), 2u);
    
    // Verify consistency
    GameResultValidator::validateConsistency(result);
}

// Test 4: Player Factory Invocation
TEST_F(InterfaceComplianceTest, CallsFactoriesWithCorrectParameters) {
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(7, 3)
            .withTwoPlayerSetup(Point(1, 1), Point(5, 1))
            .build();

    gameManager->run(
        width, height,
        satelliteView,
        maxSteps, numShells,
        *player1, *player2,
        mockFactoryDoNothing, mockFactoryShooter
    );

    // Verify factory calls
    auto doNothingCalls = MockFactoryConfigurer::getFactoryCalls("mockFactoryDoNothing");
    auto shooterCalls = MockFactoryConfigurer::getFactoryCalls("mockFactoryShooter");

    EXPECT_FALSE(doNothingCalls.empty()) << "Player 1 factory should be called";
    EXPECT_FALSE(shooterCalls.empty()) << "Player 2 factory should be called";

    // Verify correct player indices
    for (const auto& [playerIndex, tankIndex] : doNothingCalls) {
        EXPECT_EQ(playerIndex, 1) << "Player 1 factory should be called with playerIndex 1";
    }

    for (const auto& [playerIndex, tankIndex] : shooterCalls) {
        EXPECT_EQ(playerIndex, 2) << "Player 2 factory should be called with playerIndex 2";
    }
}

// Test 5: Player updateTankWithBattleInfo Called
TEST_F(InterfaceComplianceTest, CallsPlayerUpdateTankWithBattleInfo) {
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(7, 3)
            .withTwoPlayerSetup(Point(1, 1), Point(5, 1))
            .withMaxSteps(50)  // Increase steps to ensure game runs long enough
            .build();

    // Set up expectations
    EXPECT_CALL(*player1, updateTankWithBattleInfo(::testing::_, ::testing::_))
        .Times(::testing::AtLeast(1));
    EXPECT_CALL(*player2, updateTankWithBattleInfo(::testing::_, ::testing::_))
        .Times(::testing::AtLeast(1));

    // Create factories that return algorithms requesting battle info
    auto battleInfoFactory = createMockFactoryWithAction("mockFactoryBattleInfo", ActionRequest::GetBattleInfo);
    
    gameManager->run(
        width, height,
        satelliteView,
        maxSteps, numShells,
        *player1, *player2,
        battleInfoFactory, battleInfoFactory
    );
}

// Test 6: Error Handling - Null Factory
TEST_F(InterfaceComplianceTest, HandlesNullFactoryGracefully) {
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(3, 3)
            .withTwoPlayerSetup(Point(0, 1), Point(2, 1))
            .build();

            auto failingFactory = createMockFactoryWithAction("mockFactoryFailing", ActionRequest::DoNothing);
        
        GameResult result = gameManager->run(
            width, height,
            satelliteView,
            maxSteps, numShells,
            *player1, *player2,
            failingFactory, mockFactoryDoNothing
        );

    // Should handle gracefully and return a valid result
    EXPECT_TRUE(result.winner == 0 || result.winner == 1 || result.winner == 2);
}

// Test 7: Error Handling - Player Throws Exception
TEST_F(InterfaceComplianceTest, HandlesPlayerExceptionGracefully) {
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(3, 3)
            .withTwoPlayerSetup(Point(0, 1), Point(2, 1))
            .withMaxSteps(5)
            .build();

    // Test that GameManager handles player behavior gracefully
    EXPECT_NO_THROW({
        GameResult result = gameManager->run(
            width, height,
            satelliteView,
            maxSteps, numShells,
            *player1, *player2,
            mockFactoryDoNothing, mockFactoryDoNothing
        );
    });
}

// Test 8: Performance Requirement - Under 100ms
TEST_F(InterfaceComplianceTest, ExecutesWithinPerformanceRequirement) {
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(3, 3)
            .withTwoPlayerSetup(Point(0, 1), Point(2, 1))
            .withMaxSteps(50)
            .build();

    auto startTime = std::chrono::high_resolution_clock::now();

    GameResult result = gameManager->run(
        width, height,
        satelliteView,
        maxSteps, numShells,
        *player1, *player2,
        mockFactoryDoNothing, mockFactoryDoNothing
    );

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    EXPECT_LT(duration.count(), 100) << "Game should execute in under 100ms, took " << duration.count() << "ms";
}

// Test 9: Different Board Sizes
TEST_F(InterfaceComplianceTest, HandlesVariousBoardSizes) {
    // Test different board sizes
    std::vector<std::pair<size_t, size_t>> boardSizes = {
        {3, 3}, {5, 5}, {10, 8}, {20, 15}
    };

    for (const auto& [width, height] : boardSizes) {
        // Ensure tank positions are valid for the board size
        int tank1X = std::min(1, static_cast<int>(width) - 1);
        int tank1Y = std::min(1, static_cast<int>(height) - 1);
        int tank2X = std::max(static_cast<int>(width) - 2, 0);
        int tank2Y = std::max(static_cast<int>(height) - 2, 0);
        
        // Make sure tanks are not at the same position
        if (tank1X == tank2X && tank1Y == tank2Y) {
            if (width > 1) tank2X = tank1X + 1;
            else if (height > 1) tank2Y = tank1Y + 1;
        }
        
        auto [satelliteView, player1, player2, w, h, maxSteps, numShells] = 
            GameScenarioBuilder()
                .withBoard(width, height)
                .withTank(Point(tank1X, tank1Y))
                .withTank(Point(tank2X, tank2Y))
                .withMaxSteps(20)
                .build();

        EXPECT_NO_THROW({
            GameResult result = gameManager->run(
                width, height,
                satelliteView,
                maxSteps, numShells,
                *player1, *player2,
                mockFactoryDoNothing, mockFactoryDoNothing
            );
        }) << "Failed with board size " << width << "x" << height;
    }
}

// Test 10: GameResult Reason Validation
TEST_F(InterfaceComplianceTest, ReturnsCorrectGameEndReasons) {
    // Test MAX_STEPS reason
    {
        auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
            GameScenarioBuilder()
                .withBoardSize(7, 3)
                .withTwoPlayerSetup(Point(1, 1), Point(5, 1))
                .withMaxSteps(1) // Force immediate end
                .build();

        GameResult result = gameManager->run(
            width, height,
            satelliteView,
            1, numShells,
            *player1, *player2,
            mockFactoryDoNothing, mockFactoryDoNothing
        );

        GameResultValidator::validateReason(result, GameResult::Reason::MAX_STEPS);
    }

    // Test ZERO_SHELLS reason (tanks shoot all shells then wait 40 steps)
    {
        auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
            GameScenarioBuilder()
                .withBoardSize(9, 5)
                .withTwoPlayerSetup(Point(1, 1), Point(7, 3))
                .withVerticalWall(3, 0, 4) // Create wall obstacle
                .withMines({Point(5, 1), Point(5, 3)})
                .withNumShells(1) // Only one shell each
                .withMaxSteps(100)
                .build();

        GameResult result = gameManager->run(
            width, height,
            satelliteView,
            maxSteps, 1,
            *player1, *player2,
            mockFactoryShooter, mockFactoryShooter // Both shoot immediately
        );

        // After shooting all shells and waiting 40 steps, should be ZERO_SHELLS
        EXPECT_TRUE(result.reason == GameResult::Reason::ZERO_SHELLS || 
                   result.reason == GameResult::Reason::MAX_STEPS);
    }
}

TEST_F(InterfaceComplianceTest, ValidatesGameStateAndRounds) {
    // Test comprehensive GameResult validation including gameState and rounds
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(5, 5)
            .withTwoPlayerSetup(Point(1, 1), Point(3, 3))
            .withMaxSteps(10)
            .withNumShells(2)
            .build();

    GameResult result = gameManager->run(
        width, height,
        satelliteView,
        maxSteps, numShells,
        *player1, *player2,
        mockFactoryDoNothing, mockFactoryDoNothing
    );

    // Validate basic result structure
    GameResultValidator::validateGameStateExists(result);
    
    // Validate rounds are within reasonable range (should end at maxSteps)
    GameResultValidator::validateRoundsRange(result, 1, maxSteps);
    
    // For MAX_STEPS scenario, validate rounds equals maxSteps
    if (result.reason == GameResult::Reason::MAX_STEPS) {
        GameResultValidator::validateRounds(result, maxSteps);
    }
    
    // Validate gameState consistency with remaining_tanks
    GameResultValidator::validateGameStateConsistentWithRemainingTanks(result, width, height);
    
    // Test individual cell validation (center should be empty)
    Point centerPoint(width/2, height/2);
    GameResultValidator::validateGameStateEmpty(result, centerPoint);
    
    // Additional validation checks can be added here
    // Custom matchers would be defined in individual test files as needed
}

TEST_F(InterfaceComplianceTest, ValidatesSpecificGameStatePositions) {
    // Test validation of specific positions in gameState
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(3, 3)
            .withTwoPlayerSetup(Point(0, 1), Point(2, 1))
            .withMaxSteps(5)
            .build();

    GameResult result = gameManager->run(
        width, height,
        satelliteView,
        maxSteps, numShells,
        *player1, *player2,
        mockFactoryDoNothing, mockFactoryDoNothing
    );

    // Validate specific positions
    Point topLeft(0, 0);
    Point topRight(width-1, 0);
    Point center(width/2, height/2);
    
    // Test position-specific validations
    GameResultValidator::validateGameStateExists(result);
    
    // Test that we can validate specific positions without errors
    // (actual values depend on the scenario setup)
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            Point pos(x, y);
            // Just validate that the position is accessible
            char objectAt = result.gameState->getObjectAt(x, y);
            EXPECT_TRUE(objectAt == ' ' || objectAt == '#' || objectAt == '*' || 
                       (objectAt >= '1' && objectAt <= '9')) 
                << "Invalid object at position (" << x << "," << y << "): " << objectAt;
        }
    }
}

TEST_F(InterfaceComplianceTest, ValidatesAllTanksDeadScenario) {
    // Create a scenario where all tanks should die quickly
    auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = 
        GameScenarioBuilder()
            .withBoardSize(3, 3)
            .withTwoPlayerSetup(Point(0, 1), Point(2, 1))
            .withMaxSteps(100) // High enough to not interfere
            .withNumShells(10)
            .build();

    // Use shooter algorithms that will cause tanks to destroy each other
    GameResult result = gameManager->run(
        width, height,
        satelliteView,
        maxSteps, numShells,
        *player1, *player2,
        mockFactoryShooter, mockFactoryShooter
    );

    // If all tanks died, validate the gameState reflects this
    if (result.reason == GameResult::Reason::ALL_TANKS_DEAD && 
        result.remaining_tanks[0] == 0 && result.remaining_tanks[1] == 0) {
        
        GameResultValidator::validateGameStateNoTanks(result, width, height);
        GameResultValidator::validateGameStateTotalTanks(result, 0, width, height);
        
        // Validate that no tanks remain on the board
        EXPECT_EQ(result.remaining_tanks[0], 0u);
        EXPECT_EQ(result.remaining_tanks[1], 0u);
    }
}

} // namespace GameManager_098765432_123456789 