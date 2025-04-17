#include "gtest/gtest.h"
#include "algo/chase_algorithm.h"
#include "game_board.h"
#include "tank.h"
#include "shell.h"

class ChaseAlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        algorithm = new ChaseAlgorithm();
    }
    
    void TearDown() override {
        delete algorithm;
    }
    
    ChaseAlgorithm* algorithm;
    
    GameBoard create5X5TestBoard(const std::vector<std::string>& boardLines) {
        GameBoard board(5, 5);
        std::vector<std::string> errors;
        board.initialize(boardLines, errors);
        return board;
    }

    bool testCanShootEnemy(
        const GameBoard& board,
        const Tank& myTank,
        const Tank& enemyTank
    ) {
        return algorithm->canShootEnemy(board, myTank, enemyTank);
    }
};

TEST_F(ChaseAlgorithmTest, Constructor) {
    EXPECT_NE(algorithm, nullptr);
}

TEST_F(ChaseAlgorithmTest, CanShootEnemy_NoLineOfSight) {
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
    
    EXPECT_FALSE(testCanShootEnemy(board, myTank, enemyTank));
}

TEST_F(ChaseAlgorithmTest, CanShootEnemy_LineOfSightWrongDirection) {
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
    
    EXPECT_FALSE(testCanShootEnemy(board, myTank, enemyTank));
}

TEST_F(ChaseAlgorithmTest, CanShootEnemy_LineOfSightCorrectDirection) {
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
    
    EXPECT_TRUE(testCanShootEnemy(board, myTank, enemyTank));
}

TEST_F(ChaseAlgorithmTest, CanShootEnemy_TankDestroyed) {
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
    
    EXPECT_FALSE(testCanShootEnemy(board, myTank, enemyTank));
    
    myTank = Tank(1, Point(1, 1), Direction::Right);
    enemyTank.destroy();
    
    EXPECT_FALSE(testCanShootEnemy(board, myTank, enemyTank));
}

TEST_F(ChaseAlgorithmTest, CanShootEnemy_NoShells) {
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
    
    EXPECT_FALSE(testCanShootEnemy(board, myTank, enemyTank));
}

TEST_F(ChaseAlgorithmTest, CanShootEnemy_ShootCooldown) {
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
    
    EXPECT_FALSE(testCanShootEnemy(board, myTank, enemyTank));
}

TEST_F(ChaseAlgorithmTest, CanShootEnemy_WrappingLineOfSight) {
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
    
    EXPECT_TRUE(testCanShootEnemy(board, myTank, enemyTank));
}

TEST_F(ChaseAlgorithmTest, GetNextAction_NoAction) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "# # #",  // Wall in the middle
        "#   #",
        "#####"
    };
    GameBoard board = create5X5TestBoard(boardLines);
    
    // Create tanks with wall between them
    Tank myTank(1, Point(1, 1), Direction::Right);
    Tank enemyTank(2, Point(3, 1), Direction::Left);
    std::vector<Shell> shells;
    
    // Should return None since there's no clear shot and we don't have path finding yet
    EXPECT_EQ(algorithm->getNextAction(board, myTank, enemyTank, shells), Action::None);
}

TEST_F(ChaseAlgorithmTest, GetNextAction_ReturnNoneNotImlemented) {
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
    std::vector<Shell> shells;
  
    EXPECT_EQ(algorithm->getNextAction(board, myTank, enemyTank, shells), Action::None);
}