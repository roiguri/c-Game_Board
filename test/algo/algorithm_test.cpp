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
};

class AlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockAlgorithm = new MockAlgorithm();
    }
    
    void TearDown() override {
        delete mockAlgorithm;
    }
    
    Algorithm* mockAlgorithm;
};

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