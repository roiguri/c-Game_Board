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

    bool testHasDirectLineOfSight(
      const GameBoard& gameBoard,
      const Point& from,
      const Point& to
    ) const {
        return hasDirectLineOfSight(gameBoard, from, to);
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
    
    EXPECT_TRUE(mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 1), Point(3, 1)));
    EXPECT_TRUE(mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 1), Point(1, 3)));
    EXPECT_FALSE(mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 1), Point(3, 2)));
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
    
    EXPECT_FALSE(mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 1), Point(3, 1)));
    EXPECT_FALSE(mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 3), Point(3, 3)));
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
    
    EXPECT_FALSE(mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 1), Point(1, 3)));
    EXPECT_FALSE(mockAlgorithm->testHasDirectLineOfSight(board, Point(3, 1), Point(3, 3)));
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
    
    EXPECT_FALSE(mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 1), Point(3, 3)));
    EXPECT_FALSE(mockAlgorithm->testHasDirectLineOfSight(board, Point(3, 1), Point(1, 3)));
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
    
    EXPECT_TRUE(mockAlgorithm->testHasDirectLineOfSight(board, Point(1, 2), Point(3, 2)));
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
    
    EXPECT_TRUE(mockAlgorithm->testHasDirectLineOfSight(board, Point(2, 1), Point(2, 3)));
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
    
    EXPECT_TRUE(mockAlgorithm->testHasDirectLineOfSight(board, Point(3, 1), Point(1, 3)));
}