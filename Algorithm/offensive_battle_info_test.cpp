#include "gtest/gtest.h"
#include "offensive_battle_info.h"
#include "utils/point.h"

using namespace UserCommon_318835816_211314471;

namespace Algorithm_318835816_211314471 {

TEST(OffensiveBattleInfoTest, ConstructorInitializesBase) {
    OffensiveBattleInfo info(5, 6);
    EXPECT_EQ(info.getGameBoard().getWidth(), 5u);
    EXPECT_EQ(info.getGameBoard().getHeight(), 6u);
}

TEST(OffensiveBattleInfoTest, TargetTankPositionSetGetClear) {
    OffensiveBattleInfo info(3, 3);
    Point target(2, 1);
    EXPECT_FALSE(info.getTargetTankPosition().has_value());
    info.setTargetTankPosition(target);
    ASSERT_TRUE(info.getTargetTankPosition().has_value());
    EXPECT_EQ(info.getTargetTankPosition().value(), target);
    info.clearTargetTankPosition();
    EXPECT_FALSE(info.getTargetTankPosition().has_value());
}

TEST(OffensiveBattleInfoTest, DefaultTargetTankPositionIsNullopt) {
    OffensiveBattleInfo info(2, 2);
    EXPECT_FALSE(info.getTargetTankPosition().has_value());
}

} // namespace Algorithm_318835816_211314471 