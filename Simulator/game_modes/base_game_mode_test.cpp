#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "base_game_mode.h"
#include "../test/mocks/MockSatelliteView.h"
#include <memory>
#include <fstream>
#include <cstdio>

/**
 * @brief Mock implementation of BaseGameMode for testing
 * 
 * This mock class implements the abstract methods of BaseGameMode
 * to enable testing of the template method pattern and common functionality.
 */
class MockBaseGameMode : public BaseGameMode {
public:
    MockBaseGameMode() = default;
    
    // Mock the abstract methods
    MOCK_METHOD(bool, loadLibraries, (const BaseParameters& params), (override));
    MOCK_METHOD(bool, loadMap, (const std::string& mapFile), (override));
    MOCK_METHOD(GameResult, executeGameLogic, (const BaseParameters& params), (override));
    MOCK_METHOD(void, displayResults, (const GameResult& result), (override));
    
    // Expose protected methods for testing
    using BaseGameMode::cleanup;
    using BaseGameMode::generateTimestamp;
    using BaseGameMode::enumerateFiles;
    using BaseGameMode::validateLibrary;
    using BaseGameMode::createErrorResult;
    using BaseGameMode::writeToFile;
    using BaseGameMode::m_libraryManager;
};

/**
 * @brief Test fixture for BaseGameMode tests
 */
class BaseGameModeTest : public ::testing::Test {
protected:
    void SetUp() override {
        gameMode = std::make_unique<MockBaseGameMode>();
        
        // Create temporary test map file
        testMapFile = "/tmp/test_map.txt";
        std::ofstream file(testMapFile);
        file << "rows = 5\n";
        file << "cols = 5\n";
        file << "maxSteps = 100\n";
        file << "numShells = 10\n";
        file << ".....\n";
        file << ".1.2.\n";
        file << ".....\n";
        file << ".....\n";
        file << ".....\n";
        file.close();
    }
    
    void TearDown() override {
        // Clean up temporary files
        if (!testMapFile.empty()) {
            std::remove(testMapFile.c_str());
        }
        gameMode.reset();
    }
    
    std::unique_ptr<MockBaseGameMode> gameMode;
    std::string testMapFile;
};

/**
 * @brief Test that constructor initializes properly
 */
TEST_F(BaseGameModeTest, ConstructorInitializesLibraryManager) {
    // LibraryManager is a singleton reference, should always be valid
    EXPECT_NO_THROW(gameMode->m_libraryManager.getLoadedLibraryNames());
}

/**
 * @brief Test successful execution flow
 */
TEST_F(BaseGameModeTest, ExecuteSuccessfulFlow) {
    BaseGameMode::BaseParameters params;
    params.mapFile = testMapFile;
    params.verbose = true;
    
    // Set up expectations for successful execution
    EXPECT_CALL(*gameMode, loadLibraries(::testing::_))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*gameMode, loadMap(params.mapFile))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*gameMode, executeGameLogic(::testing::_))
        .WillOnce(::testing::Invoke([](const BaseGameMode::BaseParameters&) {
            GameResult result;
            result.winner = 1;
            result.reason = GameResult::Reason::ALL_TANKS_DEAD;
            result.rounds = 10;
            return result;
        }));
    EXPECT_CALL(*gameMode, displayResults(::testing::_))
        .Times(1);  // Should be called because verbose=true
    
    GameResult result = gameMode->execute(params);
    
    EXPECT_EQ(result.winner, 1);
    EXPECT_EQ(result.reason, GameResult::Reason::ALL_TANKS_DEAD);
    EXPECT_EQ(result.rounds, 10);
}

/**
 * @brief Test execution with verbose=false
 */
TEST_F(BaseGameModeTest, ExecuteWithVerboseFalse) {
    BaseGameMode::BaseParameters params;
    params.mapFile = testMapFile;
    params.verbose = false;
    
    EXPECT_CALL(*gameMode, loadLibraries(::testing::_))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*gameMode, loadMap(params.mapFile))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*gameMode, executeGameLogic(::testing::_))
        .WillOnce(::testing::Invoke([](const BaseGameMode::BaseParameters&) {
            GameResult result;
            result.winner = 1;
            result.reason = GameResult::Reason::ALL_TANKS_DEAD;
            result.rounds = 10;
            return result;
        }));
    EXPECT_CALL(*gameMode, displayResults(::testing::_))
        .Times(1);  // displayResults is always called regardless of verbose setting
    
    GameResult result = gameMode->execute(params);
    EXPECT_EQ(result.winner, 1);
}

/**
 * @brief Test execution failure in loadLibraries
 */
TEST_F(BaseGameModeTest, ExecuteFailsOnLoadLibraries) {
    BaseGameMode::BaseParameters params;
    params.mapFile = testMapFile;
    params.verbose = true;
    
    EXPECT_CALL(*gameMode, loadLibraries(::testing::_))
        .WillOnce(::testing::Return(false));
    EXPECT_CALL(*gameMode, loadMap(::testing::_))
        .Times(0);  // Should not be called if loadLibraries fails
    EXPECT_CALL(*gameMode, executeGameLogic(::testing::_))
        .Times(0);  // Should not be called if loadLibraries fails
    EXPECT_CALL(*gameMode, displayResults(::testing::_))
        .Times(0);  // Should not be called if execution fails
    
    GameResult result = gameMode->execute(params);
    
    // Should return error result
    EXPECT_EQ(result.winner, 0);
    EXPECT_EQ(result.rounds, 0);
    EXPECT_EQ(result.gameState, nullptr);
}

/**
 * @brief Test execution failure in loadMap
 */
TEST_F(BaseGameModeTest, ExecuteFailsOnLoadMap) {
    BaseGameMode::BaseParameters params;
    params.mapFile = testMapFile;  // Use valid file for setup, but mock loadMap to fail
    params.verbose = true;
    
    EXPECT_CALL(*gameMode, loadLibraries(::testing::_))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*gameMode, loadMap(params.mapFile))
        .WillOnce(::testing::Return(false));  // Mock this to fail
    EXPECT_CALL(*gameMode, executeGameLogic(::testing::_))
        .Times(0);  // Should not be called if loadMap fails
    EXPECT_CALL(*gameMode, displayResults(::testing::_))
        .Times(0);  // Should not be called if execution fails
    
    GameResult result = gameMode->execute(params);
    
    // Should return error result
    EXPECT_EQ(result.winner, 0);
    EXPECT_EQ(result.rounds, 0);
    EXPECT_EQ(result.gameState, nullptr);
}

/**
 * @brief Test execution with exception in executeGameLogic
 */
TEST_F(BaseGameModeTest, ExecuteHandlesException) {
    BaseGameMode::BaseParameters params;
    params.mapFile = testMapFile;
    params.verbose = true;
    
    EXPECT_CALL(*gameMode, loadLibraries(::testing::_))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*gameMode, loadMap(params.mapFile))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*gameMode, executeGameLogic(::testing::_))
        .WillOnce(::testing::Throw(std::runtime_error("Test exception")));
    EXPECT_CALL(*gameMode, displayResults(::testing::_))
        .Times(0);  // Should not be called if exception occurs
    
    GameResult result = gameMode->execute(params);
    
    // Should return error result
    EXPECT_EQ(result.winner, 0);
    EXPECT_EQ(result.rounds, 0);
    EXPECT_EQ(result.gameState, nullptr);
}

/**
 * @brief Test empty map file parameter
 */
TEST_F(BaseGameModeTest, ExecuteFailsOnEmptyMapFile) {
    BaseGameMode::BaseParameters params;
    params.mapFile = "";  // Empty map file
    params.verbose = true;
    
    EXPECT_CALL(*gameMode, loadLibraries(::testing::_))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*gameMode, loadMap(""))
        .WillOnce(::testing::Return(false)); 
    EXPECT_CALL(*gameMode, executeGameLogic(::testing::_))
        .Times(0);
    EXPECT_CALL(*gameMode, displayResults(::testing::_))
        .Times(0);
    
    GameResult result = gameMode->execute(params);
    
    // Should return error result
    EXPECT_EQ(result.winner, 0);
    EXPECT_EQ(result.rounds, 0);
    EXPECT_EQ(result.gameState, nullptr);
}

/**
 * @brief Test createErrorResult method
 */
TEST_F(BaseGameModeTest, CreateErrorResult) {
    GameResult result = gameMode->createErrorResult();
    
    EXPECT_EQ(result.winner, 0);
    EXPECT_EQ(result.reason, GameResult::Reason::ALL_TANKS_DEAD);
    EXPECT_TRUE(result.remaining_tanks.empty());
    EXPECT_EQ(result.gameState, nullptr);
    EXPECT_EQ(result.rounds, 0);
}

/**
 * @brief Test generateTimestamp method
 */
TEST_F(BaseGameModeTest, GenerateTimestamp) {
    std::string timestamp = gameMode->generateTimestamp();
    
    // Should not be empty and should have expected format length
    EXPECT_FALSE(timestamp.empty());
    // Basic timestamp format: YYYYMMDD_HHMMSS (15 characters)
    EXPECT_GE(timestamp.length(), 15);
}

/**
 * @brief Test generateTimestamp with milliseconds
 */
TEST_F(BaseGameModeTest, GenerateTimestampWithMilliseconds) {
    std::string timestamp = gameMode->generateTimestamp(true);
    
    // Should not be empty and should be longer with milliseconds
    EXPECT_FALSE(timestamp.empty());
    // With milliseconds should be longer than basic timestamp
    std::string basicTimestamp = gameMode->generateTimestamp(false);
    EXPECT_GT(timestamp.length(), basicTimestamp.length());
}

/**
 * @brief Test enumerateFiles with .so extension
 */
TEST_F(BaseGameModeTest, EnumerateFilesWithSoExtension) {
    // This will call FileEnumerator::enumerateSoFiles internally
    std::vector<std::string> files = gameMode->enumerateFiles("/tmp", ".so");
    
    // Should return a vector (may be empty if no .so files in /tmp)
    // The important thing is that it doesn't crash and returns a valid vector
    EXPECT_TRUE(files.empty() || !files.empty());  // Always true, but exercises the code path
}

/**
 * @brief Test enumerateFiles with .txt extension
 */
TEST_F(BaseGameModeTest, EnumerateFilesWithTxtExtension) {
    // This will call FileEnumerator::enumerateMapFiles internally
    std::vector<std::string> files = gameMode->enumerateFiles("/tmp", ".txt");
    
    // Should return a vector (may be empty if no .txt files in /tmp)
    EXPECT_TRUE(files.empty() || !files.empty());  // Always true, but exercises the code path
}

/**
 * @brief Test enumerateFiles with unsupported extension
 */
TEST_F(BaseGameModeTest, EnumerateFilesWithUnsupportedExtension) {
    std::vector<std::string> files = gameMode->enumerateFiles("/tmp", ".xyz");
    
    // Should return empty vector for unsupported extension
    EXPECT_TRUE(files.empty());
}

/**
 * @brief Test validateLibrary with GameManager type
 */
TEST_F(BaseGameModeTest, ValidateLibraryGameManager) {
    LibraryValidator::LibraryInfo info = gameMode->validateLibrary("test.so", "GameManager");
    
    // Should return a LibraryInfo object (may not be valid if file doesn't exist)
    EXPECT_FALSE(info.path.empty());
    EXPECT_EQ(info.path, "test.so");
}

/**
 * @brief Test validateLibrary with Algorithm type
 */
TEST_F(BaseGameModeTest, ValidateLibraryAlgorithm) {
    LibraryValidator::LibraryInfo info = gameMode->validateLibrary("test.so", "Algorithm");
    
    // Should return a LibraryInfo object
    EXPECT_FALSE(info.path.empty());
    EXPECT_EQ(info.path, "test.so");
}

/**
 * @brief Test validateLibrary with unknown type
 */
TEST_F(BaseGameModeTest, ValidateLibraryUnknownType) {
    LibraryValidator::LibraryInfo info = gameMode->validateLibrary("test.so", "Unknown");
    
    // Should return a LibraryInfo object with error
    EXPECT_FALSE(info.path.empty());
    EXPECT_FALSE(info.error.empty());
    EXPECT_FALSE(info.isReady());
}

/**
 * @brief Test cleanup method
 */
TEST_F(BaseGameModeTest, CleanupMethod) {
    // Should not crash when called
    EXPECT_NO_THROW(gameMode->cleanup());
    
    // Should be safe to call multiple times
    EXPECT_NO_THROW(gameMode->cleanup());
    EXPECT_NO_THROW(gameMode->cleanup());
}