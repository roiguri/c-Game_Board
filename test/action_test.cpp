#include "gtest/gtest.h"
#include "action.h"

TEST(ActionTest, ActionToString) {
    EXPECT_EQ(actionToString(Action::MoveForward), "Move Forward");
    EXPECT_EQ(actionToString(Action::MoveBackward), "Move Backward");
    EXPECT_EQ(actionToString(Action::RotateLeftEighth), "Rotate Left 1/8");
    EXPECT_EQ(actionToString(Action::RotateRightEighth), "Rotate Right 1/8");
    EXPECT_EQ(actionToString(Action::RotateLeftQuarter), "Rotate Left 1/4");
    EXPECT_EQ(actionToString(Action::RotateRightQuarter), "Rotate Right 1/4");
    EXPECT_EQ(actionToString(Action::Shoot), "Shoot");
    EXPECT_EQ(actionToString(Action::None), "None");
}