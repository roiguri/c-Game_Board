#include "gtest/gtest.h"
#include "offensive_player.h"
#include "offensive_battle_info.h"
#include "TankAlgorithm.h"
#include "SatelliteView.h"
#include "utils/point.h"

using namespace UserCommon_098765432_123456789;

namespace Algorithm_318835816_211314471 {
#include <vector>
#include <memory>

// Mock SatelliteView for a simple board
class MockSatelliteView : public SatelliteView {
public:
    MockSatelliteView(const std::vector<std::vector<char>>& board) : m_board(board) {}
    char getObjectAt(size_t x, size_t y) const override {
        if (y < m_board.size() && x < m_board[y].size())
            return m_board[y][x];
        return ' ';
    }
private:
    std::vector<std::vector<char>> m_board;
};

// Mock TankAlgorithm
class MockTankAlgorithm : public TankAlgorithm {
public:
    MockTankAlgorithm() : updateCalled(false), lastInfo(nullptr) {}
    void updateBattleInfo(BattleInfo& info) override {
        updateCalled = true;
        lastInfo = &info;
    }
    ActionRequest getAction() override { return ActionRequest::GetBattleInfo; }
    bool updateCalled;
    BattleInfo* lastInfo;
};

TEST(OffensivePlayerTest, Construction) {
    Player_318835816_211314471_A player(1, 3, 3, 10, 5);
    // Just ensure construction does not throw
    SUCCEED();
}

TEST(OffensivePlayerTest, UpdateTankWithBattleInfoSetsTarget) {
    // Board: enemy tank at (2,0), own tank at (1,1)
    std::vector<std::vector<char>> board = {
        {' ', ' ', '2'},
        {' ', '%', ' '},
        {' ', ' ', ' '}
    };
    Player_318835816_211314471_A player(1, 3, 3, 10, 5);
    MockSatelliteView view(board);
    MockTankAlgorithm algo;
    player.updateTankWithBattleInfo(algo, view);
    // The tank algorithm should have been called
    EXPECT_TRUE(algo.updateCalled);
    // The info passed should be an OffensiveBattleInfo with target set to (2,0)
    auto* info = dynamic_cast<OffensiveBattleInfo*>(algo.lastInfo);
    ASSERT_NE(info, nullptr);
    ASSERT_TRUE(info->getTargetTankPosition().has_value());
    EXPECT_EQ(info->getTargetTankPosition().value(), Point(2, 0));
}

TEST(OffensivePlayerTest, KeepsTrackingTargetIfItMoves) {
    // Initial board: enemy tank at (2,0), own tank at (1,1)
    std::vector<std::vector<char>> board1 = {
        {' ', ' ', '2'},
        {' ', '%', ' '},
        {' ', ' ', ' '}
    };
    Player_318835816_211314471_A player(1, 3, 3, 10, 5);
    MockSatelliteView view1(board1);
    MockTankAlgorithm algo1;
    player.updateTankWithBattleInfo(algo1, view1);
    auto* info1 = dynamic_cast<OffensiveBattleInfo*>(algo1.lastInfo);
    ASSERT_NE(info1, nullptr);
    ASSERT_TRUE(info1->getTargetTankPosition().has_value());
    EXPECT_EQ(info1->getTargetTankPosition().value(), Point(2, 0));

    // Move the enemy tank to (1,0) (simulate tank movement)
    std::vector<std::vector<char>> board2 = {
        {' ', '2', ' '},
        {' ', '%', ' '},
        {' ', ' ', ' '}
    };
    MockSatelliteView view2(board2);
    MockTankAlgorithm algo2;
    player.updateTankWithBattleInfo(algo2, view2);
    auto* info2 = dynamic_cast<OffensiveBattleInfo*>(algo2.lastInfo);
    ASSERT_NE(info2, nullptr);
    ASSERT_TRUE(info2->getTargetTankPosition().has_value());
    // Should now track the new position of the same tank (since only one enemy tank)
    EXPECT_EQ(info2->getTargetTankPosition().value(), Point(1, 0));
}

TEST(OffensivePlayerTest, ChoosesClosestTankAfterTargetDestroyed) {
    // Initial board: enemy tank at (2,0), own tank at (1,1)
    // Enemy tanks at (0,0) and (2,0), should pick (0,0) as closest to (0,0)
    std::vector<std::vector<char>> board1 = {
        {'2', ' ', '3'},
        {' ', '%', ' '},
        {' ', ' ', ' '}
    };
    Player_318835816_211314471_A player(1, 3, 3, 10, 5);
    MockSatelliteView view1(board1);
    MockTankAlgorithm algo1;
    player.updateTankWithBattleInfo(algo1, view1);
    auto* info1 = dynamic_cast<OffensiveBattleInfo*>(algo1.lastInfo);
    ASSERT_NE(info1, nullptr);
    ASSERT_TRUE(info1->getTargetTankPosition().has_value());

    // Remove the initial target (simulate destruction), only (2,0) remains
    std::vector<std::vector<char>> board2 = {
        {' ', ' ', '3'},
        {' ', '%', ' '},
        {' ', ' ', ' '}
    };
    MockSatelliteView view2(board2);
    MockTankAlgorithm algo2;
    player.updateTankWithBattleInfo(algo2, view2);
    auto* info2 = dynamic_cast<OffensiveBattleInfo*>(algo2.lastInfo);
    ASSERT_NE(info2, nullptr);
    ASSERT_TRUE(info2->getTargetTankPosition().has_value());
    // Should now pick (2,0) as the new target (closest to last known target)
    EXPECT_EQ(info2->getTargetTankPosition().value(), Point(2, 0));
}

TEST(OffensivePlayerTest, ChoosesClosestOfMultipleTanksAfterTargetDestroyed) {
    // Initial board: three enemy tanks, own tank at (1,1)
    // Enemy tanks at (0,0), (2,0), (2,2). Should pick (0,0) as closest to (0,0)
    std::vector<std::vector<char>> board1 = {
        {'2', ' ', '2', ' '},
        {' ', '%', ' ', ' '},
        {' ', ' ', '2', ' '}
    };
    Player_318835816_211314471_A player(1, 4, 3, 10, 5);
    MockSatelliteView view1(board1);
    MockTankAlgorithm algo1;
    player.updateTankWithBattleInfo(algo1, view1);
    auto* info1 = dynamic_cast<OffensiveBattleInfo*>(algo1.lastInfo);
    ASSERT_NE(info1, nullptr);
    ASSERT_TRUE(info1->getTargetTankPosition().has_value());
    Point initialTarget = info1->getTargetTankPosition().value();
    EXPECT_EQ(initialTarget, Point(0, 0));

    // Remove the initial target (simulate destruction), (2,0) and (2,2) remain
    std::vector<std::vector<char>> board2 = {
        {' ', ' ', '2', ' '},
        {' ', '%', ' ', ' '},
        {' ', ' ', ' ', '2'}
    };
    MockSatelliteView view2(board2);
    MockTankAlgorithm algo2;
    player.updateTankWithBattleInfo(algo2, view2);
    auto* info2 = dynamic_cast<OffensiveBattleInfo*>(algo2.lastInfo);
    ASSERT_NE(info2, nullptr);
    ASSERT_TRUE(info2->getTargetTankPosition().has_value());
    // Should now pick (2,2) as the new target (closer to last known target (0,0) than (2,2))
    EXPECT_EQ(info2->getTargetTankPosition().value(), Point(3, 2));
}

} // namespace Algorithm_318835816_211314471 