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
};

TEST_F(ChaseAlgorithmTest, Constructor) {
    EXPECT_NE(algorithm, nullptr);
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