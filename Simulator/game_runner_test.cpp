#include <gtest/gtest.h>
#include "game_runner.h"
#include "test/helpers/board_info_test_helpers.h"
#include "test/helpers/registrar_test_helpers.h"
#include "registration/AlgorithmRegistrar.h"
#include "registration/GameManagerRegistrar.h"

class GameRunnerTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegistrarTestHelpers::clearAllRegistrars();
    }
    
    void TearDown() override {
        RegistrarTestHelpers::clearAllRegistrars();
    }
};

// Test that registrars start empty
TEST_F(GameRunnerTest, RegistrarsStartEmpty) {
    auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    EXPECT_EQ(gameManagerRegistrar.count(), 0);
    EXPECT_EQ(algorithmRegistrar.size(), 0);
}

// === HAPPY PATH TESTS ===

TEST_F(GameRunnerTest, SuccessfulGameExecution) {
    // Arrange
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    RegistrarTestHelpers::setupMockRegistrars();
    
    // Act
    GameResult result = GameRunner::runSingleGame(
        boardInfo, "TestGameManager", "TestAlgo1", "TestAlgo2", false);
    
    // Assert - MockGameManager returns default GameResult
    EXPECT_EQ(result.winner, 0);
    EXPECT_EQ(result.reason, GameResult::Reason::ALL_TANKS_DEAD);
    EXPECT_EQ(result.rounds, 0);
    EXPECT_TRUE(result.remaining_tanks.empty());
}

TEST_F(GameRunnerTest, VerboseModeHandling) {
    // Arrange
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    RegistrarTestHelpers::setupMockRegistrars();
    
    // Act - Test with verbose = true
    GameResult result = GameRunner::runSingleGame(
        boardInfo, "TestGameManager", "TestAlgo1", "TestAlgo2", true);
    
    // Assert - Should complete successfully with verbose mode
    EXPECT_EQ(result.winner, 0);
    
    // Act - Test with verbose = false (default)
    result = GameRunner::runSingleGame(
        boardInfo, "TestGameManager", "TestAlgo1", "TestAlgo2");
    
    // Assert - Should complete successfully without verbose mode
    EXPECT_EQ(result.winner, 0);
}

TEST_F(GameRunnerTest, ValidBoardInfoProcessing) {
    // Arrange
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    RegistrarTestHelpers::setupMockRegistrars();
    
    // Modify board info to test different values
    boardInfo.rows = 20;
    boardInfo.cols = 15;
    boardInfo.maxSteps = 500;
    boardInfo.numShells = 10;
    
    // Act
    GameResult result = GameRunner::runSingleGame(
        boardInfo, "TestGameManager", "TestAlgo1", "TestAlgo2", false);
    
    // Assert - Should handle different board dimensions successfully
    EXPECT_EQ(result.winner, 0);
}

TEST_F(GameRunnerTest, SameAlgorithmBothPlayers) {
    // Arrange
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    RegistrarTestHelpers::setupMockRegistrars();
    
    // Act - Use same algorithm for both players
    GameResult result = GameRunner::runSingleGame(
        boardInfo, "TestGameManager", "TestAlgo1", "TestAlgo1", false);
    
    // Assert - Should complete successfully
    EXPECT_EQ(result.winner, 0);
}

// === ERROR HANDLING TESTS ===

TEST_F(GameRunnerTest, NullSatelliteView) {
    // Arrange
    auto boardInfo = BoardInfoTestHelpers::createInvalidBoardInfo(); // Has null satellite view
    RegistrarTestHelpers::setupMockRegistrars();
    
    // Act & Assert
    EXPECT_THROW({
        GameRunner::runSingleGame(
            boardInfo, "TestGameManager", "TestAlgo1", "TestAlgo2", false);
    }, std::runtime_error);
}

TEST_F(GameRunnerTest, GameManagerNotFound) {
    // Arrange
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    RegistrarTestHelpers::setupMockRegistrars();
    
    // Act & Assert
    EXPECT_THROW({
        GameRunner::runSingleGame(
            boardInfo, "NonExistentGameManager", "TestAlgo1", "TestAlgo2", false);
    }, std::runtime_error);
}

TEST_F(GameRunnerTest, Algorithm1NotFound) {
    // Arrange
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    RegistrarTestHelpers::setupMockRegistrars();
    
    // Act & Assert
    EXPECT_THROW({
        GameRunner::runSingleGame(
            boardInfo, "TestGameManager", "NonExistentAlgo", "TestAlgo2", false);
    }, std::runtime_error);
}

TEST_F(GameRunnerTest, Algorithm2NotFound) {
    // Arrange
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    RegistrarTestHelpers::setupMockRegistrars();
    
    // Act & Assert
    EXPECT_THROW({
        GameRunner::runSingleGame(
            boardInfo, "TestGameManager", "TestAlgo1", "NonExistentAlgo", false);
    }, std::runtime_error);
}

TEST_F(GameRunnerTest, IncompleteAlgorithm1) {
    // Arrange
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    
    // Setup GameManager
    auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    gameManagerRegistrar.createGameManagerEntry("TestGameManager");
    gameManagerRegistrar.addGameManagerFactoryToLastEntry(
        [](bool verbose) { return std::make_unique<MockGameManager>(verbose); }
    );
    gameManagerRegistrar.validateLastRegistration();
    
    // Setup incomplete algorithm1 and complete algorithm2
    RegistrarTestHelpers::setupIncompleteAlgorithm(); // Creates "IncompleteAlgo"
    
    auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    algorithmRegistrar.createAlgorithmFactoryEntry("TestAlgo2");
    algorithmRegistrar.addPlayerFactoryToLastEntry(MockFactories::createMockPlayer);
    algorithmRegistrar.addTankAlgorithmFactoryToLastEntry(MockFactories::createMockTankAlgorithm);
    algorithmRegistrar.validateLastRegistration();
    
    // Act & Assert
    EXPECT_THROW({
        GameRunner::runSingleGame(
            boardInfo, "TestGameManager", "IncompleteAlgo", "TestAlgo2", false);
    }, std::runtime_error);
}

TEST_F(GameRunnerTest, IncompleteAlgorithm2) {
    // Arrange
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    
    // Setup GameManager
    auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    gameManagerRegistrar.createGameManagerEntry("TestGameManager");
    gameManagerRegistrar.addGameManagerFactoryToLastEntry(
        [](bool verbose) { return std::make_unique<MockGameManager>(verbose); }
    );
    gameManagerRegistrar.validateLastRegistration();
    
    // Setup complete algorithm1 and incomplete algorithm2
    auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    algorithmRegistrar.createAlgorithmFactoryEntry("TestAlgo1");
    algorithmRegistrar.addPlayerFactoryToLastEntry(MockFactories::createMockPlayer);
    algorithmRegistrar.addTankAlgorithmFactoryToLastEntry(MockFactories::createMockTankAlgorithm);
    algorithmRegistrar.validateLastRegistration();
    
    RegistrarTestHelpers::setupIncompleteAlgorithm(); // Creates "IncompleteAlgo"
    
    // Act & Assert
    EXPECT_THROW({
        GameRunner::runSingleGame(
            boardInfo, "TestGameManager", "TestAlgo1", "IncompleteAlgo", false);
    }, std::runtime_error);
}

TEST_F(GameRunnerTest, EmptyRegistrars) {
    // Arrange
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    // Don't setup any registrars - they should be empty from SetUp()
    
    // Act & Assert
    EXPECT_THROW({
        GameRunner::runSingleGame(
            boardInfo, "TestGameManager", "TestAlgo1", "TestAlgo2", false);
    }, std::runtime_error);
}

// === INTEGRATION TESTS ===

TEST_F(GameRunnerTest, MultipleGameExecutionWithSameBoardInfo) {
    // Arrange
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    RegistrarTestHelpers::setupMockRegistrars();
    
    // Act - Run multiple games with the same BoardInfo
    GameResult result1 = GameRunner::runSingleGame(
        boardInfo, "TestGameManager", "TestAlgo1", "TestAlgo2", false);
    
    GameResult result2 = GameRunner::runSingleGame(
        boardInfo, "TestGameManager", "TestAlgo2", "TestAlgo1", false);
    
    GameResult result3 = GameRunner::runSingleGame(
        boardInfo, "TestGameManager", "TestAlgo1", "TestAlgo1", true);
    
    // Assert - All games should complete successfully
    EXPECT_EQ(result1.winner, 0);
    EXPECT_EQ(result2.winner, 0);
    EXPECT_EQ(result3.winner, 0);
}

TEST_F(GameRunnerTest, RealRegistrarIntegrationPattern) {
    // This test validates that GameRunner follows the same pattern as other tests
    // that use the registrar system
    
    // Arrange - Setup using the same pattern as AlgorithmRegistrar tests
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    
    auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    // Verify registrars start clean
    EXPECT_EQ(gameManagerRegistrar.count(), 0);
    EXPECT_EQ(algorithmRegistrar.size(), 0);
    
    // Setup GameManager following existing patterns
    gameManagerRegistrar.createGameManagerEntry("IntegrationTestGM");
    gameManagerRegistrar.addGameManagerFactoryToLastEntry(
        [](bool verbose) { return std::make_unique<MockGameManager>(verbose); }
    );
    gameManagerRegistrar.validateLastRegistration();
    
    // Setup algorithms following existing patterns
    algorithmRegistrar.createAlgorithmFactoryEntry("IntegrationTestAlgo1");
    algorithmRegistrar.addPlayerFactoryToLastEntry(MockFactories::createMockPlayer);
    algorithmRegistrar.addTankAlgorithmFactoryToLastEntry(MockFactories::createMockTankAlgorithm);
    algorithmRegistrar.validateLastRegistration();
    
    algorithmRegistrar.createAlgorithmFactoryEntry("IntegrationTestAlgo2");
    algorithmRegistrar.addPlayerFactoryToLastEntry(MockFactories::createMockPlayer);
    algorithmRegistrar.addTankAlgorithmFactoryToLastEntry(MockFactories::createMockTankAlgorithm);
    algorithmRegistrar.validateLastRegistration();
    
    // Verify registration state
    EXPECT_EQ(gameManagerRegistrar.count(), 1);
    EXPECT_EQ(algorithmRegistrar.size(), 2);
    
    // Verify we can find entries
    EXPECT_NE(gameManagerRegistrar.findByName("IntegrationTestGM"), nullptr);
    
    bool foundAlgo1 = false, foundAlgo2 = false;
    for (const auto& entry : algorithmRegistrar) {
        if (entry.getName() == "IntegrationTestAlgo1") {
            foundAlgo1 = true;
            EXPECT_TRUE(entry.isComplete());
        }
        if (entry.getName() == "IntegrationTestAlgo2") {
            foundAlgo2 = true;
            EXPECT_TRUE(entry.isComplete());
        }
    }
    EXPECT_TRUE(foundAlgo1);
    EXPECT_TRUE(foundAlgo2);
    
    // Act - Run game using the registered components
    GameResult result = GameRunner::runSingleGame(
        boardInfo, "IntegrationTestGM", "IntegrationTestAlgo1", "IntegrationTestAlgo2", false);
    
    // Assert
    EXPECT_EQ(result.winner, 0);
}

TEST_F(GameRunnerTest, ValidateCleanupBetweenTests) {
    // This test ensures that the SetUp/TearDown properly isolates tests
    
    // Arrange & Assert - Registrars should be empty at start of each test
    auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    EXPECT_EQ(gameManagerRegistrar.count(), 0);
    EXPECT_EQ(algorithmRegistrar.size(), 0);
    
    // Act - Add some entries
    RegistrarTestHelpers::setupMockRegistrars();
    
    // Assert - Entries were added
    EXPECT_EQ(gameManagerRegistrar.count(), 1);
    EXPECT_EQ(algorithmRegistrar.size(), 2);
    
    // Note: TearDown will clean these up for the next test
}

TEST_F(GameRunnerTest, DifferentBoardSizes) {
    // Arrange
    RegistrarTestHelpers::setupMockRegistrars();
    
    // Test small board
    auto smallBoard = BoardInfoTestHelpers::createCustomBoardInfo(5, 5, 50, 3);
    
    // Test large board
    auto largeBoard = BoardInfoTestHelpers::createCustomBoardInfo(50, 50, 1000, 20);
    
    // Act & Assert - Both should work
    GameResult smallResult = GameRunner::runSingleGame(
        smallBoard, "TestGameManager", "TestAlgo1", "TestAlgo2", false);
    EXPECT_EQ(smallResult.winner, 0);
    
    GameResult largeResult = GameRunner::runSingleGame(
        largeBoard, "TestGameManager", "TestAlgo1", "TestAlgo2", false);
    EXPECT_EQ(largeResult.winner, 0);
}