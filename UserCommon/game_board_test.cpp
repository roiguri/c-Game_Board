#include "gtest/gtest.h"
#include "game_board.h"
#include <vector>
#include <string>
#include <map>
#include "utils/point.h"

using namespace UserCommon_098765432_123456789;

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
    void createBoardWithContent(const std::vector<std::string>& content,
                                std::vector<std::pair<int, Point>>& tankPositions) {
        board = GameBoard(content[0].length(), content.size());
        board.initialize(content, tankPositions);
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
  
  // Redirect cerr to capture output
  std::stringstream cerr_buffer;
  std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
  std::vector<std::pair<int, Point>> tankPositions;
  
  EXPECT_FALSE(board.initialize(emptyLines, tankPositions));
  
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
  
  // Redirect cerr to capture output
  std::stringstream cerr_buffer;
  std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
  std::vector<std::pair<int, Point>> tankPositions;
  EXPECT_NO_THROW({
    board.initialize(boardLines, tankPositions);
  });
  // Restore cerr
  std::cerr.rdbuf(old_cerr);
}

TEST_F(GameBoardTest, Initialize_MissingTank2) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1  #",  // Only tank 1 is present
      "#   #",
      "# @ #",
      "#####"
  };
  
  // Redirect cerr to capture output
  std::stringstream cerr_buffer;
  std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
  std::vector<std::pair<int, Point>> tankPositions;
  EXPECT_NO_THROW({
    board.initialize(boardLines, tankPositions);
  });
  // Restore cerr
  std::cerr.rdbuf(old_cerr);
}

TEST_F(GameBoardTest, Initialize_MultipleTanks) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1 2#",
      "#1 2#",  // Multiple tanks for both players
      "# @ #",
      "#####"
  };
  
  // Add tankPositions map
  std::vector<std::pair<int, Point>> tankPositions;
  EXPECT_TRUE(board.initialize(boardLines, tankPositions));
  
  Point tank1Pos = tankPositions[0].second;
  Point tank2Pos = tankPositions[1].second;
  
  // Should match the first tanks' positions
  EXPECT_EQ(tank1Pos, Point(1, 1));
  EXPECT_EQ(tank2Pos, Point(3, 1));
  
  // Check that the second tanks are present and in order
  ASSERT_EQ(tankPositions.size(), 4);
  EXPECT_EQ(tankPositions[2].second, Point(1, 2));
  EXPECT_EQ(tankPositions[3].second, Point(3, 2));
  
  // The tanks' cells should be empty
  EXPECT_EQ(board.getCellType(1, 2), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(3, 2), GameBoard::CellType::Empty);
}

TEST_F(GameBoardTest, Initialize_ValidBoardLines) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1 2#",
      "#   #",
      "# @ #",
      "#####"
  };
  
  // Add tankPositions map
  std::vector<std::pair<int, Point>> tankPositions;
  EXPECT_TRUE(board.initialize(boardLines, tankPositions));
  
  // Check specific cells
  EXPECT_EQ(board.getCellType(0, 0), GameBoard::CellType::Wall);
  EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Empty); //TANK1
  EXPECT_EQ(board.getCellType(3, 1), GameBoard::CellType::Empty); //TANK2
  EXPECT_EQ(board.getCellType(2, 3), GameBoard::CellType::Mine);
  EXPECT_EQ(board.getCellType(2, 2), GameBoard::CellType::Empty);
  
  // Check wall health
  EXPECT_EQ(board.getWallHealth(Point(0, 0)), GameBoard::WALL_STARTING_HEALTH);
  
  // No error validation needed - handled by Simulator
}

TEST_F(GameBoardTest, Initialize_IncompleteRows) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1 2#",
      "#   #"
  };
  
  // Add tankPositions map
  std::vector<std::pair<int, Point>> tankPositions;
  EXPECT_TRUE(board.initialize(boardLines, tankPositions));
  
  // Check specific cells
  EXPECT_EQ(board.getCellType(0, 0), GameBoard::CellType::Wall);
  EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(3, 1), GameBoard::CellType::Empty);
  
  // Check that missing rows are filled with empty cells
  EXPECT_EQ(board.getCellType(0, 3), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(0, 4), GameBoard::CellType::Empty);
}

TEST_F(GameBoardTest, Initialize_IncompleteColumns) {
  std::vector<std::string> boardLines = {
      "###",
      "#12",
      "#  ",
      "## ",
      "###"      
  };
  
  // Add tankPositions map  
  std::vector<std::pair<int, Point>> tankPositions;
  EXPECT_TRUE(board.initialize(boardLines, tankPositions));
  
  // Check cells that are defined
  EXPECT_EQ(board.getCellType(0, 0), GameBoard::CellType::Wall);
  EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(2, 1), GameBoard::CellType::Empty);
  
  // Check that missing columns are filled with empty cells
  EXPECT_EQ(board.getCellType(3, 0), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(4, 0), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(3, 4), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(4, 4), GameBoard::CellType::Empty);
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
  
  // Add tankPositions map
  std::vector<std::pair<int, Point>> tankPositions;
  EXPECT_TRUE(board.initialize(boardLines, tankPositions));
  
  // Check valid cells are correct
  EXPECT_EQ(board.getCellType(0, 0), GameBoard::CellType::Wall);
  EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(3, 1), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(2, 3), GameBoard::CellType::Mine);
}

TEST_F(GameBoardTest, Initialize_ExtraColumns) {
  std::vector<std::string> boardLines = {
      "#####XX",
      "#1 2#XX",
      "#   #X",
      "# @ #", 
      "#####" 
  };
  
  // Add tankPositions map
  std::vector<std::pair<int, Point>> tankPositions;
  EXPECT_TRUE(board.initialize(boardLines, tankPositions));
  
  // Check valid cells are correct
  EXPECT_EQ(board.getCellType(0, 0), GameBoard::CellType::Wall);
  EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(3, 1), GameBoard::CellType::Empty);
  EXPECT_EQ(board.getCellType(2, 3), GameBoard::CellType::Mine);
}

TEST_F(GameBoardTest, Initialize_UnrecognizedCharacters) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1X2#",
      "#   #",
      "# @ #",
      "#####"
  };
  
  // Add tankPositions map
  std::vector<std::pair<int, Point>> tankPositions;
  EXPECT_TRUE(board.initialize(boardLines, tankPositions));
  
  // The unrecognized character should be treated as empty
  EXPECT_EQ(board.getCellType(2, 1), GameBoard::CellType::Empty);
}

TEST_F(GameBoardTest, Initialize_NoTanksReturnsFalse) {
  std::vector<std::string> boardLines = {
      "#####",
      "#   #",  // No tanks, only walls, empty spaces, and mines
      "# @ #",
      "#   #",
      "#####"
  };
  std::vector<std::pair<int, Point>> tankPositions;
  
  // Redirect cerr to capture the error output
  std::stringstream cerr_buffer;
  std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
  
  // Should return false because no tanks are present
  EXPECT_FALSE(board.initialize(boardLines, tankPositions));
  
  // Restore cerr
  std::cerr.rdbuf(old_cerr);
  
  // Verify the error message was printed
  std::string output = cerr_buffer.str();
  EXPECT_TRUE(output.find("Error: No tanks found on the board.") != std::string::npos);
  
  // Verify tankPositions is empty
  EXPECT_TRUE(tankPositions.empty());
  
  // The board should still be initialized with the other elements
  EXPECT_EQ(board.getCellType(0, 0), GameBoard::CellType::Wall);
  EXPECT_EQ(board.getCellType(2, 2), GameBoard::CellType::Mine);
  EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Empty);
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
    
    // Add tankPositions map
    std::vector<std::pair<int, Point>> tankPositions;
    createBoardWithContent(boardLines, tankPositions);
    
    EXPECT_EQ(board.getCellType(0, 0), GameBoard::CellType::Wall);
    EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Empty);
    EXPECT_EQ(board.getCellType(3, 1), GameBoard::CellType::Empty);
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
    
    // Add tankPositions map
    std::vector<std::pair<int, Point>> tankPositions;
    createBoardWithContent(boardLines, tankPositions);

    // Test wrapping - should get same result as (0, 0)
    EXPECT_EQ(board.getCellType(5, 0), GameBoard::CellType::Wall);
    EXPECT_EQ(board.getCellType(0, 5), GameBoard::CellType::Wall);
    EXPECT_EQ(board.getCellType(-5, 0), GameBoard::CellType::Wall);
    EXPECT_EQ(board.getCellType(0, -5), GameBoard::CellType::Wall);
}

// SetCellType Tests
TEST_F(GameBoardTest, SetCellType_ValidPosition) {
    // Set cells to different types
    board.setCellType(Point(2, 2), GameBoard::CellType::Wall);
    board.setCellType(Point(0, 4), GameBoard::CellType::Mine);
    
    // Verify the cells were set correctly
    EXPECT_EQ(board.getCellType(2, 2), GameBoard::CellType::Wall);
    EXPECT_EQ(board.getCellType(0, 4), GameBoard::CellType::Mine);
}

TEST_F(GameBoardTest, SetCellType_WrappedPosition) {
    // Set cells using wrapped positions
    board.setCellType(boardWidth + 1, 1, GameBoard::CellType::Mine);
    board.setCellType(-2, 3, GameBoard::CellType::Mine);
    
    // Verify the cells were set correctly at their wrapped positions
    EXPECT_EQ(board.getCellType(1, 1), GameBoard::CellType::Mine);
    EXPECT_EQ(board.getCellType(3, 3), GameBoard::CellType::Mine);
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
    
    // Add tankPositions map
    std::vector<std::pair<int, Point>> tankPositions;
    createBoardWithContent(boardLines, tankPositions);

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
    
    // Add tankPositions map
    std::vector<std::pair<int, Point>> tankPositions;
    createBoardWithContent(boardLines, tankPositions);
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
    EXPECT_EQ(wrapped.getX(), 2);
    EXPECT_EQ(wrapped.getY(), 3);
}

TEST_F(GameBoardTest, WrapPosition_OutsideBoundsPositive) {
    Point original(7, 8);
    Point wrapped = board.wrapPosition(original);
    EXPECT_EQ(wrapped.getX(), 2);
    EXPECT_EQ(wrapped.getY(), 3);
}

TEST_F(GameBoardTest, WrapPosition_OutsideBoundsNegative) {
    Point original(-3, -2);
    Point wrapped = board.wrapPosition(original);
    EXPECT_EQ(wrapped.getX(), 2);
    EXPECT_EQ(wrapped.getY(), 3);
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
    
    // Add tankPositions map
    std::vector<std::pair<int, Point>> tankPositions;
    createBoardWithContent(boardLines, tankPositions);

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
    
    // Add tankPositions map
    std::vector<std::pair<int, Point>> tankPositions;
    createBoardWithContent(boardLines, tankPositions);
    
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
    
    // Add tankPositions map
    std::vector<std::pair<int, Point>> tankPositions;
    createBoardWithContent(boardLines, tankPositions);
    
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
    
    // Add tankPositions map
    std::vector<std::pair<int, Point>> tankPositions;
    createBoardWithContent(boardLines, tankPositions);

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
    
    // Add tankPositions map
    std::vector<std::pair<int, Point>> tankPositions;
    createBoardWithContent(boardLines, tankPositions);

    std::string expected = "# #\n @ \n###\n";
    EXPECT_EQ(board.toString(), expected);
}

TEST_F(GameBoardTest, ZeroDimensionBoard_DoesNotCrash) {
    GameBoard zeroBoard(0, 0);
    // Should not crash or throw
    EXPECT_EQ(zeroBoard.getWidth(), 0);
    EXPECT_EQ(zeroBoard.getHeight(), 0);
    // wrapPosition should return the input
    Point p(5, 7);
    EXPECT_EQ(zeroBoard.wrapPosition(p), Point(-1, -1));
}

TEST_F(GameBoardTest, StepDistance_NoWrapping_StraightAndDiagonal) {
    board = GameBoard(10, 10);
    // Horizontal
    EXPECT_EQ(GameBoard::stepDistance(Point(0, 0), Point(3, 0), board.getWidth(), board.getHeight()), 3);
    // Vertical
    EXPECT_EQ(GameBoard::stepDistance(Point(0, 0), Point(0, 4), board.getWidth(), board.getHeight()), 4);
    // Diagonal
    EXPECT_EQ(GameBoard::stepDistance(Point(0, 0), Point(3, 3), board.getWidth(), board.getHeight()), 3);
    // Same point
    EXPECT_EQ(GameBoard::stepDistance(Point(2, 2), Point(2, 2), board.getWidth(), board.getHeight()), 0);
}

TEST_F(GameBoardTest, StepDistance_Wrapping_XAxis) {
    board = GameBoard(5, 5);
    // Wrapping horizontally: (0,0) to (4,0) is 1 step (wrap)
    EXPECT_EQ(GameBoard::stepDistance(Point(0, 0), Point(4, 0), board.getWidth(), board.getHeight()), 1);
    // Wrapping horizontally: (1,1) to (4,1) is 2 steps (min(3,2))
    EXPECT_EQ(GameBoard::stepDistance(Point(1, 1), Point(4, 1), board.getWidth(), board.getHeight()), 2);
}

TEST_F(GameBoardTest, StepDistance_Wrapping_YAxis) {
    board = GameBoard(5, 5);
    // Wrapping vertically: (0,0) to (0,4) is 1 step (wrap)
    EXPECT_EQ(GameBoard::stepDistance(Point(0, 0), Point(0, 4), board.getWidth(), board.getHeight()), 1);
    // Wrapping vertically: (2,1) to (2,4) is 2 steps (min(3,2))
    EXPECT_EQ(GameBoard::stepDistance(Point(2, 1), Point(2, 4), board.getWidth(), board.getHeight()), 2);
}

TEST_F(GameBoardTest, StepDistance_Wrapping_BothAxes) {
    board = GameBoard(3, 3);
    // (0,0) to (2,2): dx=1 (wrap), dy=1 (wrap) => 1
    EXPECT_EQ(GameBoard::stepDistance(Point(0, 0), Point(2, 2), board.getWidth(), board.getHeight()), 1);
    // (1,1) to (2,2): dx=1, dy=1 => 1
    EXPECT_EQ(GameBoard::stepDistance(Point(1, 1), Point(2, 2), board.getWidth(), board.getHeight()), 1);
}

TEST_F(GameBoardTest, StepDistance_EdgeCases) {
    board = GameBoard(3, 3);
    // Same point
    EXPECT_EQ(GameBoard::stepDistance(Point(1, 1), Point(1, 1), board.getWidth(), board.getHeight()), 0);
    // Opposite corners with wrapping
    EXPECT_EQ(GameBoard::stepDistance(Point(0, 0), Point(2, 2), board.getWidth(), board.getHeight()), 1);
    // Center to edge
    EXPECT_EQ(GameBoard::stepDistance(Point(1, 1), Point(0, 1), board.getWidth(), board.getHeight()), 1);
}

