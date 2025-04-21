#include "gtest/gtest.h"
#include "game_board.h"
#include <vector>
#include <string>

// Test fixture for GameBoard tests
class GameBoardTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Default setup creates a 5x5 board
        boardWidth = 5;
        boardHeight = 5;
        board = GameBoard(boardWidth, boardHeight);
    }

    // Common test data
    int boardWidth;
    int boardHeight;
    GameBoard board;
    
    // Helper method to create a board with specific content
    void createBoardWithContent(const std::vector<std::string>& content) {
        board = GameBoard(content[0].length(), content.size());
        std::vector<std::string> errors;
        board.initialize(content, errors);
    }
};

// Constructor Tests
TEST_F(GameBoardTest, Constructor_DefaultCreatesEmptyBoard) {
    GameBoard emptyBoard;
    EXPECT_EQ(emptyBoard.getWidth(), 0);
    EXPECT_EQ(emptyBoard.getHeight(), 0);
}

TEST_F(GameBoardTest, Constructor_ParameterizedCreatesCorrectSize) {
    EXPECT_EQ(board.getWidth(), boardWidth);
    EXPECT_EQ(board.getHeight(), boardHeight);
    
    // Check that all cells are initialized as empty
    for (int y = 0; y < boardHeight; ++y) {
        for (int x = 0; x < boardWidth; ++x) {
            EXPECT_EQ(board.getCellType(x, y), GameBoard::CellType::Empty);
        }
    }
}

// Initialize Tests
TEST_F(GameBoardTest, Initialize_EmptyBoardLines) {
  std::vector<std::string> emptyLines;
  std::vector<std::string> errors;
  
  // Redirect cerr to capture output
  std::stringstream cerr_buffer;
  std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
  
  EXPECT_FALSE(board.initialize(emptyLines, errors));
  
  // Restore cerr
  std::cerr.rdbuf(old_cerr);
  
  // Check error message
  std::string output = cerr_buffer.str();
  EXPECT_TRUE(output.find("Error: Input board is empty") != std::string::npos);
}

TEST_F(GameBoardTest, Initialize_MissingTank1) {
  std::vector<std::string> boardLines = {
      "#####",
      "#  2#",  // Only tank 2 is present
      "#   #",
      "# @ #",
      "#####"
  };
  std::vector<std::string> errors;
  
  // Redirect cerr to capture output
  std::stringstream cerr_buffer;
  std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
  
  EXPECT_FALSE(board.initialize(boardLines, errors));
  
  // Restore cerr
  std::cerr.rdbuf(old_cerr);
  
  // Check error message
  std::string output = cerr_buffer.str();
  EXPECT_TRUE(output.find("Error: No tank found for player 1") != std::string::npos);
}

TEST_F(GameBoardTest, Initialize_MissingTank2) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1  #",  // Only tank 1 is present
      "#   #",
      "# @ #",
      "#####"
  };
  std::vector<std::string> errors;
  
  // Redirect cerr to capture output
  std::stringstream cerr_buffer;
  std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
  
  EXPECT_FALSE(board.initialize(boardLines, errors));
  
  // Restore cerr
  std::cerr.rdbuf(old_cerr);
  
  // Check error message
  std::string output = cerr_buffer.str();
  EXPECT_TRUE(output.find("Error: No tank found for player 2") != std::string::npos);
}

TEST_F(GameBoardTest, Initialize_MultipleTanks) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1 2#",
      "#1 2#",  // Multiple tanks for both players
      "# @ #",
      "#####"
  };
  std::vector<std::string> errors;
  
  EXPECT_TRUE(board.initialize(boardLines, errors));
  
  // Check that the error messages were captured
  bool foundPlayer1Error = false;
  bool foundPlayer2Error = false;
  
  for (const auto& error : errors) {
      if (error.find("Multiple tanks for player 1") != std::string::npos) {
          foundPlayer1Error = true;
      }
      if (error.find("Multiple tanks for player 2") != std::string::npos) {
          foundPlayer2Error = true;
      }
  }
  
  EXPECT_TRUE(foundPlayer1Error);
  EXPECT_TRUE(foundPlayer2Error);
  
  // Check that only the first tanks were kept
  // Find the positions of tanks
  Point tank1Pos(-1, -1);
  Point tank2Pos(-1, -1);
  
  // TODO: consider adding find tanks method to GameBoard
  for (int y = 0; y < board.getHeight(); ++y) {
      for (int x = 0; x < board.getWidth(); ++x) {
          if (board.getCellType(x, y) == GameBoard::CellType::Tank1) {
              tank1Pos = Point(x, y);
          } else if (board.getCellType(x, y) == GameBoard::CellType::Tank2) {
              tank2Pos = Point(x, y);
          }
      }
  }
  
  // Should match the first tanks' positions
  EXPECT_EQ(tank1Pos, Point(1, 1));
  EXPECT_EQ(tank2Pos, Point(3, 1));
  
  // The second tanks should have been ignored
  EXPECT_NE(board.getCellType(1, 2), GameBoard::CellType::Tank1);
  EXPECT_NE(board.getCellType(3, 2), GameBoard::CellType::Tank2);
}

TEST_F(GameBoardTest, Initialize_ValidBoardLines) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1 2#",
      "#   #",
      "# @ #",
      "#####"
  };
  std::vector<std::string> errors;
  
  EXPECT_TRUE(board.initialize(boardLines, errors));
  
  // Check specific cells
  EXPECT_EQ(board.getCellType(0, 0), GameBoard::CellType::Wall);
  EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Tank1);
  EXPECT_EQ(board.getCellType(3, 1), GameBoard::CellType::Tank2);
  EXPECT_EQ(board.getCellType(2, 3), GameBoard::CellType::Mine);
  EXPECT_EQ(board.getCellType(2, 2), GameBoard::CellType::Empty);
  
  // Check wall health
  EXPECT_EQ(board.getWallHealth(Point(0, 0)), GameBoard::WALL_STARTING_HEALTH);
  
  // Expect no errors
  EXPECT_TRUE(errors.empty());
}

TEST_F(GameBoardTest, Initialize_IncompleteRows) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1 2#",
      "#   #"
  };
  std::vector<std::string> errors;
  
  EXPECT_TRUE(board.initialize(boardLines, errors));
  
  // Check specific cells
  EXPECT_EQ(board.getCellType(0, 0), GameBoard::CellType::Wall);
  EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Tank1);
  EXPECT_EQ(board.getCellType(3, 1), GameBoard::CellType::Tank2);
  
  // Check that missing rows are filled with empty cells
  EXPECT_EQ(board.getCellType(0, 3), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(0, 4), GameBoard::CellType::Empty);
  
  EXPECT_EQ(errors.size(), 2);
  
  // Check for errors about missing rows
  int missingRowErrorCount = 0;
  for (const auto& error : errors) {
      if (error.find("Missing row") != std::string::npos) {
          missingRowErrorCount++;
      }
  }
  EXPECT_EQ(missingRowErrorCount, 2);
}

TEST_F(GameBoardTest, Initialize_IncompleteColumns) {
  std::vector<std::string> boardLines = {
      "###",
      "#12",
      "#  ",
      "## ",
      "###"      
  };
  std::vector<std::string> errors;
  
  EXPECT_TRUE(board.initialize(boardLines, errors));
  
  // Check cells that are defined
  EXPECT_EQ(board.getCellType(0, 0), GameBoard::CellType::Wall);
  EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Tank1);
  EXPECT_EQ(board.getCellType(2, 1), GameBoard::CellType::Tank2);
  
  // Check that missing columns are filled with empty cells
  EXPECT_EQ(board.getCellType(3, 0), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(4, 0), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(3, 4), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(4, 4), GameBoard::CellType::Empty);
  
  // Check the error count
  EXPECT_EQ(errors.size(), 5);
  
  // Check for errors about shorter lines
  int shortRowErrorCount = 0;
  for (const auto& error : errors) {
      if (error.find("shorter than expected width") != std::string::npos) {
          shortRowErrorCount++;
      }
  }
  EXPECT_EQ(shortRowErrorCount, 5);
}

TEST_F(GameBoardTest, Initialize_ExtraRows) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1 2#",
      "#   #",
      "# @ #",
      "#####", 
      "XXXXX",  
      "XXXXX"  
  };
  std::vector<std::string> errors;
  
  EXPECT_TRUE(board.initialize(boardLines, errors));
  
  // Check valid cells are correct
  EXPECT_EQ(board.getCellType(0, 0), GameBoard::CellType::Wall);
  EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Tank1);
  EXPECT_EQ(board.getCellType(3, 1), GameBoard::CellType::Tank2);
  EXPECT_EQ(board.getCellType(2, 3), GameBoard::CellType::Mine);
  
  // Check the error count
  EXPECT_EQ(errors.size(), 1);
  
  // Verify the error message
  EXPECT_EQ(errors[0], "Input has more rows than expected height. Extra rows ignored.");
}

TEST_F(GameBoardTest, Initialize_ExtraColumns) {
  std::vector<std::string> boardLines = {
      "#####XX",
      "#1 2#XX",
      "#   #X",
      "# @ #", 
      "#####" 
  };
  std::vector<std::string> errors;
  
  EXPECT_TRUE(board.initialize(boardLines, errors));
  
  // Check valid cells are correct
  EXPECT_EQ(board.getCellType(0, 0), GameBoard::CellType::Wall);
  EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Tank1);
  EXPECT_EQ(board.getCellType(3, 1), GameBoard::CellType::Tank2);
  EXPECT_EQ(board.getCellType(2, 3), GameBoard::CellType::Mine);
  
  // Check the error count
  EXPECT_EQ(errors.size(), 3);
  
  // Verify the error messages
  int extraColErrorCount = 0;
  for (const auto& error : errors) {
      if (error.find("longer than expected width") != std::string::npos) {
          extraColErrorCount++;
      }
  }
  EXPECT_EQ(extraColErrorCount, 3);
}

TEST_F(GameBoardTest, Initialize_UnrecognizedCharacters) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1X2#",
      "#   #",
      "# @ #",
      "#####"
  };
  std::vector<std::string> errors;
  
  EXPECT_TRUE(board.initialize(boardLines, errors));
  
  // Check if the error about unrecognized character is reported
  bool foundUnrecognizedCharError = false;
  for (const auto& error : errors) {
      if (error.find("Unrecognized character 'X'") != std::string::npos) {
          foundUnrecognizedCharError = true;
          break;
      }
  }
  EXPECT_TRUE(foundUnrecognizedCharError);
  
  // The unrecognized character should be treated as empty
  EXPECT_EQ(board.getCellType(2, 1), GameBoard::CellType::Empty);
}

// GetCellType Tests
TEST_F(GameBoardTest, GetCellType_ValidPosition) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    };
    
    createBoardWithContent(boardLines);
    
    EXPECT_EQ(board.getCellType(0, 0), GameBoard::CellType::Wall);
    EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Tank1);
    EXPECT_EQ(board.getCellType(3, 1), GameBoard::CellType::Tank2);
    EXPECT_EQ(board.getCellType(Point(2, 3)), GameBoard::CellType::Mine);
}

TEST_F(GameBoardTest, GetCellType_WrappedPosition) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    };
    
    createBoardWithContent(boardLines);

    // Test wrapping - should get same result as (0, 0)
    EXPECT_EQ(board.getCellType(5, 0), GameBoard::CellType::Wall);
    EXPECT_EQ(board.getCellType(0, 5), GameBoard::CellType::Wall);
    EXPECT_EQ(board.getCellType(-5, 0), GameBoard::CellType::Wall);
    EXPECT_EQ(board.getCellType(0, -5), GameBoard::CellType::Wall);
}

// SetCellType Tests
TEST_F(GameBoardTest, SetCellType_ValidPosition) {
    // Set cells to different types
    board.setCellType(1, 1, GameBoard::CellType::Tank1);
    board.setCellType(3, 3, GameBoard::CellType::Tank2);
    board.setCellType(Point(2, 2), GameBoard::CellType::Wall);
    board.setCellType(Point(0, 4), GameBoard::CellType::Mine);
    
    // Verify the cells were set correctly
    EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Tank1);
    EXPECT_EQ(board.getCellType(3, 3), GameBoard::CellType::Tank2);
    EXPECT_EQ(board.getCellType(2, 2), GameBoard::CellType::Wall);
    EXPECT_EQ(board.getCellType(0, 4), GameBoard::CellType::Mine);
}

TEST_F(GameBoardTest, SetCellType_WrappedPosition) {
    // Set cells using wrapped positions
    board.setCellType(boardWidth + 1, 1, GameBoard::CellType::Tank1);
    board.setCellType(-2, 3, GameBoard::CellType::Tank2);
    
    // Verify the cells were set correctly at their wrapped positions
    EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Tank1);
    EXPECT_EQ(board.getCellType(3, 3), GameBoard::CellType::Tank2);
}

// IsWall Tests
TEST_F(GameBoardTest, IsWall_WallAndNonWall) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    };
    
    createBoardWithContent(boardLines);

    // Check wall positions
    EXPECT_TRUE(board.isWall(Point(0, 0)));
    EXPECT_TRUE(board.isWall(Point(4, 4)));
    
    // Check non-wall positions
    EXPECT_FALSE(board.isWall(Point(1, 1)));
    EXPECT_FALSE(board.isWall(Point(2, 2)));
    EXPECT_FALSE(board.isWall(Point(2, 3)));
}

// DamageWall Tests
TEST_F(GameBoardTest, DamageWall_WallDestructionAfterTwoHits) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    };
    
    createBoardWithContent(boardLines);
    Point wallPosition(0, 0);
    
    // First hit - wall should be damaged but not destroyed
    EXPECT_FALSE(board.damageWall(wallPosition));
    EXPECT_EQ(board.getWallHealth(wallPosition), 1);
    EXPECT_TRUE(board.isWall(wallPosition));
    
    // Second hit - wall should be destroyed
    EXPECT_TRUE(board.damageWall(wallPosition));
    EXPECT_EQ(board.getWallHealth(wallPosition), 0);
    EXPECT_FALSE(board.isWall(wallPosition));
    EXPECT_EQ(board.getCellType(wallPosition), GameBoard::CellType::Empty);
}

TEST_F(GameBoardTest, DamageWall_NonWall) {
    // Try to damage a position that doesn't have a wall
    Point emptyPosition(2, 2);
    EXPECT_FALSE(board.isWall(emptyPosition));
    EXPECT_FALSE(board.damageWall(emptyPosition));
}

// GetWallHealth Tests
TEST_F(GameBoardTest, GetWallHealth_WallAndNonWall) {
    board.setCellType(1, 1, GameBoard::CellType::Wall);
    
    // Check health of a wall
    EXPECT_EQ(board.getWallHealth(Point(1, 1)), 2);
    
    // Check health of a non-wall
    EXPECT_EQ(board.getWallHealth(Point(2, 2)), 0);
    
    // Damage the wall and check health again
    board.damageWall(Point(1, 1));
    EXPECT_EQ(board.getWallHealth(Point(1, 1)), 1);
}

// WrapPosition Tests
TEST_F(GameBoardTest, WrapPosition_InsideBounds) {
    Point original(2, 3);
    Point wrapped = board.wrapPosition(original);
    EXPECT_EQ(wrapped.x, 2);
    EXPECT_EQ(wrapped.y, 3);
}

TEST_F(GameBoardTest, WrapPosition_OutsideBoundsPositive) {
    Point original(7, 8);
    Point wrapped = board.wrapPosition(original);
    EXPECT_EQ(wrapped.x, 2);
    EXPECT_EQ(wrapped.y, 3);
}

TEST_F(GameBoardTest, WrapPosition_OutsideBoundsNegative) {
    Point original(-3, -2);
    Point wrapped = board.wrapPosition(original);
    EXPECT_EQ(wrapped.x, 2);
    EXPECT_EQ(wrapped.y, 3);
}

// CanMoveTo Tests
TEST_F(GameBoardTest, CanMoveTo_EmptySpace) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    };
    
    createBoardWithContent(boardLines);

    // Should be able to move to empty space
    EXPECT_TRUE(board.canMoveTo(Point(2, 2)));
}

TEST_F(GameBoardTest, CanMoveTo_Mine) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    };
    
    createBoardWithContent(boardLines);
    
    // Should not be able to move to a mine
    EXPECT_TRUE(board.canMoveTo(Point(2, 3)));
}

TEST_F(GameBoardTest, CanMoveTo_Wall) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    };
    
    createBoardWithContent(boardLines);
    
    // Should not be able to move to a wall
    EXPECT_FALSE(board.canMoveTo(Point(0, 0)));
}

TEST_F(GameBoardTest, CanMoveTo_Tank) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    };
    createBoardWithContent(boardLines);

    // Should be able to move to another tank's position (this will trigger a collision)
    EXPECT_TRUE(board.canMoveTo(Point(1, 1))); // Tank1's position
}

// ToString Tests
TEST_F(GameBoardTest, ToString_EmptyBoard) {
    GameBoard smallBoard(3, 2);
    std::string expected = "   \n   \n";
    EXPECT_EQ(smallBoard.toString(), expected);
}

TEST_F(GameBoardTest, ToString_PopulatedBoard) {
    std::vector<std::string> boardLines = {
        "# #",
        "1@2",
        "###"
    };
    createBoardWithContent(boardLines);

    std::string expected = "# #\n1@2\n###\n";
    EXPECT_EQ(board.toString(), expected);
}