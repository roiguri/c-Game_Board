#include <gtest/gtest.h>
#include "board_info_test_helpers.h"

class BoardInfoTestHelpersTest : public ::testing::Test {
};

TEST_F(BoardInfoTestHelpersTest, CreateValidBoardInfo) {
    auto boardInfo = BoardInfoTestHelpers::createValidBoardInfo();
    
    EXPECT_EQ(boardInfo.rows, 10);
    EXPECT_EQ(boardInfo.cols, 10);
    EXPECT_EQ(boardInfo.maxSteps, 100);
    EXPECT_EQ(boardInfo.numShells, 5);
    EXPECT_NE(boardInfo.satelliteView, nullptr);
    
    // Test satellite view has correct dimensions
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(0, 0), '#');
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(1, 1), '1');
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(8, 1), '2');
}

TEST_F(BoardInfoTestHelpersTest, CreateInvalidBoardInfo) {
    auto boardInfo = BoardInfoTestHelpers::createInvalidBoardInfo();
    
    EXPECT_EQ(boardInfo.rows, 10);
    EXPECT_EQ(boardInfo.cols, 10);
    EXPECT_EQ(boardInfo.maxSteps, 100);
    EXPECT_EQ(boardInfo.numShells, 5);
    EXPECT_EQ(boardInfo.satelliteView, nullptr);
}

TEST_F(BoardInfoTestHelpersTest, CreateCustomBoardInfo) {
    auto boardInfo = BoardInfoTestHelpers::createCustomBoardInfo(5, 8, 200, 15);
    
    EXPECT_EQ(boardInfo.rows, 5);
    EXPECT_EQ(boardInfo.cols, 8);
    EXPECT_EQ(boardInfo.maxSteps, 200);
    EXPECT_EQ(boardInfo.numShells, 15);
    EXPECT_NE(boardInfo.satelliteView, nullptr);
    
    // Test board structure
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(0, 0), '#'); // Top-left corner
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(7, 0), '#'); // Top-right corner
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(0, 4), '#'); // Bottom-left corner
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(7, 4), '#'); // Bottom-right corner
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(1, 1), '1'); // Player 1
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(6, 1), '2'); // Player 2
}