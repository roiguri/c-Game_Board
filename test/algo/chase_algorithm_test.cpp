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