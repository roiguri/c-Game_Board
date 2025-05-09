#include "gtest/gtest.h"
#include "players/basic_player.h"
#include "players/battle_info_impl.h"
#include "SatelliteView.h"
#include "TankAlgorithm.h"
#include "utils/point.h"
#include <vector>

// Mock SatelliteView
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

class BasicPlayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 3x3 board: wall, mine, tanks, shell, empty
        // Row 0: wall, friendly tank (player 1), enemy tank (player 2)
        // Row 1: shell, mine, current tank
        // Row 2: empty, empty, wall
        board = {
            {'#', '1', '2'},
            {'*', '@', '%'},
            {' ', ' ', '#'}
        };
        player = std::make_unique<BasicPlayer>(1, 3, 3, 10, 5);
    }
    std::vector<std::vector<char>> board;
    std::unique_ptr<BasicPlayer> player;

    // Helper methods to access private members/methods
    void callPopulateBattleInfo(SatelliteView& view) {
        player->populateBattleInfo(view);
    }
    BattleInfoImpl& getBattleInfo() {
        return player->m_battleInfo;
    }
    void callProcessTankFeedback(int tankId) {
        player->processTankFeedback(tankId);
    }
};

TEST_F(BasicPlayerTest, PopulatesBoardCellTypes) {
    MockSatelliteView view(board);
    callPopulateBattleInfo(view);
    // Check cell types
    auto& info = getBattleInfo();
    EXPECT_EQ(info.getCellType(0, 0), GameBoard::CellType::Wall);
    EXPECT_EQ(info.getCellType(1, 1), GameBoard::CellType::Mine);
    EXPECT_EQ(info.getCellType(0, 1), GameBoard::CellType::Empty); // shell is not a cell type
    EXPECT_EQ(info.getCellType(2, 2), GameBoard::CellType::Wall);
}

TEST_F(BasicPlayerTest, PopulatesTankAndShellPositions) {
    MockSatelliteView view(board);
    callPopulateBattleInfo(view);
    auto& info = getBattleInfo();
    // Friendly tanks: '1' and '%'
    EXPECT_EQ(info.getFriendlyTankPositions().size(), 2);
    EXPECT_EQ(info.getFriendlyTankPositions()[0], Point(1, 0));
    EXPECT_EQ(info.getFriendlyTankPositions()[1], Point(2, 1));
    // Enemy tanks: '2'
    EXPECT_EQ(info.getEnemyTankPositions().size(), 1);
    EXPECT_EQ(info.getEnemyTankPositions()[0], Point(2, 0));
    // Shells: '*'
    EXPECT_EQ(info.getShellPositions().size(), 1);
    EXPECT_EQ(info.getShellPositions()[0], Point(0, 1));
}

TEST_F(BasicPlayerTest, UpdateTankWithBattleInfoCallsAlgorithm) {
    MockSatelliteView view(board);
    MockTankAlgorithm algo;
    player->updateTankWithBattleInfo(algo, view);
    EXPECT_TRUE(algo.updateCalled);
    // Should pass the correct info object
    EXPECT_EQ(algo.lastInfo, &getBattleInfo());
}

TEST_F(BasicPlayerTest, ProcessTankFeedbackIsNoOp) {
    // Should not throw or do anything
    EXPECT_NO_THROW(callProcessTankFeedback(0));
} 