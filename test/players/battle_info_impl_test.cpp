#include "gtest/gtest.h"
#include "players/battle_info_impl.h"
#include "game_board.h"
#include "utils/point.h"
#include <string>
#include <vector>

class BattleInfoImplTest : public ::testing::Test {
protected:
    void SetUp() override {
        boardWidth = 4;
        boardHeight = 3;
        info = std::make_unique<BattleInfoImpl>(boardWidth, boardHeight);
    }
    int boardWidth;
    int boardHeight;
    std::unique_ptr<BattleInfoImpl> info;
};

TEST_F(BattleInfoImplTest, Constructor_InitializesBoard) {
    EXPECT_EQ(info->getGameBoard().getWidth(), boardWidth);
    EXPECT_EQ(info->getGameBoard().getHeight(), boardHeight);
}

TEST_F(BattleInfoImplTest, SetAndGetCellType) {
    info->setCellType(1, 2, GameBoard::CellType::Wall);
    EXPECT_EQ(info->getCellType(1, 2), GameBoard::CellType::Wall);
    info->setCellType(1, 2, GameBoard::CellType::Mine);
    EXPECT_EQ(info->getCellType(1, 2), GameBoard::CellType::Mine);
}

TEST_F(BattleInfoImplTest, AddAndGetEnemyTankPositions) {
    Point p1(1, 1), p2(2, 2);
    info->addEnemyTankPosition(p1);
    info->addEnemyTankPosition(p2);
    const auto& positions = info->getEnemyTankPositions();
    ASSERT_EQ(positions.size(), 2);
    EXPECT_EQ(positions[0], p1);
    EXPECT_EQ(positions[1], p2);
}

TEST_F(BattleInfoImplTest, AddAndGetFriendlyTankPositions) {
    Point p1(0, 0), p2(3, 2);
    info->addFriendlyTankPosition(p1);
    info->addFriendlyTankPosition(p2);
    const auto& positions = info->getFriendlyTankPositions();
    ASSERT_EQ(positions.size(), 2);
    EXPECT_EQ(positions[0], p1);
    EXPECT_EQ(positions[1], p2);
}

TEST_F(BattleInfoImplTest, AddAndGetShellPositions) {
    Point p1(2, 1);
    info->addShellPosition(p1);
    const auto& shells = info->getShellPositions();
    ASSERT_EQ(shells.size(), 1);
    EXPECT_EQ(shells[0], p1);
}

TEST_F(BattleInfoImplTest, SetAndGetFeedback) {
    std::string feedback = "Test feedback message.";
    info->setFeedback(feedback);
    EXPECT_EQ(info->getFeedback(), feedback);
}

TEST_F(BattleInfoImplTest, Clear_ResetsState) {
    info->setCellType(1, 1, GameBoard::CellType::Wall);
    info->addEnemyTankPosition(Point(1, 1));
    info->addFriendlyTankPosition(Point(2, 2));
    info->addShellPosition(Point(0, 0));
    info->setFeedback("msg");
    info->clear();
    // Board should be empty
    for (int y = 0; y < boardHeight; ++y) {
        for (int x = 0; x < boardWidth; ++x) {
            EXPECT_EQ(info->getCellType(x, y), GameBoard::CellType::Empty);
        }
    }
    EXPECT_TRUE(info->getEnemyTankPositions().empty());
    EXPECT_TRUE(info->getFriendlyTankPositions().empty());
    EXPECT_TRUE(info->getShellPositions().empty());
    EXPECT_TRUE(info->getFeedback().empty());
} 