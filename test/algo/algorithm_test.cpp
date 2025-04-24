#include "gtest/gtest.h"
#include "algo/algorithm.h"
#include "algo/chase_algorithm.h"
#include "algo/defensive_algorithm.h"
#include "game_board.h"
#include "objects/tank.h"
#include "objects/shell.h"
#include "test/mock_algorithm.h"

class AlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockAlgorithm = new MockAlgorithm();
    }
    
    void TearDown() override {
        delete mockAlgorithm;
    }
    
    MockAlgorithm* mockAlgorithm;

    // Helper to create a test board
    GameBoard create5X5TestBoard(const std::vector<std::string>& boardLines) {
        GameBoard board(5, 5);
        std::vector<std::string> errors;
        std::vector<std::pair<int, Point>> tankPositions;
        board.initialize(boardLines, errors, tankPositions);
        return board;
    }

    // Expose protected methods for testing
    bool testIsInDanger(
        Algorithm* algo,
        const GameBoard& gameBoard,
        const Point& position,
        const std::vector<Shell>& shells,
        int lookAheadSteps = 3
    ) const {
        return algo->isInDanger(gameBoard, position, shells, lookAheadSteps);
    }
  
    std::optional<Direction> testGetLineOfSightDirection(
        Algorithm* algo,
        const GameBoard& gameBoard,
        const Point& from,
        const Point& to
    ) const {
        return algo->getLineOfSightDirection(gameBoard, from, to);
    }
  
    bool testCheckLineOfSightInDirection(
        Algorithm* algo,
        const GameBoard& gameBoard,
        const Point& from,
        const Point& to,
        Direction direction
    ) const {
        return algo->checkLineOfSightInDirection(gameBoard, from, to, direction);
    }
  
    Action testFindOptimalSafeMove(
        Algorithm* algo,
        const GameBoard& gameBoard,
        const Tank& tank,
        const Tank& enemyTank,
        const std::vector<Shell>& shells,
        bool avoidEnemySight = false
    ) const {
        return algo->findOptimalSafeMove(gameBoard, tank, enemyTank, shells, avoidEnemySight);
    }
  
    std::vector<Algorithm::SafeMoveOption> testGetSafeMoveOptions(
        Algorithm* algo,
        const GameBoard& gameBoard,
        const Tank& tank,
        const Tank& enemyTank,
        const std::vector<Shell>& shells,
        bool avoidEnemySight = false
    ) const {
        return algo->getSafeMoveOptions(gameBoard, tank, enemyTank, shells, avoidEnemySight);
    }
  
    bool testIsExposedToEnemy(
        Algorithm* algo,
        const GameBoard& gameBoard,
        const Point& position,
        const Tank& enemyTank
    ) const {
        return algo->isExposedToEnemy(gameBoard, position, enemyTank);
    }
  
    int testCalculateMoveCost(
        Algorithm* algo,
        const Tank& tank,
        const Point& targetPos,
        Direction targetDir
    ) const {
        return algo->calculateMoveCost(tank, targetPos, targetDir);
    }
  
    bool testCanHitTarget(
        Algorithm* algo,
        const GameBoard& board,
        const Tank& myTank,
        const Point& targetPos
    ) const {
        return algo->canHitTarget(board, myTank, targetPos);
    }
  
    Action testGetRotationToDirection(
        Algorithm* algo,
        Direction current,
        Direction target
    ) const {
        return algo->getRotationToDirection(current, target);
    }
  
    Action testEvaluateOffensiveOptions(
        Algorithm* algo,
        const GameBoard& gameBoard,
        const Tank& myTank,
        const Tank& enemyTank
    ) const {
        return algo->evaluateOffensiveOptions(gameBoard, myTank, enemyTank);
    }
};

TEST_F(AlgorithmTest, Constructor) {
    Algorithm* testAlgorithm = new MockAlgorithm();
    EXPECT_NE(testAlgorithm, nullptr);
    delete testAlgorithm;
}

// IsInDanger Tests
TEST_F(AlgorithmTest, IsInDanger_NoShells) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    Point position(2, 2);
    std::vector<Shell> shells;
    
    EXPECT_FALSE(testIsInDanger(mockAlgorithm, board, position, shells));
}

TEST_F(AlgorithmTest, IsInDanger_DirectHit) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    Point position(2, 2);
    
    std::vector<Shell> shells;
    shells.push_back(Shell(2, Point(2, 2), Direction::Left)); // Shell at same position
    
    EXPECT_TRUE(testIsInDanger(mockAlgorithm, board, position, shells));
}

TEST_F(AlgorithmTest, IsInDanger_ShellInPath) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    Point position(2, 2);
    
    std::vector<Shell> shells;
    shells.push_back(Shell(2, Point(3, 2), Direction::Left)); // Shell will hit position
    
    EXPECT_TRUE(testIsInDanger(mockAlgorithm, board, position, shells, 1));
}

TEST_F(AlgorithmTest, IsInDanger_ShellFarInPath) {
  GameBoard board = create5X5TestBoard({
      "## ##",
      "#   #",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  });
  Point position(2, 4);
  
  std::vector<Shell> shells;
  shells.push_back(Shell(2, Point(2, 0), Direction::Down)); // Shell will hit position
  
  EXPECT_TRUE(testIsInDanger(mockAlgorithm, board, position, shells, 2));
}

TEST_F(AlgorithmTest, IsInDanger_ShellBlocked) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "# # #", // Wall at (2, 2)
        "#   #",
        "#####"
    });
    Point position(1, 2);
    
    std::vector<Shell> shells;
    shells.push_back(Shell(2, Point(3, 2), Direction::Left)); // Shell would hit but blocked by wall
    
    EXPECT_FALSE(testIsInDanger(mockAlgorithm, board, position, shells, 2));
}

// Line of Sight Tests
TEST_F(AlgorithmTest, GetLineOfSightDirection_DirectLine) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    
    // Horizontal line of sight
    auto result1 = testGetLineOfSightDirection(mockAlgorithm, board, Point(1, 2), Point(3, 2));
    EXPECT_TRUE(result1.has_value());
    EXPECT_EQ(result1.value(), Direction::Right);
    
    // Vertical line of sight
    auto result2 = testGetLineOfSightDirection(mockAlgorithm, board, Point(2, 1), Point(2, 3));
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(result2.value(), Direction::Down);
    
    // Diagonal line of sight
    auto result3 = testGetLineOfSightDirection(mockAlgorithm, board, Point(1, 1), Point(3, 3));
    EXPECT_TRUE(result3.has_value());
    EXPECT_EQ(result3.value(), Direction::DownRight);
}

TEST_F(AlgorithmTest, GetLineOfSightDirection_BlockedLine) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "# # #", // Wall at (2, 2)
        "#   #",
        "#####"
    });
    
    // Horizontal line blocked
    auto result1 = testGetLineOfSightDirection(mockAlgorithm, board, Point(1, 2), Point(3, 2));
    EXPECT_FALSE(result1.has_value());
    
    // Diagonal line blocked
    auto result2 = testGetLineOfSightDirection(mockAlgorithm, board, Point(1, 1), Point(3, 3));
    EXPECT_FALSE(result2.has_value());
}

TEST_F(AlgorithmTest, CheckLineOfSightInDirection_CorrectDirections) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    
    // Same point
    EXPECT_TRUE(testCheckLineOfSightInDirection(
      mockAlgorithm, board, Point(2, 2), Point(2, 2), Direction::Up));
    
    // Correct directions
    EXPECT_TRUE(testCheckLineOfSightInDirection(
      mockAlgorithm, board, Point(1, 2), Point(3, 2), Direction::Right));
    
    EXPECT_TRUE(testCheckLineOfSightInDirection(
      mockAlgorithm, board, Point(2, 1), Point(2, 3), Direction::Down));
    
    EXPECT_TRUE(testCheckLineOfSightInDirection(
      mockAlgorithm, board, Point(1, 1), Point(3, 3), Direction::DownRight));
    
    // Wrong directions
    EXPECT_FALSE(testCheckLineOfSightInDirection(
      mockAlgorithm, board, Point(1, 2), Point(3, 2), Direction::Left));
    
    EXPECT_FALSE(testCheckLineOfSightInDirection(
      mockAlgorithm, board, Point(1, 1), Point(3, 3), Direction::Up));
}

// Safe Move Tests
TEST_F(AlgorithmTest, GetSafeMoveOptions_AllOptionsSafe) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    
    Tank tank(1, Point(2, 2), Direction::Right);
    Tank enemyTank(2, Point(4, 4), Direction::Left);
    std::vector<Shell> shells;
    
    auto options = testGetSafeMoveOptions(mockAlgorithm, board, tank, enemyTank, shells);
    
    // (7 directions + 1 forward + 1 backward)
    EXPECT_EQ(options.size(), 9);
    
    // Forward should be the cheapest option
    bool foundForward = false;
    for (const auto& option : options) {
        if (option.action == Action::MoveForward) {
            EXPECT_EQ(option.stepCost, 1); // Forward costs 1 step
            foundForward = true;
            break;
        }
    }
    EXPECT_TRUE(foundForward);
    
    // Backward should be the most expensive
    bool foundBackward = false;
    for (const auto& option : options) {
        if (option.action == Action::MoveBackward) {
            EXPECT_EQ(option.stepCost, 3); // Initial backward costs 3 steps
            foundBackward = true;
            break;
        }
    }
    EXPECT_TRUE(foundBackward);
}

TEST_F(AlgorithmTest, GetSafeMoveOptions_SomeOptionsBlocked) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "# # #", // Wall at (2, 2)
        "#   #",
        "#####"
    });
    
    Tank tank(1, Point(1, 2), Direction::Right);
    Tank enemyTank(2, Point(4, 4), Direction::Left);
    std::vector<Shell> shells;
    
    auto options = testGetSafeMoveOptions(mockAlgorithm, board, tank, enemyTank, shells);
    
    // Forward is blocked by wall, so it shouldn't be an option
    for (const auto& option : options) {
        EXPECT_NE(option.action, Action::MoveForward);
    }
}

TEST_F(AlgorithmTest, GetSafeMoveOptions_DangerousOptions) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    
    Tank tank(1, Point(2, 2), Direction::Right);
    Tank enemyTank(2, Point(4, 4), Direction::Left);
    std::vector<Shell> shells;
    
    // Shell that will hit position (3, 2) - forward position
    shells.push_back(Shell(2, Point(4, 2), Direction::Left));
    
    auto options = testGetSafeMoveOptions(mockAlgorithm, board, tank, enemyTank, shells);
    
    // Forward should not be an option as it's dangerous
    for (const auto& option : options) {
        EXPECT_NE(option.position, Point(3, 2));
    }
}

TEST_F(AlgorithmTest, FindOptimalSafeMove_SelectsCheapest) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    
    // Tank currently facing Right
    Tank tank(1, Point(2, 2), Direction::Right);
    Tank enemyTank(2, Point(4, 4), Direction::Left);
    std::vector<Shell> shells;
    
    // Add a shell that endangers the current position but not forward
    shells.push_back(Shell(2, Point(2, 4), Direction::Up));
    
    // Forward should be the optimal move
    Action result = testFindOptimalSafeMove(mockAlgorithm, board, tank, enemyTank, shells);
    EXPECT_EQ(result, Action::MoveForward);
    
    // Now block forward and endanger right side
    shells.clear();
    shells.push_back(Shell(2, Point(4, 2), Direction::Left));
    
    // Rotate to a different direction
    result = testFindOptimalSafeMove(mockAlgorithm, board, tank, enemyTank, shells);
    EXPECT_NE(result, Action::MoveForward);
}

TEST_F(AlgorithmTest, GetSafeMoveOptions_AvoidEnemyTankCollision) {
  GameBoard board = create5X5TestBoard({
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  });
  
  Tank myTank(1, Point(2, 2), Direction::Right);
  Tank enemyTank(2, Point(3, 2), Direction::Left); // Enemy right in front
  std::vector<Shell> shells;
  
  auto options = testGetSafeMoveOptions(mockAlgorithm, board, myTank, enemyTank, shells, false);
  
  // Forward should not be an option due to enemy tank
  for (const auto& option : options) {
      EXPECT_NE(option.position, enemyTank.getPosition());
  }
}

TEST_F(AlgorithmTest, GetSafeMoveOptions_AvoidEnemySight) {
  GameBoard board = create5X5TestBoard({
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  });
  
  Tank myTank(1, Point(1, 2), Direction::Right);
  Tank enemyTank(2, Point(3, 2), Direction::Left); // Enemy with line of sight
  std::vector<Shell> shells;
  
  // First without line of sight avoidance
  auto optionsWithoutAvoidance = testGetSafeMoveOptions(
    mockAlgorithm, board, myTank, enemyTank, shells, false);
  
  // Should include positions in enemy's line of sight
  bool includesExposedPositions = false;
  for (const auto& option : optionsWithoutAvoidance) {
      if (option.position.getX() == 2 && option.position.getY() == 2) {
          includesExposedPositions = true;
          break;
      }
  }
  EXPECT_TRUE(includesExposedPositions);
  
  // Now with line of sight avoidance
  auto optionsWithAvoidance = testGetSafeMoveOptions(
    mockAlgorithm, board, myTank, enemyTank, shells, true);
  
  // Should not include positions in enemy's line of sight
  includesExposedPositions = false;
  for (const auto& option : optionsWithAvoidance) {
      bool isExposed = testIsExposedToEnemy(mockAlgorithm, board, option.position, enemyTank);
      if (isExposed) {
          includesExposedPositions = true;
          break;
      }
  }
  EXPECT_FALSE(includesExposedPositions);
}

// New test for IsExposedToEnemy
TEST_F(AlgorithmTest, IsExposedToEnemy_DetectsLineOfSight) {
  GameBoard board = create5X5TestBoard({
      "#####",
      "#   #",
      "#   #",
      "#   #",
      "#####"
  });
  
  Tank enemyTank(2, Point(1, 2), Direction::Right);
  
  // Position directly in line of sight
  EXPECT_TRUE(testIsExposedToEnemy(mockAlgorithm, board, Point(3, 2), enemyTank));
  
  // Position behind a wall
  board.setCellType(Point(2, 2), GameBoard::CellType::Wall);
  EXPECT_FALSE(testIsExposedToEnemy(mockAlgorithm, board, Point(3, 2), enemyTank));
  
  // Position not in line with enemy
  EXPECT_FALSE(testIsExposedToEnemy(mockAlgorithm, board, Point(2, 4), enemyTank));
  
  // Enemy destroyed
  Tank destroyedTank(2, Point(1, 2), Direction::Right);
  destroyedTank.destroy();
  EXPECT_FALSE(testIsExposedToEnemy(mockAlgorithm, board, Point(3, 2), destroyedTank));
}

TEST_F(AlgorithmTest, CalculateMoveCost_AlreadyFacingTarget) {
    Tank tank(1, Point(2, 2), Direction::Right);
    
    // Already facing target direction
    int cost = testCalculateMoveCost(mockAlgorithm, tank, Point(3, 2), Direction::Right);
    EXPECT_EQ(cost, 1);
}

TEST_F(AlgorithmTest, CalculateMoveCost_SingleRotation) {
    Tank tank(1, Point(2, 2), Direction::Right);
    
    // 1/8 turn (45 degrees)
    int cost1 = testCalculateMoveCost(mockAlgorithm, tank, Point(3, 3), Direction::DownRight);
    EXPECT_EQ(cost1, 2);  // 1 rotate + 1 move
    
    // 1/4 turn (90 degrees)
    int cost2 = testCalculateMoveCost(mockAlgorithm, tank, Point(2, 3), Direction::Down);
    EXPECT_EQ(cost2, 2);  // 1 rotate + 1 move
}

TEST_F(AlgorithmTest, CalculateMoveCost_MultipleRotations) {
    Tank tank(1, Point(2, 2), Direction::Right);
    
    // 3/8 turn (135 degrees)
    int cost1 = testCalculateMoveCost(mockAlgorithm, tank, Point(1, 3), Direction::DownLeft);
    EXPECT_EQ(cost1, 3);  // 2 rotates + 1 move
    
    // 1/2 turn (180 degrees)
    int cost2 = testCalculateMoveCost(mockAlgorithm, tank, Point(1, 2), Direction::Left);
    EXPECT_EQ(cost2, 3);  // 2 rotates + 1 move
}

// Targeting Tests
TEST_F(AlgorithmTest, CanHitTarget_InLineOfSight) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    
    Tank tank(1, Point(1, 2), Direction::Right);
    
    // Target in line of sight
    EXPECT_TRUE(testCanHitTarget(mockAlgorithm, board, tank, Point(3, 2)));
    
    // Target not in line with direction
    EXPECT_FALSE(testCanHitTarget(mockAlgorithm, board, tank, Point(1, 3)));
    
    // Target blocked by wall
    board.setCellType(Point(2, 2), GameBoard::CellType::Wall);
    EXPECT_FALSE(testCanHitTarget(mockAlgorithm, board, tank, Point(3, 2)));
}

TEST_F(AlgorithmTest, CanHitTarget_CooldownAndShells) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    
    Tank tank(1, Point(1, 2), Direction::Right);
    
    // With cooldown
    tank.shoot();  // Activates cooldown
    EXPECT_FALSE(testCanHitTarget(mockAlgorithm, board, tank, Point(3, 2)));
    
    // With no shells
    Tank emptyTank(1, Point(1, 2), Direction::Right);
    for (int i = 0; i < Tank::INITIAL_SHELLS; i++) {
        emptyTank.decrementShells();
    }
    EXPECT_FALSE(testCanHitTarget(mockAlgorithm, board, emptyTank, Point(3, 2)));
}

TEST_F(AlgorithmTest, GetRotationToDirection_SingleRotation) {
    // 1/8 turns
    EXPECT_EQ(testGetRotationToDirection(mockAlgorithm, Direction::Up, Direction::UpRight), 
              Action::RotateRightEighth);
    EXPECT_EQ(testGetRotationToDirection(mockAlgorithm, Direction::Up, Direction::UpLeft), 
              Action::RotateLeftEighth);
    
    // 1/4 turns
    EXPECT_EQ(testGetRotationToDirection(mockAlgorithm, Direction::Up, Direction::Right), 
              Action::RotateRightQuarter);
    EXPECT_EQ(testGetRotationToDirection(mockAlgorithm, Direction::Up, Direction::Left), 
              Action::RotateLeftQuarter);
}

TEST_F(AlgorithmTest, GetRotationToDirection_MultipleRotations) {
    // Multiple steps - should return first rotation in optimal direction
    
    // 3/8 turn (need 3 eighth turns) - should use quarter then eighth
    EXPECT_EQ(testGetRotationToDirection(mockAlgorithm, Direction::Up, Direction::DownRight), 
              Action::RotateRightQuarter);
    
    // 1/2 turn (need 4 eighth turns) - should use quarter
    EXPECT_EQ(testGetRotationToDirection(mockAlgorithm, Direction::Up, Direction::Down), 
              Action::RotateRightQuarter);
    
    // 5/8 turn (need 5 eighth turns) - shorter to go other way (3 eighth turns)
    EXPECT_EQ(testGetRotationToDirection(mockAlgorithm, Direction::Up, Direction::DownLeft), 
              Action::RotateLeftQuarter);
}

TEST_F(AlgorithmTest, EvaluateOffensiveOptions_CanShoot) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    
    Tank myTank(1, Point(1, 2), Direction::Right);
    Tank enemyTank(2, Point(3, 2), Direction::Left);
    
    Action action = testEvaluateOffensiveOptions(mockAlgorithm, board, myTank, enemyTank);
    EXPECT_EQ(action, Action::Shoot);
}

TEST_F(AlgorithmTest, EvaluateOffensiveOptions_NeedRotation) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    
    Tank myTank(1, Point(1, 2), Direction::Down);
    Tank enemyTank(2, Point(3, 2), Direction::Left);
    
    Action action = testEvaluateOffensiveOptions(mockAlgorithm, board, myTank, enemyTank);
    EXPECT_EQ(action, Action::RotateLeftQuarter);
}

TEST_F(AlgorithmTest, EvaluateOffensiveOptions_NoOptions) {
    GameBoard board = create5X5TestBoard({
        "#####",
        "#   #",
        "# # #", // Wall at (2, 2)
        "#   #",
        "#####"
    });
    
    Tank myTank(1, Point(1, 2), Direction::Right);
    Tank enemyTank(2, Point(3, 2), Direction::Left);
    
    Action action = testEvaluateOffensiveOptions(mockAlgorithm, board, myTank, enemyTank);
    EXPECT_EQ(action, Action::None);
}