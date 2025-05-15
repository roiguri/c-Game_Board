#include "gtest/gtest.h"
#include "players/basic_player_factory.h"
#include "players/basic_player.h"
#include "players/offensive_player.h"
#include "TankAlgorithm.h"
#include "SatelliteView.h"
#include <memory>

// Mock TankAlgorithm
class MockTankAlgorithm : public TankAlgorithm {
public:
    void updateBattleInfo(BattleInfo&) override {}
    ActionRequest getAction() override { return ActionRequest::GetBattleInfo; }
};

// Mock SatelliteView
class MockSatelliteView : public SatelliteView {
public:
    char getObjectAt(size_t, size_t) const override { return ' '; }
};

TEST(BasicPlayerFactoryTest, CreatesBasicPlayer) {
    BasicPlayerFactory factory;
    int playerIndex = 1;
    size_t x = 10, y = 8, maxSteps = 100, numShells = 5;
    std::unique_ptr<Player> player = factory.create(playerIndex, x, y, maxSteps, numShells);
    ASSERT_NE(player, nullptr);
    // Check type via dynamic_cast
    auto* basicPlayer = dynamic_cast<BasicPlayer*>(player.get());
    ASSERT_NE(basicPlayer, nullptr);
}

TEST(BasicPlayerFactoryTest, CreatesOffensivePlayer) {
    BasicPlayerFactory factory;
    int playerIndex = 2;
    size_t x = 10, y = 8, maxSteps = 100, numShells = 5;
    std::unique_ptr<Player> player = factory.create(playerIndex, x, y, maxSteps, numShells);
    ASSERT_NE(player, nullptr);
    // Check type via dynamic_cast
    auto* offensivePlayer = dynamic_cast<OffensivePlayer*>(player.get());
    ASSERT_NE(offensivePlayer, nullptr);
} 