#include "gtest/gtest.h"
#include "battle_info_impl.h"
#include "UserCommon/game_board.h"
#include "utils/point.h"

using namespace UserCommon_098765432_123456789;

namespace Algorithm_098765432_123456789 {
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

TEST_F(BattleInfoImplTest, Clear_ResetsState) {
    info->setCellType(1, 1, GameBoard::CellType::Wall);
    info->addEnemyTankPosition(Point(1, 1));
    info->addFriendlyTankPosition(Point(2, 2));
    info->addShellPosition(Point(0, 0));
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
}

TEST_F(BattleInfoImplTest, SetAndGetOwnTankPosition) {
    Point pos(2, 1);
    info->setOwnTankPosition(pos);
    EXPECT_EQ(info->getOwnTankPosition(), pos);
    // Overwrite
    Point pos2(3, 2);
    info->setOwnTankPosition(pos2);
    EXPECT_EQ(info->getOwnTankPosition(), pos2);
}

TEST_F(BattleInfoImplTest, OwnTankPositionPersistsAfterClear) {
    Point pos(1, 2);
    info->setOwnTankPosition(pos);
    info->clear();
    // Own tank position should remain unchanged after clear (unless design says otherwise)
    EXPECT_EQ(info->getOwnTankPosition(), pos);
}

} // namespace Algorithm_098765432_123456789 