#include "gtest/gtest.h"
#include "game_manager.h"
#include "algo/algorithm.h"
#include "utils/action.h"
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

    Action testGetPlayerAction(GameManager& manager, int playerId) {
      return manager.getPlayerAction(playerId);
    }
    
    void testLogAction(GameManager& manager, int playerId, Action action, bool valid) {
        manager.logAction(playerId, action, valid);
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

// Test getTanks returns empty vector before initialization
TEST_F(GameManagerTest, GetTanks_EmptyBeforeInit) {
  GameManager manager;
  
  // Before initialization, tanks vector should be empty
  EXPECT_TRUE(manager.getTanks().empty());
}

// Test tank creation with normal board
TEST_F(GameManagerTest, Initialize_NormalTankCreation) {
  std::vector<std::string> boardLines = {
      "5 5",
      "#####",
      "#1 2#",  // Player 1 at (1,1), Player 2 at (3,1)
      "#   #",
      "#   #",
      "#####"
  };
  createTestBoardFile(boardLines);
  
  GameManager manager;
  EXPECT_TRUE(manager.initialize(tempFilePath));
  
  const auto& tanks = manager.getTanks();
  ASSERT_EQ(tanks.size(), 2);
  
  // Check player 1's tank
  EXPECT_EQ(tanks[0].getPlayerId(), 1);
  EXPECT_EQ(tanks[0].getPosition(), Point(1, 1));
  EXPECT_EQ(tanks[0].getDirection(), Direction::Left);
  
  // Check player 2's tank
  EXPECT_EQ(tanks[1].getPlayerId(), 2);
  EXPECT_EQ(tanks[1].getPosition(), Point(3, 1));
  EXPECT_EQ(tanks[1].getDirection(), Direction::Right);
}

// Test tank creation with multiple tanks for one player
TEST_F(GameManagerTest, Initialize_MultipleTanksOnePlayer) {
  std::vector<std::string> boardLines = {
      "5 5",
      "#1###",
      "#   #",
      "# 1 #",  // Second tank for player 1
      "#  2#",
      "#####"
  };
  createTestBoardFile(boardLines);
  
  GameManager manager;
  EXPECT_TRUE(manager.initialize(tempFilePath));
  
  const auto& tanks = manager.getTanks();
  ASSERT_EQ(tanks.size(), 2);
  
  // Should keep the first tank found (top-to-bottom, left-to-right scan)
  EXPECT_EQ(tanks[0].getPlayerId(), 1);
  EXPECT_EQ(tanks[0].getPosition(), Point(1, 0));
  
  EXPECT_EQ(tanks[1].getPlayerId(), 2);
  EXPECT_EQ(tanks[1].getPosition(), Point(3, 3));
}

// Test tank creation with multiple tanks for both players
TEST_F(GameManagerTest, Initialize_MultipleTanksBothPlayers) {
  std::vector<std::string> boardLines = {
      "5 5",
      "#12##",  // First tanks
      "#   #",
      "# 1 #",  // Duplicate player 1
      "#  2#",  // Duplicate player 2
      "#####"
  };
  createTestBoardFile(boardLines);
  
  GameManager manager;
  EXPECT_TRUE(manager.initialize(tempFilePath));
  
  const auto& tanks = manager.getTanks();
  ASSERT_EQ(tanks.size(), 2);
  
  // Should keep the first tanks found
  EXPECT_EQ(tanks[0].getPlayerId(), 1);
  EXPECT_EQ(tanks[0].getPosition(), Point(1, 0));
  
  EXPECT_EQ(tanks[1].getPlayerId(), 2);
  EXPECT_EQ(tanks[1].getPosition(), Point(2, 0));
}

TEST_F(GameManagerTest, Initialize_CreatesErrorFile) {
  // Create a board with a recoverable error (extra row)
  std::vector<std::string> boardLines = {
      "5 4",    // 5x4 board, but we provide 5 rows
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"  // Extra row
  };
  createTestBoardFile(boardLines);
  
  // Remove any existing error file first
  std::remove("input_errors.txt");
  
  GameManager manager;
  EXPECT_TRUE(manager.initialize(tempFilePath));
  
  // Check that error file was created
  std::ifstream errorFile("input_errors.txt");
  ASSERT_TRUE(errorFile.is_open());
  
  // Verify file has content (at least one line)
  std::string line;
  std::getline(errorFile, line);
  bool hasContent = !errorFile.fail() && !line.empty();
  EXPECT_TRUE(hasContent);
  
  // Clean up
  errorFile.close();
  std::remove("input_errors.txt");
}

// Test that initialization doesn't create an error file when there are no errors
TEST_F(GameManagerTest, Initialize_NoErrorFile) {
  // Create a valid board with no errors
  std::vector<std::string> boardLines = {
      "5 5",
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"
  };
  createTestBoardFile(boardLines);
  
  // Remove any existing error file first
  std::remove("input_errors.txt");
  
  GameManager manager;
  EXPECT_TRUE(manager.initialize(tempFilePath));
  
  // Check that error file was not created
  std::ifstream errorFile("input_errors.txt");
  EXPECT_FALSE(errorFile.is_open());
}

TEST_F(GameManagerTest, GetPlayerAction_ClearShot) {
  // Create a board where tanks are in a direct line of sight
  std::vector<std::string> boardLines = {
      "5 5",
      "#####",
      "#2 1#",  // Player 1 and 2 in same row with clear line of sight
      "#   #",
      "#   #",
      "#####"
  };
  createTestBoardFile(boardLines);
  
  GameManager manager;
  ASSERT_TRUE(manager.initialize(tempFilePath));
  
  // For DefensiveAlgorithm, when there's a clear shot, it should return Shoot
  Action player2Action = testGetPlayerAction(manager, 2);
  std::cout << "Player 2 action (defensive with clear shot): " 
            << actionToString(player2Action) << std::endl;
  
  // We expect DefensiveAlgorithm to shoot when there's a clear shot
  EXPECT_EQ(player2Action, Action::Shoot);
}

// Test getPlayerAction with invalid player IDs
TEST_F(GameManagerTest, GetPlayerAction_InvalidPlayerIds) {
  // Create a valid test board file
  std::vector<std::string> boardLines = {
      "5 5",
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"
  };
  createTestBoardFile(boardLines);
  
  GameManager manager;
  ASSERT_TRUE(manager.initialize(tempFilePath));
  
  // Test with invalid player IDs
  Action invalidAction1 = testGetPlayerAction(manager, 0);
  Action invalidAction2 = testGetPlayerAction(manager, 3);
  
  // Should return None for invalid IDs
  EXPECT_EQ(invalidAction1, Action::None);
  EXPECT_EQ(invalidAction2, Action::None);
}


// Test logAction adds entries correctly
TEST_F(GameManagerTest, LogAction_FormatsCorrectly) {
  GameManager manager;
  
  // Log a valid action using the test helper method
  testLogAction(manager, 1, Action::MoveForward, true);
  
  // Check log entry format
  const auto& log = manager.getGameLog();
  ASSERT_EQ(log.size(), 1);
  EXPECT_EQ(log[0], "Player 1: Move Forward - Success");
  
  // Log an invalid action
  testLogAction(manager, 2, Action::Shoot, false);
  
  // Check both log entries
  ASSERT_EQ(log.size(), 2);
  EXPECT_EQ(log[1], "Player 2: Shoot - Invalid");
}

// Test getGameLog accessor with multiple entries
TEST_F(GameManagerTest, GetGameLog_MultipleEntries) {
  GameManager manager;
  
  // Log several actions
  testLogAction(manager, 1, Action::MoveForward, true);
  testLogAction(manager, 2, Action::RotateLeftEighth, false);
  testLogAction(manager, 1, Action::Shoot, true);
  
  // Get the log
  const auto& log = manager.getGameLog();
  
  // Check that the log contains all entries
  ASSERT_EQ(log.size(), 3);
  EXPECT_EQ(log[0], "Player 1: Move Forward - Success");
  EXPECT_EQ(log[1], "Player 2: Rotate Left 1/8 - Invalid");
  EXPECT_EQ(log[2], "Player 1: Shoot - Success");
}