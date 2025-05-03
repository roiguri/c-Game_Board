#include "gtest/gtest.h"
#include "algo/defensive_algorithm.h"
#include "game_board.h"
#include "objects/tank.h"
#include "objects/shell.h"

class DefensiveAlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        algorithm = new DefensiveAlgorithm();
    }
    
    void TearDown() override {
        delete algorithm;
    }
    
    // Helper method to create a test board
    GameBoard createTestBoard(const std::vector<std::string>& boardLines) const {
        GameBoard board(boardLines[0].length(), boardLines.size());
        std::vector<std::string> errors;
        std::map<int, std::vector<Point>> tankPositions;
        board.initialize(boardLines, errors, tankPositions);
        return board;
    }
    
    DefensiveAlgorithm* algorithm;
};

// Test Priority 1: Avoid shells
TEST_F(DefensiveAlgorithmTest, Priority1_AvoidShells_Immediate) {
    std::vector<std::string> boardLines = {
        "#####",
        "#   #",
        "# 1 #",
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(2, 2), Direction::Right);
    Tank enemyTank(2, Point(4, 4), Direction::Left);  // Enemy not nearby
    
    // Shell coming directly at the tank
    std::vector<Shell> shells;
    shells.push_back(Shell(2, Point(4, 2), Direction::Left));
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // Should move to avoid the shell (not forward into the shell's path)
    EXPECT_NE(action, Action::None);
    EXPECT_NE(action, Action::MoveForward);
}

TEST_F(DefensiveAlgorithmTest, Priority1_AvoidShells_MultipleSafeOptions) {
    std::vector<std::string> boardLines = {
        "#####",
        "#   #",
        "# 1 #",
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(2, 2), Direction::Right);
    Tank enemyTank(2, Point(4, 4), Direction::Left);  // Enemy not nearby
    
    // Shell coming at tank from above
    std::vector<Shell> shells;
    shells.push_back(Shell(2, Point(2, 0), Direction::Down));
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // Should choose an optimal move away from the shell's path
    // The cheapest move would be moving forward as already facing right
    EXPECT_EQ(action, Action::MoveForward);
}

TEST_F(DefensiveAlgorithmTest, Priority1_AvoidShells_NoSafeMove) {
    std::vector<std::string> boardLines = {
        "#####",
        "#SSS#",
        "# 1 #",  // Shells coming from three directions
        "#S S#",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(2, 2), Direction::Right);
    Tank enemyTank(2, Point(4, 4), Direction::Left);
    
    std::vector<Shell> shells;
    shells.push_back(Shell(2, Point(1, 1), Direction::Right));
    shells.push_back(Shell(2, Point(2, 1), Direction::Down));
    shells.push_back(Shell(2, Point(3, 1), Direction::Down));
    shells.push_back(Shell(2, Point(3, 3), Direction::Left));
    shells.push_back(Shell(2, Point(1, 3), Direction::Up));
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // With no safe moves, should rotate
    EXPECT_EQ(action, Action::RotateRightEighth);
    EXPECT_TRUE(action == Action::None || 
                action == Action::RotateRightEighth || 
                action == Action::RotateLeftEighth);
}

// Test Priority 2: Shoot if in direction and have line of sight
TEST_F(DefensiveAlgorithmTest, Priority2_Shoot_DirectLineOfSight) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1  #",
        "#   #",
        "#  2#",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    // Tank 1 facing diagonally toward tank 2
    Tank myTank(1, Point(1, 1), Direction::DownRight);
    Tank enemyTank(2, Point(3, 3), Direction::Left);
    std::vector<Shell> shells;  // No shells
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    EXPECT_EQ(action, Action::Shoot);
}

TEST_F(DefensiveAlgorithmTest, Priority2_Shoot_NeedsRotation) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1  #",
        "#   #",
        "#  2#",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    // Tank 1 not facing tank 2
    Tank myTank(1, Point(1, 1), Direction::Left);
    Tank enemyTank(2, Point(3, 3), Direction::Left);
    std::vector<Shell> shells;  // No shells
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // Should not shoot as not facing the enemy
    EXPECT_NE(action, Action::Shoot);
}

TEST_F(DefensiveAlgorithmTest, Priority2_CannotShoot_Cooldown) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1  #",
        "#   #",
        "#  2#",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    // Tank 1 facing tank 2 but on cooldown
    Tank myTank(1, Point(1, 1), Direction::DownRight);
    myTank.shoot();  // Trigger cooldown
    
    Tank enemyTank(2, Point(3, 3), Direction::Left);
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    EXPECT_NE(action, Action::Shoot);
}

// Test Priority 3: Run away if enemy has line of sight
TEST_F(DefensiveAlgorithmTest, Priority3_RunAway_EnemyHasLineOfSight) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1  #",
        "#   #",
        "#  2#",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    // Tank 1 not facing enemy, but enemy is facing tank 1
    Tank myTank(1, Point(1, 1), Direction::Up);
    Tank enemyTank(2, Point(3, 3), Direction::UpLeft);  // Facing toward tank 1
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // Should try to move away since enemy can see us
    EXPECT_TRUE(action == Action::MoveForward || 
                action == Action::RotateLeftQuarter ||
                action == Action::RotateRightQuarter);
}

TEST_F(DefensiveAlgorithmTest, Priority3_RunAway_EnemyNoLineOfSight) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1  #",
        "# # #",  // Wall blocking line of sight
        "#  2#",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    // Wall blocks line of sight between tanks
    Tank myTank(1, Point(1, 1), Direction::Up);
    Tank enemyTank(2, Point(3, 3), Direction::UpLeft);
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // Not in danger, shouldn't need to run, might rotate to scan
    EXPECT_TRUE(action == Action::RotateRightEighth || 
                action == Action::RotateLeftEighth);
}

TEST_F(DefensiveAlgorithmTest, Priority3_RunAway_NowhereToHide) {
    std::vector<std::string> boardLines = {
        "#####",
        "## ##",
        "#1 2#",  // Tanks facing each other with limited movement options
        "## ##",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 2), Direction::Down);
    Tank enemyTank(2, Point(3, 2), Direction::Left);  // Facing each other
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    // Nowhere to hide, should shoot since it can
    EXPECT_EQ(action, Action::RotateLeftEighth);
}

// Combined priority tests
TEST_F(DefensiveAlgorithmTest, CombinedPriorities_DangerOverShooting) {
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

TEST_F(DefensiveAlgorithmTest, CombinedPriorities_ShootingOverRunning) {
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
    
    // Should shoot even though exposed to enemy
    EXPECT_EQ(action, Action::Shoot);
}

// Edge cases
TEST_F(DefensiveAlgorithmTest, EdgeCase_NoShellsRemaining) {
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
    
    // Can't shoot, should try to move away from enemy sight
    EXPECT_NE(action, Action::Shoot);
}