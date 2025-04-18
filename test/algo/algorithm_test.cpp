#include "gtest/gtest.h"
#include "algo/algorithm.h"
#include "game_board.h"
#include "tank.h"
#include "shell.h"

class MockAlgorithm : public Algorithm {
public:
    MockAlgorithm() : Algorithm() {}
    
    virtual Action getNextAction(
        const GameBoard& gameBoard,
        const Tank& myTank,
        const Tank& enemyTank,
        const std::vector<Shell>& shells
    ) override {
        // Simple implementation for testing
        return Action::None;
    }

    Direction* testHasDirectLineOfSight(
      const GameBoard& gameBoard,
      const Point& from,
      const Point& to
    ) const {
        return hasDirectLineOfSight(gameBoard, from, to);
    }

    bool testHasLineOfSightInDirection(
      const GameBoard& gameBoard,
      const Point& from,
      const Point& to,
      Direction direction
    ) const {
        return hasLineOfSightInDirection(gameBoard, from, to, direction);
    }

    bool testIsPositionInDangerFromShells(
      const GameBoard& gameBoard,
      const Point& position,
      const std::vector<Shell>& shells,
      int stepsToCheck = 3
    ) const {
        return isPositionInDangerFromShells(gameBoard, position, shells, stepsToCheck);
    }

    Action testFindSafeAction(
      const GameBoard& gameBoard,
      const Tank& tank,
      const std::vector<Shell>& shells
    ) const {
        return findSafeAction(gameBoard, tank, shells);
    }

    bool testCanShootEnemy(
      const GameBoard& board,
      const Tank& myTank,
      const Tank& enemyTank
    ) {
        return canShootEnemy(board, myTank, enemyTank);
    }
};

class AlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockAlgorithm = new MockAlgorithm();
    }
    
    void TearDown() override {
        delete mockAlgorithm;
    }
    
    MockAlgorithm* mockAlgorithm;
};

GameBoard create5X5TestBoard(const std::vector<std::string>& boardLines) {
    GameBoard board(5, 5);
    std::vector<std::string> errors;
    board.initialize(boardLines, errors);
    return board;
}

TEST_F(AlgorithmTest, Constructor) {
    Algorithm* testAlgorithm = new MockAlgorithm();
    EXPECT_NE(testAlgorithm, nullptr);
    delete testAlgorithm;
}

TEST_F(AlgorithmTest, CreateAlgorithmNotImplemented) {
    Algorithm* algorithm = Algorithm::createAlgorithm("chase");
    EXPECT_EQ(algorithm, nullptr);
    
    algorithm = Algorithm::createAlgorithm("defensive");
    EXPECT_EQ(algorithm, nullptr);
    
    algorithm = Algorithm::createAlgorithm("invalid");
    EXPECT_EQ(algorithm, nullptr);
}

TEST_F(AlgorithmTest, GetNextAction) {
    // Create test objects
    GameBoard board(10, 10);
    Tank myTank(1, Point(1, 1), Direction::Right);
    Tank enemyTank(2, Point(8, 8), Direction::Left);
    std::vector<Shell> shells;
    
    // Test that our mock implementation returns the expected action
    Action result = mockAlgorithm->getNextAction(board, myTank, enemyTank, shells);
    EXPECT_EQ(result, Action::None);
}

TEST_F(AlgorithmTest, HasDirectLineOfSight_BasicClear) {
    // Create a board specific to this test
    std::vector<std::string> boardLines = {
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
    };
    GameBoard board = create5X5TestBoard(boardLines);
    
    Direction* dir1 = mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 1), Point(3, 1));
    EXPECT_NE(dir1, nullptr);
    EXPECT_EQ(*dir1, Direction::Right);
    delete dir1;
    
    // No line of sight
    Direction* dir3 = mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 1), Point(3, 2));
    EXPECT_EQ(dir3, nullptr);
}

TEST_F(AlgorithmTest, HasDirectLineOfSight_BlockedHorizontal) {
    std::vector<std::string> boardLines = {
        "#####",
        "# # #",
        "#   #",
        "# # #",
        "#####"
    };
    GameBoard board = create5X5TestBoard(boardLines);
    
    Direction* dir = mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 1), Point(3, 1));
    EXPECT_EQ(dir, nullptr);
}

TEST_F(AlgorithmTest, HasDirectLineOfSight_BlockedVertical) {
    std::vector<std::string> boardLines = {
        "#####",
        "#   #",
        "## ##",
        "#   #",
        "#####"
    };
    GameBoard board = create5X5TestBoard(boardLines);
    
    Direction* dir = mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 1), Point(1, 3));
    EXPECT_EQ(dir, nullptr);
}

TEST_F(AlgorithmTest, HasDirectLineOfSight_BlockedDiagonal) {
    std::vector<std::string> boardLines = {
        "#####",
        "#   #",
        "# # #",
        "#   #",
        "#####"
    };
    GameBoard board = create5X5TestBoard(boardLines);
    
    Direction* dir = mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 1), Point(3, 3));
    EXPECT_EQ(dir, nullptr);
}

TEST_F(AlgorithmTest, HasDirectLineOfSight_WrappingHorizontal) {
    std::vector<std::string> boardLines = {
        "#####",
        "#####",
        "  #  ",
        "#####",
        "#####"
    };
    GameBoard board = create5X5TestBoard(boardLines);
    
    Direction* dir = mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 2), Point(3, 2));
    EXPECT_NE(dir, nullptr);
    EXPECT_EQ(*dir, Direction::Left);
    delete dir;
}

TEST_F(AlgorithmTest, HasDirectLineOfSight_WrappingVertical) {
  std::vector<std::string> boardLines = {
      "## ##",
      "## ##",
      "#####",
      "## ##",
      "## ##"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  
  Direction* dir = mockAlgorithm->testHasDirectLineOfSight(board, Point(2, 1), Point(2, 3));
  EXPECT_NE(dir, nullptr);
  EXPECT_EQ(*dir, Direction::Up);
  delete dir;
}

TEST_F(AlgorithmTest, HasDirectLineOfSight_WrappingDiagonal) {
  std::vector<std::string> boardLines = {
    "#### ",
    "### #",
    "#####",
    "# ###",
    " ####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  
  Direction* dir = mockAlgorithm->testHasDirectLineOfSight(board, Point(3, 1), Point(1, 3));
  EXPECT_NE(dir, nullptr);
  EXPECT_EQ(*dir, Direction::UpRight);
  delete dir;
}

TEST_F(AlgorithmTest, HasLineOfSightInDirection_CorrectDirection) {
  std::vector<std::string> boardLines = {
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  
  // Test with correct directions
  EXPECT_TRUE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(1, 2), Point(3, 2), Direction::Right));
  EXPECT_TRUE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(2, 1), Point(2, 3), Direction::Down));
  EXPECT_TRUE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(1, 1), Point(3, 3), Direction::DownRight));
}

TEST_F(AlgorithmTest, HasLineOfSightInDirection_WrongDirection) {
  std::vector<std::string> boardLines = {
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  
  EXPECT_FALSE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(1, 2), Point(3, 2), Direction::Left));
  EXPECT_FALSE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(2, 1), Point(2, 3), Direction::Up));
  EXPECT_FALSE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(1, 1), Point(3, 3), Direction::UpRight));
}

TEST_F(AlgorithmTest, HasLineOfSightInDirection_BlockedPath) {
  std::vector<std::string> boardLines = {
      "#####",
      "#   #",
      "# # #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  
  EXPECT_FALSE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(1, 2), Point(3, 2), Direction::Right));
  EXPECT_FALSE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(2, 1), Point(2, 3), Direction::Down));
  EXPECT_FALSE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(1, 1), Point(3, 3), Direction::DownRight));
}

TEST_F(AlgorithmTest, HasLineOfSightInDirection_Wrapping) {
  // Create a 5x5 board with no walls
  std::vector<std::string> boardLines = {
      "     ",
      "     ",
      "     ",
      "     ",
      "     "
  };
  GameBoard board = create5X5TestBoard(boardLines);
  
  EXPECT_TRUE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(4, 2), Point(0, 2), Direction::Right));
  EXPECT_TRUE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(2, 4), Point(2, 0), Direction::Down));
  EXPECT_TRUE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(4, 4), Point(0, 0), Direction::DownRight));
}

TEST_F(AlgorithmTest, HasLineOfSightInDirection_SamePoint) {
  std::vector<std::string> boardLines = {
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  
  EXPECT_TRUE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(2, 2), Point(2, 2), Direction::Right));
  EXPECT_TRUE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(2, 2), Point(2, 2), Direction::Down));
  EXPECT_TRUE(mockAlgorithm->testHasLineOfSightInDirection(board, Point(2, 2), Point(2, 2), Direction::UpLeft));
}

TEST_F(AlgorithmTest, IsPositionInDangerFromShells_NoShells) {
  std::vector<std::string> boardLines = {
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Point position(2, 2);
  std::vector<Shell> shells;
  
  EXPECT_FALSE(mockAlgorithm->testIsPositionInDangerFromShells(board, position, shells));
}

TEST_F(AlgorithmTest, IsPositionInDangerFromShells_DestroyedShell) {
  std::vector<std::string> boardLines = {
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Point position(2, 2);
  
  std::vector<Shell> shells;
  Shell shell(2, Point(3, 2), Direction::Left);
  shell.destroy();
  shells.push_back(shell);
  
  EXPECT_FALSE(mockAlgorithm->testIsPositionInDangerFromShells(board, position, shells));
}

TEST_F(AlgorithmTest, IsPositionInDangerFromShells_DirectHit) {
  std::vector<std::string> boardLines = {
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Point position(2, 2);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(2, 2), Direction::Left)); // Shell at same position as tank
  
  EXPECT_TRUE(mockAlgorithm->testIsPositionInDangerFromShells(board, position, shells));
}

TEST_F(AlgorithmTest, IsPositionInDangerFromShells_ImmediateImpact) {
  std::vector<std::string> boardLines = {
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Point position(2, 2);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(3, 2), Direction::Left));
  
  EXPECT_TRUE(mockAlgorithm->testIsPositionInDangerFromShells(board, position, shells, 1));
}

TEST_F(AlgorithmTest, IsPositionInDangerFromShells_FutureImpact) {
  std::vector<std::string> boardLines = {
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Point position(2, 2);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(1, 2), Direction::Right));
  
  EXPECT_FALSE(mockAlgorithm->testIsPositionInDangerFromShells(board, position, shells, 0));
  EXPECT_TRUE(mockAlgorithm->testIsPositionInDangerFromShells(board, position, shells, 1));
}

TEST_F(AlgorithmTest, IsPositionInDangerFromShells_DiagonalApproach) {
  std::vector<std::string> boardLines = {
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Point position(2, 2);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(3, 3), Direction::UpLeft));
  
  EXPECT_TRUE(mockAlgorithm->testIsPositionInDangerFromShells(board, position, shells, 2));
}

TEST_F(AlgorithmTest, IsPositionInDangerFromShells_ShellHitsWall) {
  std::vector<std::string> boardLines = {
      "#####",
      "#   #",
      "# # #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Point position(2, 2);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(3, 2), Direction::Left));
  
  EXPECT_FALSE(mockAlgorithm->testIsPositionInDangerFromShells(board, position, shells, 3));
}

TEST_F(AlgorithmTest, IsPositionInDangerFromShells_ShellWrapsAround) {
  std::vector<std::string> boardLines = {
      "     ",
      "     ",
      "     ",
      "     ",
      "     "
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Point position(2, 2);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(0, 2), Direction::Left));
  
  EXPECT_TRUE(mockAlgorithm->testIsPositionInDangerFromShells(board, position, shells, 3));
}

TEST_F(AlgorithmTest, IsPositionInDangerFromShells_MultipleShells) {
  std::vector<std::string> boardLines = {
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Point position(2, 2);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(4, 2), Direction::Left));
  shells.push_back(Shell(2, Point(2, 0), Direction::Down));
  
  EXPECT_TRUE(mockAlgorithm->testIsPositionInDangerFromShells(board, position, shells, 2));
}

TEST_F(AlgorithmTest, FindSafeAction_NoShells) {
  std::vector<std::string> boardLines = {
    "     ",
    "     ",
    "  1  ",
    "     ",
    "     "
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Tank tank(1, Point(2, 2), Direction::Right);
  std::vector<Shell> shells;
  
  EXPECT_EQ(mockAlgorithm->testFindSafeAction(board, tank, shells), Action::None);
}

TEST_F(AlgorithmTest, FindSafeAction_DestroyedTank) {
  std::vector<std::string> boardLines = {
      "     ",
      "     ", 
      "  1S ",
      "     ",
      "     "
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Tank tank(1, Point(2, 2), Direction::Right);
  tank.destroy();
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(3, 2), Direction::Left));
  
  EXPECT_EQ(mockAlgorithm->testFindSafeAction(board, tank, shells), Action::None);
}

TEST_F(AlgorithmTest, FindSafeAction_AlreadySafe) {
  std::vector<std::string> boardLines = {
      "     ",
      "     ", 
      "  1  ",
      "   s ",
      "     "
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Tank tank(1, Point(2, 2), Direction::Right);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(3, 3), Direction::Up));
  
  EXPECT_EQ(mockAlgorithm->testFindSafeAction(board, tank, shells), Action::None);
}

TEST_F(AlgorithmTest, FindSafeAction_ContinueBackwardMovement) {
  std::vector<std::string> boardLines = {
      "## ##",
      "#   #",
      "#   #",
      "#   #",
      "# 1 #"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Tank tank(1, Point(2, 4), Direction::Right);
  
  tank.moveBackward(Point(1, 4));
  EXPECT_TRUE(tank.isMovingBackward());
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(1, 2), Direction::Right));
  
  EXPECT_EQ(mockAlgorithm->testFindSafeAction(board, tank, shells), Action::None);
}

TEST_F(AlgorithmTest, FindSafeAction_MoveForward) {
  std::vector<std::string> boardLines = {
      "     ",
      "     ",
      "  1  ",
      "     ",
      "     "
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Tank tank(1, Point(2, 2), Direction::Down);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(0, 2), Direction::Right));
  
  EXPECT_EQ(mockAlgorithm->testFindSafeAction(board, tank, shells), Action::MoveForward);
}

TEST_F(AlgorithmTest, FindSafeAction_MoveBackward) {
  std::vector<std::string> boardLines = {
      "     ",
      "     ",
      "     ",
      "     ",
      "  1# "
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Tank tank(1, Point(2, 4), Direction::Right);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(2, 0), Direction::Down));
  
  EXPECT_EQ(mockAlgorithm->testFindSafeAction(board, tank, shells), Action::MoveBackward);
}

TEST_F(AlgorithmTest, FindSafeAction_RotateRightEighth) {
  std::vector<std::string> boardLines = {
      "     ",
      "     ",
      " # # ",
      " #1# ",
      " ##  "
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Tank tank(1, Point(2, 3), Direction::Right);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(2, 0), Direction::Down));

  EXPECT_EQ(mockAlgorithm->testFindSafeAction(board, tank, shells), Action::RotateRightEighth);
}

TEST_F(AlgorithmTest, FindSafeAction_RotateLeftEighth) {
  std::vector<std::string> boardLines = {
    "     ",
    "     ",
    " #   ",
    " #1# ",
    " ### "
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Tank tank(1, Point(2, 3), Direction::Right);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(2, 0), Direction::Down));
  
  EXPECT_EQ(mockAlgorithm->testFindSafeAction(board, tank, shells), Action::RotateLeftEighth);
}

TEST_F(AlgorithmTest, FindSafeAction_RotateRightQuarter) {
  std::vector<std::string> boardLines = {
    "     ",
    "     ",
    "  ###",
    "s  1#",
    "  # #"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Tank tank(1, Point(3, 3), Direction::Right);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(0, 3), Direction::Right));
  
  EXPECT_EQ(mockAlgorithm->testFindSafeAction(board, tank, shells), Action::RotateRightQuarter);
}

TEST_F(AlgorithmTest, FindSafeAction_RotateLeftQuarter) {
  std::vector<std::string> boardLines = {
    "     ",
    "     ",
    "# #  ",
    "#1  s",
    "###  "
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Tank tank(1, Point(1, 3), Direction::Right);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(0, 3), Direction::Left));
  
  // Should rotate left by 1/4 to face up
  EXPECT_EQ(mockAlgorithm->testFindSafeAction(board, tank, shells), Action::RotateLeftQuarter);
}

TEST_F(AlgorithmTest, FindSafeAction_NoSafeMove) {
  std::vector<std::string> boardLines = {
      "     ",
      "     ",
      "  1  ",
      "     ",
      "     "
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Tank tank(1, Point(2, 2), Direction::Right);
  
  std::vector<Shell> shells;
  // Shells surrounding the tank from all directions
  for (int x = 1; x <= 3; x++) {
      for (int y = 1; y <= 3; y++) {
          if (x != 2 || y != 2) { // Skip tank's position
              Point shellPos(x, y);
              Direction shellDir;
              if (x < 2) shellDir = Direction::Right;
              else if (x > 2) shellDir = Direction::Left;
              else if (y < 2) shellDir = Direction::Down;
              else shellDir = Direction::Up;
              shells.push_back(Shell(2, shellPos, shellDir));
          }
      }
  }
  
  // No safe move available
  EXPECT_EQ(mockAlgorithm->testFindSafeAction(board, tank, shells), Action::None);
}

TEST_F(AlgorithmTest, FindSafeAction_MoreThanOneRotationNeeded) {
  std::vector<std::string> boardLines = {
    "     ",
    "     ",
    "###  ",
    "#1  s",
    " ##  "
  };
  GameBoard board = create5X5TestBoard(boardLines);
  Tank tank(1, Point(1, 3), Direction::Up);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(0, 3), Direction::Left));
  
  EXPECT_EQ(mockAlgorithm->testFindSafeAction(board, tank, shells), Action::RotateLeftQuarter);
}



TEST_F(AlgorithmTest, CanShootEnemy_NoLineOfSight) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1#2#",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);

  Tank myTank(1, Point(1, 1), Direction::Right);
  Tank enemyTank(2, Point(3, 1), Direction::Left);
  
  EXPECT_FALSE(mockAlgorithm->testCanShootEnemy(board, myTank, enemyTank));
}

TEST_F(AlgorithmTest, CanShootEnemy_LineOfSightWrongDirection) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  
  Tank myTank(1, Point(1, 1), Direction::Down);  
  Tank enemyTank(2, Point(3, 1), Direction::Left);
  
  EXPECT_FALSE(mockAlgorithm->testCanShootEnemy(board, myTank, enemyTank));
}

TEST_F(AlgorithmTest, CanShootEnemy_LineOfSightCorrectDirection) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  
  Tank myTank(1, Point(1, 1), Direction::Right);
  Tank enemyTank(2, Point(3, 1), Direction::Left);
  
  EXPECT_TRUE(mockAlgorithm->testCanShootEnemy(board, myTank, enemyTank));
}

TEST_F(AlgorithmTest, CanShootEnemy_TankDestroyed) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  
  Tank myTank(1, Point(1, 1), Direction::Right);
  myTank.destroy();
  Tank enemyTank(2, Point(3, 1), Direction::Left);
  
  EXPECT_FALSE(mockAlgorithm->testCanShootEnemy(board, myTank, enemyTank));
  
  myTank = Tank(1, Point(1, 1), Direction::Right);
  enemyTank.destroy();
  
  EXPECT_FALSE(mockAlgorithm->testCanShootEnemy(board, myTank, enemyTank));
}

TEST_F(AlgorithmTest, CanShootEnemy_NoShells) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  
  Tank myTank(1, Point(1, 1), Direction::Right);
  // Use all shells
  for (int i = 0; i < Tank::INITIAL_SHELLS; i++) {
      myTank.decrementShells();
  }
  Tank enemyTank(2, Point(3, 1), Direction::Left);
  
  EXPECT_FALSE(mockAlgorithm->testCanShootEnemy(board, myTank, enemyTank));
}

TEST_F(AlgorithmTest, CanShootEnemy_ShootCooldown) {
  std::vector<std::string> boardLines = {
      "#####",
      "#1 2#",
      "#   #",
      "#   #",
      "#####"
  };
  GameBoard board = create5X5TestBoard(boardLines);
  
  Tank myTank(1, Point(1, 1), Direction::Right);
  myTank.shoot();  // Activate the cooldown
  Tank enemyTank(2, Point(3, 1), Direction::Left);
  
  EXPECT_FALSE(mockAlgorithm->testCanShootEnemy(board, myTank, enemyTank));
}

TEST_F(AlgorithmTest, CanShootEnemy_WrappingLineOfSight) {
  std::vector<std::string> boardLines = {
      "     ",
      "1   2",
      "     ",
      "     ",
      "     "
  };
  GameBoard board = create5X5TestBoard(boardLines);
  
  Tank myTank(1, Point(0, 1), Direction::Left);
  Tank enemyTank(2, Point(4, 1), Direction::Right);
  
  EXPECT_TRUE(mockAlgorithm->testCanShootEnemy(board, myTank, enemyTank));
}