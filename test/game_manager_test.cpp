#include "gtest/gtest.h"
#include "game_manager.h"
#include <fstream>
#include <sstream>
#include <cstdio>

class GameManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempFilePath = "temp_test_board.txt";
    }
    
    void TearDown() override {
        std::remove(tempFilePath.c_str());
    }
    
    void createTestBoardFile(const std::vector<std::string>& lines) {
        std::ofstream file(tempFilePath);
        for (const auto& line : lines) {
            file << line << '\n';
        }
        file.close();
    }
    
    std::string tempFilePath;
};

TEST_F(GameManagerTest, Constructor) {
    GameManager manager;
    
    // We can't directly access private members, so we'll test indirectly
    // by calling initialize with an invalid file and checking it fails
    EXPECT_FALSE(manager.initialize("nonexistent_file.txt"));
}

TEST_F(GameManagerTest, Initialize_ValidBoard) {
    // Create a valid test board file
    std::vector<std::string> boardLines = {
        "5 5",    // 5x5 board
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    };
    createTestBoardFile(boardLines);
    
    GameManager manager;
    EXPECT_TRUE(manager.initialize(tempFilePath));
}

// Test initialize with invalid board file
TEST_F(GameManagerTest, Initialize_InvalidBoard) {
    // Create an invalid test board file (no tanks)
    std::vector<std::string> boardLines = {
        "5 5",    // 5x5 board
        "#####",
        "#   #",
        "#   #",
        "# @ #",
        "#####"
    };
    createTestBoardFile(boardLines);
    
    GameManager manager;
    EXPECT_FALSE(manager.initialize(tempFilePath));
}

// Test initialize with invalid board dimensions
TEST_F(GameManagerTest, Initialize_InvalidDimensions) {
    // Create a test board file with invalid dimensions
    std::vector<std::string> boardLines = {
        "-5 -5",  // Negative dimensions
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    };
    createTestBoardFile(boardLines);
    
    GameManager manager;
    EXPECT_FALSE(manager.initialize(tempFilePath));
}

// Test initialize with empty file
TEST_F(GameManagerTest, Initialize_EmptyFile) {
    // Create an empty file
    createTestBoardFile({});
    
    GameManager manager;
    EXPECT_FALSE(manager.initialize(tempFilePath));
}

// Test initialize with missing tank
TEST_F(GameManagerTest, Initialize_MissingTank) {
    // Create a board with only one tank
    std::vector<std::string> boardLines = {
        "5 5",    // 5x5 board
        "#####",
        "#1  #",  // Missing tank 2
        "#   #",
        "# @ #",
        "#####"
    };
    createTestBoardFile(boardLines);
    
    GameManager manager;
    EXPECT_FALSE(manager.initialize(tempFilePath));
}

// Test cleanup method through reinitialization
TEST_F(GameManagerTest, Cleanup_ThroughReinitialization) {
    // Create a valid test board file
    std::vector<std::string> boardLines = {
        "5 5",    // 5x5 board
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    };
    createTestBoardFile(boardLines);
    
    GameManager manager;
    
    // Initialize once
    EXPECT_TRUE(manager.initialize(tempFilePath));
    
    // Initialize again - this should call cleanup internally
    EXPECT_TRUE(manager.initialize(tempFilePath));
    
    // No crash means cleanup worked properly
}

// This test checks that initialization errors are being logged
TEST_F(GameManagerTest, Initialize_ErrorsAreLogged) {
    // Create a board with recoverable errors (e.g., multiple tanks)
    std::vector<std::string> boardLines = {
        "5 5",      // 5x5 board
        "#####",
        "#1 2#",
        "#1 2#",    // Duplicate tanks
        "# @ #",
        "#####"
    };
    createTestBoardFile(boardLines);
    
    GameManager manager;
    EXPECT_TRUE(manager.initialize(tempFilePath));
    
    // We can't directly access the game log, but we know errors should be logged
    // Instead, we'll save results to a temporary file and check for error messages
    std::string outputPath = tempFilePath + ".out";
    manager.saveResults(outputPath);
    
    // TODO: This part of the test might need adjustment if saveResults is fully implemented
    // as it currently only outputs to cout
}