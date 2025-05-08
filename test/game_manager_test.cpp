#include "gtest/gtest.h"
#include "game_manager.h"
#include "algo/algorithm.h"
#include "utils/action.h"
#include "test/mock_algorithm.h"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <filesystem>
#include "player_factory_impl.h"
#include "algo/tank_algorithm_factory_impl.h"

class GameManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempFilePath = "temp_test_board.txt";
        outputFilePath = "test_output.txt";

        algo1 = new MockAlgorithm();
        algo2 = new MockAlgorithm();
        mockAlgo1 = dynamic_cast<MockAlgorithm*>(algo1);
        mockAlgo2 = dynamic_cast<MockAlgorithm*>(algo2);
    }
    
    void TearDown() override {
        std::remove(tempFilePath.c_str());
        std::remove(outputFilePath.c_str());

        delete algo1;
        delete algo2;
    }

    Algorithm* algo1;
    Algorithm* algo2;
    MockAlgorithm* mockAlgo1;
    MockAlgorithm* mockAlgo2;
    
    void createTestBoardFile(const std::vector<std::string>& lines) {
        std::ofstream file(tempFilePath);
        for (const auto& line : lines) {
            file << line << '\n';
        }
        file.close();
    }

    bool initializeManager(GameManager& manager, const std::vector<std::string>& boardLines) {
      createTestBoardFile(boardLines);
      
      // We'll create copies of our algorithms to give to the manager
      // This way we keep our originals for setting up test behaviors
      Algorithm* algoForP1 = new MockAlgorithm();
      Algorithm* algoForP2 = new MockAlgorithm();
      
      // Copy the current settings from our test algorithms
      if (dynamic_cast<MockAlgorithm*>(algoForP1) && dynamic_cast<MockAlgorithm*>(algoForP2)) {
          *dynamic_cast<MockAlgorithm*>(algoForP1) = *mockAlgo1;
          *dynamic_cast<MockAlgorithm*>(algoForP2) = *mockAlgo2;
      }
      
      // Initialize the manager with our algorithms
      return manager.readBoard(tempFilePath, algoForP1, algoForP2);
    }

    // Standard test board for most scenarios
    std::vector<std::string> getStandardBoard() {
        return {
            "########",
            "#2    1#", // Tanks are in the same row with clear line of sight
            "#   @  #",
            "#      #", // Mine in the middle
            "########"
        };
    }

    Action testGetPlayerAction(GameManager& manager, int playerId) {
      return manager.getPlayerAction(playerId);
    }
    
    void testLogAction(GameManager& manager, int playerId, Action action, bool valid) {
        manager.logAction(playerId, action, valid);
    }

    void testMoveShellsOnce(GameManager& manager) {
      manager.moveShellsOnce();
    }

    void testProcessStep(GameManager& manager) {
      manager.processStep();
    }

    void testProcessStep(GameManager& manager, int steps) {
      for (int i = 0; i < steps; ++i) {
        manager.processStep();
      }
    }

    void testApplyAction(GameManager& manager, int playerId, Action action) {
      manager.applyAction(playerId, action);
    }

    bool testCheckGameOver(GameManager& manager) {
      return manager.checkGameOver();
    }

    std::string testGetGameResult(GameManager& manager) {
      return manager.m_gameResult;
  }
  
    void testSetRemainingSteps(GameManager& manager, int steps) {
        manager.m_remaining_steps = steps;
    }
    
    std::vector<Tank>& getTanksMutable(GameManager& manager) {
        return manager.m_tanks;
    }

    void testSaveResults(GameManager& manager, const std::string& outputFilePath) {
      manager.saveResults(outputFilePath);
    }

    void setAlgorithm(GameManager& manager, int playerId, Algorithm* algorithm) {
        if (playerId == 1) {
            manager.m_player1Algorithm = algorithm;
        } else if (playerId == 2) {
            manager.m_player2Algorithm = algorithm;
        }
    }

    void createShell(GameManager& manager, int playerId, const Point& position, Direction direction) {
        Shell shell(playerId, position, direction);
        manager.m_shells.push_back(shell);
    }

    void setTankDirection(GameManager& manager, int playerId, Direction direction) {
        for (auto& tank : manager.m_tanks) {
            if (tank.getPlayerId() == playerId) {
                tank.setDirection(direction);
                break;
            }
        }
    }

    // Helper method to verify log contents
    void verifyLogContains(GameManager& manager, const std::vector<std::string>& expectedEntries) {
        for (const auto& expected : expectedEntries) {
            bool found = false;
            for (const auto& logEntry : manager.m_gameLog) {
                if (logEntry.find(expected) != std::string::npos) {
                    found = true;
                    break;
                }
            }
            EXPECT_TRUE(found) << "Expected log entry not found: " << expected;
        }
    }

    const std::vector<std::string>& getGameLog(GameManager& manager) const {
      return manager.m_gameLog;
    }
    std::vector<Tank> getTanks(GameManager& manager) const {
        return manager.m_tanks;
    }
    std::vector<Shell> getShells(GameManager& manager) const {
        return manager.m_shells;
    }
  
    // Helper to check if the game ended with the expected result
    void verifyGameResult(GameManager& manager, const std::string& expectedResult) {
        EXPECT_EQ(manager.m_gameResult, expectedResult);
    }

    void testRunGame(GameManager& manager) {
        manager.run();
    }

    int getGameSteps(GameManager& manager) {
        return manager.m_currentStep;
    }
    
    std::string tempFilePath;
    std::string outputFilePath;

    // Helper for new board input format
    std::vector<std::string> makeBoardFile(const std::string& name, int maxSteps, int numShells, int rows, int cols, const std::vector<std::string>& map) {
        std::vector<std::string> result;
        result.push_back(name);
        result.push_back("MaxSteps=" + std::to_string(maxSteps));
        result.push_back("NumShells=" + std::to_string(numShells));
        result.push_back("Rows=" + std::to_string(rows));
        result.push_back("Cols=" + std::to_string(cols));
        result.insert(result.end(), map.begin(), map.end());
        return result;
    }

    TankAlgorithmFactory* getTankAlgorithmFactory(GameManager& manager) {
        return manager.m_tankAlgorithmFactory.get();
    }
    void callCreateTankAlgorithms(GameManager& manager) {
        manager.createTankAlgorithms();
    }
};

// Mock TankAlgorithmFactory for testing createTankAlgorithms
class RecordingTankAlgorithmFactory : public TankAlgorithmFactory {
public:
    struct Call {
        int playerId;
        int tankIndex;
    };
    std::vector<Call> calls;
    std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override {
        const_cast<RecordingTankAlgorithmFactory*>(this)->calls.push_back({player_index, tank_index});
        // Return a dummy pointer (could be nullptr for this test)
        return nullptr;
    }
};

TEST_F(GameManagerTest, CreateTankAlgorithms_CreatesCorrectAlgorithms) {
    // Board with 3 tanks for player 1 and 2 tanks for player 2
    std::vector<std::pair<int, Point>> tankPositions = {
        {1, Point(0,0)}, {2, Point(1,0)}, {1, Point(2,0)}, {2, Point(3,0)}, {1, Point(4,0)}
    };
    GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<RecordingTankAlgorithmFactory>());
    // Directly set m_tanks for the test
    auto& tanks = getTanksMutable(manager);
    tanks.clear();
    for (const auto& [playerId, pos] : tankPositions) {
        tanks.emplace_back(playerId, pos, playerId == 1 ? Direction::Left : Direction::Right);
    }
    auto* factory = static_cast<RecordingTankAlgorithmFactory*>(getTankAlgorithmFactory(manager));
    callCreateTankAlgorithms(manager);
    // There should be 5 calls, one for each tank
    ASSERT_EQ(factory->calls.size(), tankPositions.size());
    // Check the playerId and tankIndex for each call
    std::map<int, int> expectedTankIndices; // playerId -> next expected index
    for (size_t i = 0; i < tankPositions.size(); ++i) {
        int expectedPlayer = tankPositions[i].first;
        int expectedIndex = expectedTankIndices[expectedPlayer]++;
        EXPECT_EQ(factory->calls[i].playerId, expectedPlayer);
        EXPECT_EQ(factory->calls[i].tankIndex, expectedIndex);
    }
}

TEST_F(GameManagerTest, Constructor) {
    GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
    
    // We can't directly access private members, so we'll test indirectly
    // by calling initialize with an invalid file and checking it fails
    EXPECT_FALSE(manager.readBoard("nonexistent_file.txt"));
}

TEST_F(GameManagerTest, Initialize_ValidBoard) {
    // Create a valid test board file
    auto boardLines = makeBoardFile("Test Board", 1000, 20, 8, 5, getStandardBoard());
    createTestBoardFile(boardLines);
    
    GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
    EXPECT_TRUE(manager.readBoard(tempFilePath));
}

// Test initialize with invalid board file
TEST_F(GameManagerTest, Initialize_NoTanks) {
    // Create an invalid test board file (no tanks)
    auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 5, {
        "#####",
        "#   #",
        "#   #",
        "# @ #",
        "#####"
    });
    createTestBoardFile(boardLines);
    
    GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
    EXPECT_TRUE(manager.readBoard(tempFilePath));
}

// Test initialize with invalid board dimensions
TEST_F(GameManagerTest, Initialize_InvalidDimensions) {
    // Create a test board file with invalid dimensions
    auto boardLines = makeBoardFile("Test Board", 1000, 20, -5, -5, {
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    });
    createTestBoardFile(boardLines);
    
    GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
    EXPECT_FALSE(manager.readBoard(tempFilePath));
}

// Test initialize with empty file
TEST_F(GameManagerTest, Initialize_EmptyFile) {
    // Create an empty file
    auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 5, {});
    createTestBoardFile(boardLines);
    
    GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
    EXPECT_FALSE(manager.readBoard(tempFilePath));
}

// Test cleanup method through reinitialization
TEST_F(GameManagerTest, Cleanup_ThroughReinitialization) {
    // Create a valid test board file
    auto boardLines = makeBoardFile("Test Board", 1000, 20, 8, 5, getStandardBoard());
    createTestBoardFile(boardLines);
    
    GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
    
    // Initialize once
    EXPECT_TRUE(manager.readBoard(tempFilePath));
    
    // Initialize again - this should call cleanup internally
    EXPECT_TRUE(manager.readBoard(tempFilePath));
    
    // No crash means cleanup worked properly
}

// Test getTanks returns empty vector before initialization
TEST_F(GameManagerTest, GetTanks_EmptyBeforeInit) {
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  
  // Before initialization, tanks vector should be empty
  EXPECT_TRUE(getTanks(manager).empty());
}

// Test tank creation with normal board
TEST_F(GameManagerTest, Initialize_NormalTankCreation) {
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 5, {
      "#####",
      "#1 2#",  // Player 1 at (1,1), Player 2 at (3,1)
      "#   #",
      "#   #",
      "#####"
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  EXPECT_TRUE(manager.readBoard(tempFilePath));
  
  const auto& tanks = getTanks(manager);
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
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 5, {
      "#1###",
      "#   #",
      "# 1 #",  // Second tank for player 1
      "#  2#",
      "#####"
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  EXPECT_TRUE(manager.readBoard(tempFilePath));
  
  const auto& tanks = getTanks(manager);
  ASSERT_EQ(tanks.size(), 3);
  
  // Should keep the first tank found (top-to-bottom, left-to-right scan)
  EXPECT_EQ(tanks[0].getPlayerId(), 1);
  EXPECT_EQ(tanks[0].getPosition(), Point(1, 0));

  EXPECT_EQ(tanks[1].getPlayerId(), 1);
  EXPECT_EQ(tanks[1].getPosition(), Point(2, 2));
  
  EXPECT_EQ(tanks[2].getPlayerId(), 2);
  EXPECT_EQ(tanks[2].getPosition(), Point(3, 3));
}

// Test tank creation with multiple tanks for both players
TEST_F(GameManagerTest, Initialize_MultipleTanksBothPlayers) {
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 5, {
      "#12##",  // First tanks
      "#   #",
      "# 1 #",  // Duplicate player 1
      "#  2#",  // Duplicate player 2
      "#####"
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  EXPECT_TRUE(manager.readBoard(tempFilePath));
  
  const auto& tanks = getTanks(manager);
  ASSERT_EQ(tanks.size(), 4);
  
  // Should keep the first tanks found
  EXPECT_EQ(tanks[0].getPlayerId(), 1);
  EXPECT_EQ(tanks[0].getPosition(), Point(1, 0));

  EXPECT_EQ(tanks[1].getPlayerId(), 2);
  EXPECT_EQ(tanks[1].getPosition(), Point(2, 0));
  
  EXPECT_EQ(tanks[2].getPlayerId(), 1);
  EXPECT_EQ(tanks[2].getPosition(), Point(2, 2));

  EXPECT_EQ(tanks[3].getPlayerId(), 2);
  EXPECT_EQ(tanks[3].getPosition(), Point(3, 3));
}

TEST_F(GameManagerTest, Initialize_CreatesErrorFile) {
  // Create a board with a recoverable error (extra row)
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 4, {
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"  // Extra row
  });
  createTestBoardFile(boardLines);
  
  // Remove any existing error file first
  std::remove("input_errors.txt");
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  EXPECT_TRUE(manager.readBoard(tempFilePath));
  
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
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 5, {
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"
  });
  createTestBoardFile(boardLines);
  
  // Remove any existing error file first
  std::remove("input_errors.txt");
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  EXPECT_TRUE(manager.readBoard(tempFilePath));
  
  // Check that error file was not created
  std::ifstream errorFile("input_errors.txt");
  EXPECT_FALSE(errorFile.is_open());
}

TEST_F(GameManagerTest, GetPlayerAction_ClearShot) {
  // Create a board where tanks are in a direct line of sight
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 5, {
      "#####",
      "#2 1#",  // Player 1 and 2 in same row with clear line of sight
      "#   #",
      "#   #",
      "#####"
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(manager.readBoard(tempFilePath));
  
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
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 5, {
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(manager.readBoard(tempFilePath));
  
  // Test with invalid player IDs
  Action invalidAction1 = testGetPlayerAction(manager, 0);
  Action invalidAction2 = testGetPlayerAction(manager, 3);
  
  // Should return None for invalid IDs
  EXPECT_EQ(invalidAction1, Action::None);
  EXPECT_EQ(invalidAction2, Action::None);
}


// Test logAction adds entries correctly
TEST_F(GameManagerTest, LogAction_FormatsCorrectly) {
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  
  // Log a valid action using the test helper method
  testLogAction(manager, 1, Action::MoveForward, true);
  
  // Check log entry format
  const auto& log = getGameLog(manager);
  ASSERT_EQ(log.size(), 1);
  EXPECT_EQ(log[0], "Player 1: Move Forward - Success");
  
  // Log an invalid action
  testLogAction(manager, 2, Action::Shoot, false);
  
  // Check both log entries
  ASSERT_EQ(log.size(), 2);
  EXPECT_EQ(log[1], "Player 2: Shoot - Bad Step");
}

// Test getGameLog accessor with multiple entries
TEST_F(GameManagerTest, GetGameLog_MultipleEntries) {
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  
  // Log several actions
  testLogAction(manager, 1, Action::MoveForward, true);
  testLogAction(manager, 2, Action::RotateLeftEighth, false);
  testLogAction(manager, 1, Action::Shoot, true);
  
  // Get the log
  const auto& log = getGameLog(manager);
  
  // Check that the log contains all entries
  ASSERT_EQ(log.size(), 3);
  EXPECT_EQ(log[0], "Player 1: Move Forward - Success");
  EXPECT_EQ(log[1], "Player 2: Rotate Left 1/8 - Bad Step");
  EXPECT_EQ(log[2], "Player 1: Shoot - Success");
}

TEST_F(GameManagerTest, MoveShellsOnce_ShellMovementAndCollision) {
  // Create a board with tank 1 to the right of tank 2, with 2 spaces between
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 3, 7, {
      "#######",
      "#2   1#", // Tank 2 at (1,1), Tank 1 at (5,1)
      "#######"
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(manager.readBoard(tempFilePath));
  
  // Verify tank positions and directions
  auto initialTanks = getTanks(manager);
  ASSERT_EQ(initialTanks.size(), 2);
  EXPECT_EQ(initialTanks[0].getPosition(), Point(1, 1)); // Tank 2
  EXPECT_EQ(initialTanks[1].getPosition(), Point(5, 1)); // Tank 1
  
  // Player 2 shoots (facing right)
  testApplyAction(manager, 2, Action::Shoot);
  
  // Get the shell
  auto shellsBeforeMove = getShells(manager);
  ASSERT_EQ(shellsBeforeMove.size(), 1);
  EXPECT_EQ(shellsBeforeMove[0].getPosition(), Point(1, 1));
  
  // Move shell once
  testMoveShellsOnce(manager);
  
  // Verify shell moved one step right
  auto shellsAfterFirstMove = getShells(manager);
  ASSERT_EQ(shellsAfterFirstMove.size(), 1);
  EXPECT_EQ(shellsAfterFirstMove[0].getPosition(), Point(2, 1));
}

TEST_F(GameManagerTest, ApplyAction_DoNothing) {
  // Create a basic test board 
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 8, getStandardBoard());
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(manager.readBoard(tempFilePath));
  
  // Capture initial state
  auto initialTanks = getTanks(manager);
  ASSERT_EQ(initialTanks.size(), 2);
  Point initialPos1 = initialTanks[1].getPosition(); // Player 1
  Point initialPos2 = initialTanks[0].getPosition(); // Player 2
  
  // Verify positions and directions are unchanged
  auto finalTanks = getTanks(manager);
  EXPECT_EQ(finalTanks[1].getPosition(), initialPos1);
  EXPECT_EQ(finalTanks[0].getPosition(), initialPos2);

  testApplyAction(manager, 1, Action::MoveForward);
  testApplyAction(manager, 2, Action::MoveForward);

  Point newPosition1 = Point(5, 1);
  Point newPosition2 = Point(2, 1);
  
  EXPECT_EQ(getTanks(manager)[1].getPreviousPosition(), initialPos1);
  EXPECT_EQ(getTanks(manager)[0].getPreviousPosition(), initialPos2);
  EXPECT_EQ(getTanks(manager)[1].getPosition(), newPosition1);
  EXPECT_EQ(getTanks(manager)[0].getPosition(), newPosition2);

  // Apply "do nothing" action for both players
  testApplyAction(manager, 1, Action::None);
  testApplyAction(manager, 2, Action::None);

  EXPECT_TRUE(getTanks(manager)[1].getPosition() == newPosition1);
  EXPECT_TRUE(getTanks(manager)[0].getPosition() == newPosition2);
  EXPECT_TRUE(getTanks(manager)[1].getPreviousPosition() == newPosition1);
  EXPECT_TRUE(getTanks(manager)[0].getPreviousPosition() == newPosition2);
  
  // Check that the action was logged
  const auto& log = getGameLog(manager);
  ASSERT_GE(log.size(), 2);
  EXPECT_TRUE(log[log.size()-2].find("Player 1: None - Success") != std::string::npos);
  EXPECT_TRUE(log[log.size()-1].find("Player 2: None - Success") != std::string::npos);
}

TEST_F(GameManagerTest, MoveShellsOnce_EdgeWrapping) {
  // Create a board with tank 1 at position (5,1) and open edges for wrapping
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 3, 7, {
      "       ", // No walls at top
      "  1    ", // Tank 1 at position (2,1)
      "      2"  // No walls at bottom
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(manager.readBoard(tempFilePath));
  
  // Verify tank 1 position
  auto initialTanks = getTanks(manager);
  ASSERT_EQ(initialTanks.size(), 2);
  EXPECT_EQ(initialTanks[0].getPosition(), Point(2, 1)); // Tank 1
  
  // Player 1 shoots (towards left)
  testApplyAction(manager, 1, Action::Shoot);
  
  // Move shell once - should be at (1,1)
  testMoveShellsOnce(manager);
  EXPECT_EQ(getShells(manager)[0].getPosition(), Point(1, 1));
  
  // Move shell again - should be at (0,1)
  testMoveShellsOnce(manager);
  EXPECT_EQ(getShells(manager)[0].getPosition(), Point(0, 1));
  
  // Move shell again - should wrap to the right edge (6,1)
  testMoveShellsOnce(manager);
  EXPECT_EQ(getShells(manager)[0].getPosition(), Point(6, 1));
  
  // Move shell again - should be at (5,1)
  testMoveShellsOnce(manager);
  EXPECT_EQ(getShells(manager)[0].getPosition(), Point(5, 1));
}

// Test checkGameOver with one tank destroyed
TEST_F(GameManagerTest, CheckGameOver_OneTankDestroyed) {
  // Create a valid test board file
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 5, {
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(manager.readBoard(tempFilePath));
  
  // Manually set one tank to destroyed state
  std::vector<Tank>& tanks = getTanksMutable(manager);
  tanks[0].destroy();
  
  // Test checkGameOver
  bool gameOver = testCheckGameOver(manager);
  
  EXPECT_TRUE(gameOver);
  EXPECT_EQ(testGetGameResult(manager), "Player 2 wins - Enemy tank destroyed");
}

// Test checkGameOver with both tanks destroyed
TEST_F(GameManagerTest, CheckGameOver_BothTanksDestroyed) {
  // Create a valid test board file
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 5, {
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(manager.readBoard(tempFilePath));
  
  // Destroy both tanks
  std::vector<Tank>& tanks = getTanksMutable(manager);
  tanks[0].destroy();
  tanks[1].destroy();
  
  // Test checkGameOver
  bool gameOver = testCheckGameOver(manager);
  
  EXPECT_TRUE(gameOver);
  EXPECT_EQ(testGetGameResult(manager), "Tie - Both tanks destroyed");
}

// Test checkGameOver with shells depleted and steps exhausted
TEST_F(GameManagerTest, CheckGameOver_ShellsDepleted) {
  // Create a valid test board file
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 5, {
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(manager.readBoard(tempFilePath));
  
  // Set the remaining steps counter to negative
  testSetRemainingSteps(manager, -1);
  
  // Test checkGameOver
  bool gameOver = testCheckGameOver(manager);
  
  EXPECT_TRUE(gameOver);
  EXPECT_EQ(testGetGameResult(manager), "Tie - Maximum steps reached after shells depleted");
}

// Test checkGameOver with game still in progress
TEST_F(GameManagerTest, CheckGameOver_GameInProgress) {
  // Create a valid test board file
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 5, {
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(manager.readBoard(tempFilePath));
  
  // No tanks destroyed, steps not exhausted
  std::vector<Tank>& tanks = getTanksMutable(manager);
  ASSERT_FALSE(tanks[0].isDestroyed());
  ASSERT_FALSE(tanks[1].isDestroyed());
  testSetRemainingSteps(manager, 40); // default value
  
  // Test checkGameOver
  bool gameOver = testCheckGameOver(manager);
  
  EXPECT_FALSE(gameOver);
  // Game should not be over yet
}

// Test saveResults writes game log to output file
TEST_F(GameManagerTest, SaveResults_WritesGameLog) {
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  
  // Add some entries to the game log
  testLogAction(manager, 1, Action::MoveForward, true);
  testLogAction(manager, 2, Action::Shoot, false);
  testLogAction(manager, 1, Action::RotateLeftQuarter, true);
  
  std::string outputFilePath = "test_output.txt";
  
  // Save the results
  testSaveResults(manager, outputFilePath);
  
  // Read the output file and verify its contents
  std::ifstream outputFile(outputFilePath);
  ASSERT_TRUE(outputFile.is_open());
  
  std::vector<std::string> fileContents;
  std::string line;
  while (std::getline(outputFile, line)) {
      fileContents.push_back(line);
  }
  
  // Check file contents match game log
  const auto& gameLog = getGameLog(manager);
  ASSERT_EQ(fileContents.size(), gameLog.size());
  
  for (size_t i = 0; i < gameLog.size(); ++i) {
      EXPECT_EQ(fileContents[i], gameLog[i]);
  }
  
  // Clean up
  outputFile.close();
  std::remove(outputFilePath.c_str());
}

// Test saveResults with empty game log
TEST_F(GameManagerTest, SaveResults_EmptyGameLog) {
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  
  // No log entries added, should create an empty file
  std::string outputFilePath = "test_empty_output.txt";
  
  testSaveResults(manager, outputFilePath);
  
  // Check that the file was created but is empty
  std::ifstream outputFile(outputFilePath);
  ASSERT_TRUE(outputFile.is_open());
  
  std::string line;
  std::getline(outputFile, line);
  bool hasContent = !outputFile.fail() && !line.empty();
  EXPECT_FALSE(hasContent);
  
  // Clean up
  outputFile.close();
  std::remove(outputFilePath.c_str());
}

// Test saveResults with invalid file path
TEST_F(GameManagerTest, SaveResults_InvalidFilePath) {
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  
  // Add some entries to the game log
  testLogAction(manager, 1, Action::MoveForward, true);
  
  // Redirect cerr to capture error output
  std::stringstream cerr_buffer;
  std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
  
  // Attempt to save to an invalid path (on most systems, writing to /proc/invalid will fail)
  // If this doesn't reliably fail on your system, consider using a different invalid path
  testSaveResults(manager, "/proc/invalid/output.txt");
  
  // Restore cerr
  std::cerr.rdbuf(old_cerr);
  
  // Check error message
  std::string output = cerr_buffer.str();
  EXPECT_TRUE(output.find("Error: Could not open output file") != std::string::npos);
}

TEST_F(GameManagerTest, ProcessStep_shellStartsNextToTank) {
  // Create a simple test board with two tanks in clear view of each other
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 3, 10, {
      "##########",
      "#2      1#", // Tanks are in the same row with clear line of sight
      "# ###### #"
    // 0123456789 (cell indexes)
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(manager.readBoard(tempFilePath));
  
  // Both tanks Shoot
  testProcessStep(manager);
  
  EXPECT_EQ(getShells(manager).size(), 2);
  EXPECT_EQ(getShells(manager)[0].getPosition(), Point(7, 1));
  EXPECT_EQ(getShells(manager)[1].getPosition(), Point(2, 1));

  testProcessStep(manager);

  EXPECT_EQ(getShells(manager).size(), 2);
  EXPECT_EQ(getShells(manager)[0].getPosition(), Point(5, 1));
  EXPECT_EQ(getShells(manager)[1].getPosition(), Point(4, 1));
  EXPECT_EQ(getTanks(manager)[0].getDirection(), Direction::Down);
  EXPECT_EQ(getTanks(manager)[1].getDirection(), Direction::Down);
}

TEST_F(GameManagerTest, ProcessStep_shellsCollide) {
  // Create a simple test board with two tanks in clear view of each other
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 3, 10, {
      "##########",
      "#2      1#", // Tanks are in the same row with clear line of sight
      "# ###### #"
    // 0123456789 (cell indexes)
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(manager.readBoard(tempFilePath));
  
  // Both tanks Shoot
  testProcessStep(manager);
  
  EXPECT_EQ(getShells(manager).size(), 2);

  testProcessStep(manager);

  EXPECT_EQ(getShells(manager).size(), 2);

  testProcessStep(manager);
  EXPECT_EQ(getShells(manager).size(), 0);
  // Tanks hide from shells
  EXPECT_EQ(getTanks(manager)[1].getPosition(), Point(8, 2));
  EXPECT_EQ(getTanks(manager)[0].getPosition(), Point(1, 2));
}

TEST_F(GameManagerTest, ProcessStep_waitForCooldown) {
  // Create a simple test board with two tanks in clear view of each other
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 3, 27, {
      "###########################",
      "#2                       1#", // Tanks are in the same row with clear line of sight
      "###########################"
    // 012345678901234567890123456 (cell indexes)
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  mockAlgo1->setConstantAction(Action::Shoot);
  mockAlgo2->setConstantAction(Action::None);
  ASSERT_TRUE(initializeManager(manager, boardLines));
  
  testProcessStep(manager);
  
  EXPECT_EQ(getShells(manager).size(), 1);

  for (int i = 1; i <= Tank::SHOOT_COOLDOWN; ++i) {
    // tanks can't shoot due to cooldown
    testProcessStep(manager);
    EXPECT_EQ(getShells(manager).size(), 1);
    EXPECT_EQ(getTanks(manager)[0].getPosition(), Point(1, 1));
    EXPECT_EQ(getTanks(manager)[1].getPosition(), Point(25, 1));
  }
  EXPECT_EQ(getTanks(manager)[1].getDirection(), Direction::Left);
  EXPECT_EQ(getTanks(manager)[0].getDirection(), Direction::Right);
  testProcessStep(manager);
  // tanks can shoot again (only one of the tanks is in direction)
  EXPECT_EQ(getShells(manager).size(), 2);
}

TEST_F(GameManagerTest, GameManagerTest_maximumSteps) {
  // Create a simple test board with two tanks in clear view of each other
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 3, 6, {
      "######",
      "#2##1#", // Tanks are in the same row with clear line of sight
      "######"
    // 012345678901234567890123456 (cell indexes)
  });
  createTestBoardFile(boardLines);
  
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(manager.readBoard(tempFilePath));
    
  testRunGame(manager);
  EXPECT_EQ(getGameSteps(manager), 1000);
}

TEST_F(GameManagerTest, RunGame_TanksShooting) {
  mockAlgo1->setConstantAction(Action::Shoot);
  mockAlgo2->setConstantAction(Action::Shoot);
  
  // Initialize manager
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 8, getStandardBoard());
  ASSERT_TRUE(initializeManager(manager, boardLines));
  
  // Run the game
  manager.run();
  
  // Verify game results
  const auto& gameLog = getGameLog(manager);
  
  // Check that the game ended (either a win or tie)
  bool gameEndedFound = false;
  for (const auto& logEntry : gameLog) {
      if (logEntry.find("Game ended") != std::string::npos) {
          gameEndedFound = true;
          break;
      }
  }
  EXPECT_TRUE(gameEndedFound);
  bool p1ShootFound = false;
  bool p2ShootFound = false;
  
  for (const auto& logEntry : gameLog) {
      if (logEntry.find("Player 1: Shoot - Success") != std::string::npos) {
          p1ShootFound = true;
      }
      if (logEntry.find("Player 2: Shoot - Success") != std::string::npos) {
          p2ShootFound = true;
      }
  }
  
  EXPECT_TRUE(p1ShootFound);
  EXPECT_TRUE(p2ShootFound);
}

TEST_F(GameManagerTest, RunGame_TankHitsMine) {
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 8, {
      "########",
      "#2  @ 1#", // Tanks are in the same row with clear line of sight
      "#      #", // Mine in the middle
      "#      #",
      "########"
  });
  
  // Setup mock algorithm for player 1 to move down toward the mine
  mockAlgo1->setActionSequence({
      Action::MoveForward,        
      Action::MoveForward,        
      Action::RotateLeftQuarter,  
      Action::MoveForward        
  });
  
  // Player 2 does nothing
  mockAlgo2->setConstantAction(Action::None);
  
  // Initialize manager
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(initializeManager(manager, boardLines));
  
  // Run the game
  manager.run();
  
  // Verify game results
  const auto& gameLog = getGameLog(manager);
  
  // Check that player 2 won because player 1 hit a mine
  bool p2WinFound = false;
  for (const auto& logEntry : gameLog) {
      if (logEntry.find("Player 2 wins") != std::string::npos) {
          p2WinFound = true;
          break;
      }
  }
  
  EXPECT_TRUE(p2WinFound);
}

TEST_F(GameManagerTest, RunGame_TankHitsWall) {
  // Setup algorithm for player 1 to try to move into a wall
  mockAlgo1->setActionSequence({
      Action::RotateRightQuarter,  // Face up
      Action::MoveForward         // Try to move into wall
  });
  
  // Player 2 does nothing
  mockAlgo2->setConstantAction(Action::None);
  
  // Initialize manager
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 8, getStandardBoard());
  ASSERT_TRUE(initializeManager(manager, boardLines));
  
  testProcessStep(manager, 3);

  // Verify game results
  const auto& gameLog = getGameLog(manager);
  
  // Check that the move into the wall was blocked
  bool badStepFound = false;
  for (const auto& logEntry : gameLog) {
      if (logEntry.find("Player 1: Move Forward - Bad Step") != std::string::npos) {
          badStepFound = true;
          break;
      }
  }
  
  EXPECT_TRUE(badStepFound);
  
  // Tank 1 position should still be in the same row (not in the wall)
  EXPECT_EQ(getTanks(manager)[1].getPosition().getY(), 1);
}

TEST_F(GameManagerTest, RunGame_TanksTryToOccupySameSpace) {
  // Create a board with tanks next to each other
  auto tightBoard = makeBoardFile("Test Board", 1000, 20, 3, 10, {
      "##########",
      "#21      #", // Tanks are right next to each other
      "##########"
  });
  
  // Setup algorithms to try to move into each other's space
  mockAlgo1->setConstantAction(Action::MoveForward); // Player 1 moves left
  mockAlgo2->setConstantAction(Action::MoveForward);  // Player 2 moves right
  
  // Initialize manager
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(initializeManager(manager, tightBoard));
  
  // Run the game
  manager.run();
  
  // Verify game results
  const auto& gameLog = getGameLog(manager);
  
  // Since they try to move to the same spot, we should eventually see
  // either both destroyed or a bad step
  bool collision = false;
  for (const auto& logEntry : gameLog) {
      if (logEntry.find("Both tanks destroyed") != std::string::npos) {
          collision = true;
          break;
      }
  }
  
  EXPECT_TRUE(collision);
  EXPECT_TRUE(getTanks(manager)[0].isDestroyed());
  EXPECT_TRUE(getTanks(manager)[1].isDestroyed());
}

TEST_F(GameManagerTest, RunGame_ShellDestroyingWallsThenTank) {
  // Create a board with a wall between the tanks
  auto walledBoard = makeBoardFile("Test Board", 1000, 20, 3, 7, {
      "#######",
      "#2#  1#", // Wall between tanks
      "#######"
  });
  
  // Setup algorithms to shoot constantly
  mockAlgo1->setConstantAction(Action::Shoot);
  mockAlgo2->setConstantAction(Action::None);
  
  // Initialize manager
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(initializeManager(manager, walledBoard));
  
  // Run the game
  manager.run();
  
  // Verify game results
  const auto& gameLog = getGameLog(manager);
  
  // The wall should eventually be destroyed after multiple hits
  // Then one tank should hit the other
  bool tankDestroyed = false;
  
  for (const auto& logEntry : gameLog) {
      if (logEntry.find("Player") != std::string::npos && 
          logEntry.find("wins") != std::string::npos) {
          tankDestroyed = true;
      }
  }
  
  // We expect the game to eventually end with tank destruction
  // but wall destruction is harder to verify in logs, so we'll skip that check
  EXPECT_TRUE(tankDestroyed); // Only way to destroy tank is to destroy wall first
}

// Test complex sequences including backward movement
TEST_F(GameManagerTest, RunGame_BackwardMovementSequence) {
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 3, 10, {
      "##########",
      "#         #",
      "#2  @1    #", // Player 1 at (5,2)
      "##########"
  });
  
  // Setup algorithm for player 1 with a sequence including backward moves
  mockAlgo1->setActionSequence({
      Action::MoveBackward,   
      Action::RotateLeftEighth,           
      Action::RotateLeftEighth,           
      Action::MoveBackward,   // Should move backwards
      Action::MoveBackward,       // Should move backwards again
      Action::Shoot,              // Shoot 
      Action::None, Action::None, Action::None, Action::None, // Wait           
  });
  
  // Player 2 does nothing
  mockAlgo2->setConstantAction(Action::None);
  
  // Initialize manager
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(initializeManager(manager, boardLines));

  EXPECT_EQ(getTanks(manager)[1].getPlayerId(), 1);
  EXPECT_EQ(getTanks(manager)[1].getPosition(), Point(5, 2));
  testProcessStep(manager, 2);
  EXPECT_EQ(getTanks(manager)[1].getPosition(), Point(5, 2));
  testProcessStep(manager);
  EXPECT_EQ(getTanks(manager)[1].getPosition(), Point(6, 2));
  testProcessStep(manager);
  EXPECT_EQ(getTanks(manager)[1].getPosition(), Point(7, 2));
  testProcessStep(manager);
  EXPECT_EQ(getTanks(manager)[1].getPosition(), Point(8, 2));
  testProcessStep(manager, 4);
  ASSERT_TRUE(testCheckGameOver(manager));
  EXPECT_EQ(testGetGameResult(manager), "Player 1 wins - Enemy tank destroyed");  
}

// Test shells colliding with each other
TEST_F(GameManagerTest, RunGame_ShellsCollide) {
  // Setup both players to rotate and shoot in a way that shells will collide
  mockAlgo1->setActionSequence({
      Action::RotateLeftQuarter,  // Face up
      Action::Shoot,              // Shoot upward
      Action::None                // Wait
  });
  
  mockAlgo2->setActionSequence({
      Action::RotateRightQuarter, // Face down  
      Action::Shoot,              // Shoot downward
      Action::None                // Wait
  });
  
  // Initialize manager
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 5, 8, getStandardBoard());
  ASSERT_TRUE(initializeManager(manager, boardLines));
  
  testProcessStep(manager, 2);
  
  // Check the shell count - shells should eventually destroy each other
  bool shellsCreated = false;
  bool shellsDestroyed = false;
  
  // We first expect there to be shells (after shooting)
  if (!getShells(manager).empty()) {
      shellsCreated = true;
  }
  
  testProcessStep(manager, 6);
  
  // Now we expect shells to be gone (after collision)
  if (getShells(manager).empty()) {
      shellsDestroyed = true;
  }
  
  EXPECT_TRUE(shellsCreated);
  EXPECT_TRUE(shellsDestroyed);
}

// Test a full game until both tanks are out of shells
TEST_F(GameManagerTest, RunGame_OutOfShells) {
  auto boardLines = makeBoardFile("Test Board", 1000, 20, 4, 18, {
      "2#################",
      "#                #",
      "#   @            #",  
      "#################1"
  });
  
  mockAlgo1->setConstantAction(Action::Shoot);
  mockAlgo2->setConstantAction(Action::Shoot);
  
  // Initialize manager
  GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
  ASSERT_TRUE(initializeManager(manager, boardLines));
  
  // Run the full game
  manager.run();
  
  // Verify game results
  const auto& gameLog = getGameLog(manager);
  
  // Check that the game ended in a tie due to shells depleted
  bool outOfShellsTie = false;
  for (const auto& logEntry : gameLog) {
      if (logEntry.find("shells depleted") != std::string::npos) {
          outOfShellsTie = true;
          break;
      }
  }
  ASSERT_EQ(testGetGameResult(manager), "Tie - Maximum steps reached after shells depleted");
  EXPECT_TRUE(outOfShellsTie);
  
  // Verify all shells were used
  for (const auto& tank : getTanks(manager)) {
      EXPECT_EQ(tank.getRemainingShells(), 0);
  }
  EXPECT_EQ(getGameSteps(manager), 116);
}

TEST_F(GameManagerTest, OutputFileCreatedInCurrentDir) {
    GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
    std::string inputFile = "test_board_output.txt";
    auto boardLines = makeBoardFile("TestOutput", 100, 10, 3, 3, {
        "###",
        "#1#",
        "###"
    });
    std::ofstream dummy(inputFile);
    for (const auto& line : boardLines) dummy << line << '\n';
    dummy.close();
    manager.readBoard(inputFile);
    std::string expected = "output_test_board_output.txt";
    manager.run();
    std::ifstream outputFile(expected);
    EXPECT_TRUE(outputFile.is_open());
    outputFile.close();
    std::remove(inputFile.c_str());
    std::remove(expected.c_str());
}

TEST_F(GameManagerTest, OutputFileCreatedInSubDir) {
    GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
    std::string dir = "test_output_dir";
    std::string inputFile = dir + "/test_board_output2.txt";
    std::filesystem::create_directory(dir);
    auto boardLines = makeBoardFile("TestOutput", 100, 10, 3, 3, {
        "###",
        "#2#",
        "###"
    });
    std::ofstream dummy(inputFile);
    for (const auto& line : boardLines) dummy << line << '\n';
    dummy.close();
    manager.readBoard(inputFile);
    std::string expected = dir + "/output_test_board_output2.txt";
    manager.run();
    std::ifstream outputFile(expected);
    EXPECT_TRUE(outputFile.is_open());
    outputFile.close();
    std::remove(inputFile.c_str());
    std::remove(expected.c_str());
    std::filesystem::remove(dir);
}

TEST_F(GameManagerTest, Initialize_ManyTanksPerPlayer_IntertwinedOrder) {
    // 5x2 board, alternating tanks left-to-right, top-to-bottom
    // 1 2 1 2 1
    // 2 1 2 1 2
    auto boardLines = makeBoardFile("Test Board", 1000, 20, 2, 5, {
        "12121",
        "21212"
    });
    createTestBoardFile(boardLines);
    GameManager manager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
    ASSERT_TRUE(manager.readBoard(tempFilePath));
    const auto& tanks = getTanks(manager);
    ASSERT_EQ(tanks.size(), 10);
    // Expected order: (x, y) left-to-right, top-to-bottom
    std::vector<std::pair<int, Point>> expected = {
        {1, Point(0,0)}, {2, Point(1,0)}, {1, Point(2,0)}, {2, Point(3,0)}, {1, Point(4,0)},
        {2, Point(0,1)}, {1, Point(1,1)}, {2, Point(2,1)}, {1, Point(3,1)}, {2, Point(4,1)}
    };
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tanks[i].getPlayerId(), expected[i].first) << "Tank " << i << " wrong player";
        EXPECT_EQ(tanks[i].getPosition(), expected[i].second) << "Tank " << i << " wrong position";
    }
}

