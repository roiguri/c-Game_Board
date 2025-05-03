#include "gtest/gtest.h"
#include "algo/chase_algorithm.h"
#include "game_board.h"
#include "objects/tank.h"
#include "objects/shell.h"

class ChaseAlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        algorithm = new ChaseAlgorithm();
    }
    
    void TearDown() override {
        delete algorithm;
    }
    
    // Helper method to create a test board
    GameBoard createTestBoard(const std::vector<std::string>& boardLines) const {
        GameBoard board(boardLines[0].length(), boardLines.size());
        std::vector<std::string> errors;
        std::vector<std::pair<int, Point>> tankPositions;
        board.initialize(boardLines, errors, tankPositions);
        return board;
    }
    
    ChaseAlgorithm* algorithm;

    std::vector<Point> getCurrentPath() const {
        return algorithm->m_currentPath;
    }
};

// Test Priority 1: Avoid shells
TEST_F(ChaseAlgorithmTest, Priority1_AvoidShells_ImmediateDanger) {
    std::vector<std::string> boardLines = {
        "#####",
        "#   #",
        "# 1 #",
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(2, 2), Direction::Right);
    Tank enemyTank(2, Point(4, 4), Direction::Left);
    
    // Shell coming directly at the tank
    std::vector<Shell> shells;
    shells.push_back(Shell(2, Point(4, 2), Direction::Left));
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // Should move to avoid the shell
    EXPECT_NE(action, Action::None);
    EXPECT_NE(action, Action::MoveForward); // Forward would move into shell's path
}

// Test Priority 2: Shoot if in direction + have line of sight
TEST_F(ChaseAlgorithmTest, Priority2_Shoot_DirectLineOfSight) {
    std::vector<std::string> boardLines = {
        "#####",
        "#   #",
        "#1 2#", // Tanks facing each other
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 2), Direction::Right);
    Tank enemyTank(2, Point(3, 2), Direction::Left);
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    EXPECT_EQ(action, Action::Shoot);
}

// Test Priority 3: Rotate to face enemy if line of sight exists
TEST_F(ChaseAlgorithmTest, Priority3_Rotate_LineOfSight) {
    std::vector<std::string> boardLines = {
        "#####",
        "#   #",
        "#1 2#", // Tanks in same row but not facing each other
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 2), Direction::Down); // Facing down, not towards enemy
    Tank enemyTank(2, Point(3, 2), Direction::Left);
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // Should rotate right to face the enemy
    EXPECT_EQ(action, Action::RotateLeftQuarter);
}

// Test Priority 4: Chase enemy
TEST_F(ChaseAlgorithmTest, Priority4_Chase_DirectPath) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1  #",
        "#   #",
        "#  2#",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 1), Direction::Right);
    Tank enemyTank(2, Point(3, 3), Direction::Left);
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    EXPECT_EQ(action, Action::RotateRightEighth);
}

TEST_F(ChaseAlgorithmTest, Priority4_Chase_PathWithObstacles) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1  #",
        "### #",  // Wall forcing a different path
        "#  2#",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 1), Direction::Right);
    Tank enemyTank(2, Point(3, 3), Direction::Left);
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // Should find path around obstacle
    EXPECT_TRUE(action == Action::MoveForward);
}

TEST_F(ChaseAlgorithmTest, Priority4_Chase_NoPathAvailable) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1###",  // Tanks separated by impassable walls
        "### #",
        "#  2#",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 1), Direction::Right);
    Tank enemyTank(2, Point(3, 3), Direction::Left);
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // No path available, should rotate to scan
    EXPECT_TRUE(action == Action::RotateRightEighth || 
                action == Action::RotateLeftEighth);
}

// Combined priorities tests
TEST_F(ChaseAlgorithmTest, CombinedPriorities_DangerOverShooting) {
    std::vector<std::string> boardLines = {
        "#####",
        "#   #",
        "#1 2#",  // Tanks facing each other
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 2), Direction::Right);
    Tank enemyTank(2, Point(3, 2), Direction::Left);
    
    // Shell coming at tank 1
    std::vector<Shell> shells;
    shells.push_back(Shell(2, Point(0, 2), Direction::Right));
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // Should avoid shell even though it could shoot
    EXPECT_NE(action, Action::Shoot);
}

TEST_F(ChaseAlgorithmTest, CombinedPriorities_ShootingOverChasing) {
    std::vector<std::string> boardLines = {
        "#####",
        "#   #",
        "#1 2#",  // Tanks facing each other
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 2), Direction::Right);
    Tank enemyTank(2, Point(3, 2), Direction::Left);
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // Should shoot rather than chase since enemy is in line of sight
    EXPECT_EQ(action, Action::Shoot);
}

// Edge cases
TEST_F(ChaseAlgorithmTest, EdgeCase_NoShellsRemaining) {
    std::vector<std::string> boardLines = {
        "#####",
        "#   #",
        "#1 2#",
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 2), Direction::Right);
    
    // Use all shells
    for (int i = 0; i < Tank::INITIAL_SHELLS; i++) {
        myTank.decrementShells();
    }
    
    Tank enemyTank(2, Point(3, 2), Direction::Left);
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // Can't shoot, but should still prioritize chasing over rotating
    EXPECT_NE(action, Action::Shoot);
    EXPECT_EQ(action, Action::RotateLeftEighth);
}

TEST_F(ChaseAlgorithmTest, RecalculatesPathWhenOffTrack) {
  std::vector<std::string> boardLines = {
      " ######",
      "#     #",
      "#1    #",
      "#     #",
      "#    2#",
      "#     #",
      "#######"
  };
  GameBoard board = createTestBoard(boardLines);
  
  // Set up initial positions
  Tank myTank(1, Point(1, 2), Direction::Right);
  Tank enemyTank(2, Point(5, 4), Direction::Left);
  std::vector<Shell> shells;
  
  // First generate a path
  algorithm->getNextAction(board, myTank, enemyTank, shells);
  
  // Get the initial path
  std::vector<Point> originalPath = getCurrentPath();
  
  // Move tank to a position off the expected path
  Point newPosition(0, 0);
  bool positionInPath = false;
  for (const auto& point : originalPath) {
      if (point == newPosition) {
          positionInPath = true;
          break;
      }
  }
  ASSERT_FALSE(positionInPath) << "Test setup error: new position should not be on the original path";
  
  myTank = Tank(1, newPosition, Direction::Right);
  
  // Call again - should trigger recalculation
  algorithm->getNextAction(board, myTank, enemyTank, shells);
  
  // Get the new path
  std::vector<Point> newPath = getCurrentPath();
  
  // Verify path was recalculated
  bool pathsAreDifferent = originalPath.size() != newPath.size() ||
                          !std::equal(originalPath.begin(), originalPath.end(), newPath.begin());
  EXPECT_TRUE(pathsAreDifferent) << "Path should be recalculated when tank moves off track";
  
  // The first point in the new path should be adjacent to the tank's current position
  bool firstPointIsAdjacent = false;
  if (!newPath.empty()) {
      double distance = Point::euclideanDistance(newPath.front(), newPosition);
      firstPointIsAdjacent = (distance <= 1.5);
  }
  EXPECT_TRUE(firstPointIsAdjacent) << "First point in new path should be adjacent to the tank's position";
}

TEST_F(ChaseAlgorithmTest, RecalculatesPathWhenTargetMoves) {
  std::vector<std::string> boardLines = {
      "#######",
      "#     #",
      "#1    #",
      "#     #",
      "#    2#",
      "#     #",
      "#######"
  };
  GameBoard board = createTestBoard(boardLines);
  
  // Set up initial positions
  Tank myTank(1, Point(1, 2), Direction::Right);
  Tank enemyTank(2, Point(5, 4), Direction::Left);
  std::vector<Shell> shells;
  
  // First generate a path
  algorithm->getNextAction(board, myTank, enemyTank, shells);
  
  // Get the initial path
  std::vector<Point> originalPath = getCurrentPath();
  ASSERT_FALSE(originalPath.empty()) << "Initial path should not be empty";
  
  // Store the last target position
  Point originalTarget = enemyTank.getPosition();
  
  // Move target significantly (more than 1.5 Euclidean distance)
  Point newEnemyPosition(2, 5);
  double distance = Point::euclideanDistance(originalTarget, newEnemyPosition);
  ASSERT_GT(distance, 1.5) << "Target should have moved more than 1.5 distance";
  
  enemyTank = Tank(2, newEnemyPosition, Direction::Left);
  
  // Call again - should trigger recalculation
  algorithm->getNextAction(board, myTank, enemyTank, shells);
  
  // Get the new path
  std::vector<Point> newPath = getCurrentPath();
  
  // Verify path was recalculated
  bool pathsAreDifferent = originalPath.size() != newPath.size() ||
                          !std::equal(originalPath.begin(), originalPath.end(), newPath.begin());
  EXPECT_TRUE(pathsAreDifferent) << "Path should be recalculated when target moves significantly";
  
  // Verify the last point in the new path is close to the new enemy position
  // (Might not be exactly at enemy position due to obstacles)
  if (!newPath.empty()) {
      Point lastPoint = newPath.back();
      bool pathLeadsToTarget = Point::euclideanDistance(lastPoint, newEnemyPosition) <= 2.0;
      EXPECT_TRUE(pathLeadsToTarget) << "New path should lead toward the new target position";
  }
}
