#include "gtest/gtest.h"
#include "algo/defensive_algorithm.h"
#include "game_board.h"
#include "tank.h"
#include "shell.h"
#include <vector>

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
        board.initialize(boardLines, errors);
        return board;
    }
    
    DefensiveAlgorithm* algorithm;
};

TEST_F(DefensiveAlgorithmTest, Constructor) {
    EXPECT_NE(algorithm, nullptr);
}

TEST_F(DefensiveAlgorithmTest, GetNextAction_InDangerNoSafeAction) {
    std::vector<std::string> boardLines = {
        "#####",
        "# 1 #",
        "#####",
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(2, 1), Direction::Right);
    Tank enemyTank(2, Point(3, 3), Direction::Left);
    
    std::vector<Shell> shells;
    shells.push_back(Shell(2, Point(1, 1), Direction::Right)); // Shell about to hit tank
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    EXPECT_EQ(action, Action::None);
}

TEST_F(DefensiveAlgorithmTest, GetNextAction_SafeCanShoot) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "#   #",
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 1), Direction::Right);
    Tank enemyTank(2, Point(3, 1), Direction::Left);
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    EXPECT_EQ(action, Action::Shoot);
}

TEST_F(DefensiveAlgorithmTest, GetNextAction_SafeCannotShoot) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "#   #",
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 1), Direction::Down);
    Tank enemyTank(2, Point(3, 1), Direction::Left);
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    EXPECT_EQ(action, Action::RotateLeftQuarter);
}

TEST_F(DefensiveAlgorithmTest, GetNextAction_InDangerCanEscape) {
    std::vector<std::string> boardLines = {
        "#####",
        "#   #",
        "  1 #",
        "#   #",
        "##2##"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(2, 2), Direction::Up);
    Tank enemyTank(2, Point(4, 4), Direction::Left);
    
    std::vector<Shell> shells;
    shells.push_back(Shell(2, Point(0, 2), Direction::Right));
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    EXPECT_EQ(action, Action::MoveForward);
}

TEST_F(DefensiveAlgorithmTest, GetNextAction_DangerPrioritizedOverShooting) {
    std::vector<std::string> boardLines = {
        "#####",
        "#   #",
        "#1 2#",
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 2), Direction::Right);
    Tank enemyTank(2, Point(3, 2), Direction::Left);
    
    std::vector<Shell> shells;
    shells.push_back(Shell(2, Point(0, 2), Direction::Right));
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    
    EXPECT_NE(action, Action::Shoot);
}

TEST_F(DefensiveAlgorithmTest, GetNextAction_NoShellsRemaining) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "#   #",
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 1), Direction::Right);
    // Use all shells
    for (int i = 0; i < Tank::INITIAL_SHELLS; i++) {
        myTank.decrementShells();
    }
    
    Tank enemyTank(2, Point(3, 1), Direction::Left);
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    EXPECT_EQ(action, Action::RotateRightQuarter);
}

TEST_F(DefensiveAlgorithmTest, GetNextAction_ShootingCooldown) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "#   #",
        "#   #",
        "#####"
    };
    GameBoard board = createTestBoard(boardLines);
    
    Tank myTank(1, Point(1, 1), Direction::Right);
    myTank.shoot();
    
    Tank enemyTank(2, Point(3, 1), Direction::Left);
    std::vector<Shell> shells;
    
    Action action = algorithm->getNextAction(board, myTank, enemyTank, shells);
    EXPECT_EQ(action, Action::RotateRightQuarter);
}