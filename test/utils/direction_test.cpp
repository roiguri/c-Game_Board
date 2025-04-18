#include "gtest/gtest.h"
#include "utils/direction.h"
#include "utils/point.h"

TEST(DirectionTest, DirectionToString) {
    EXPECT_EQ(directionToString(Direction::Up), "Up");
    EXPECT_EQ(directionToString(Direction::UpRight), "UpRight");
    EXPECT_EQ(directionToString(Direction::Right), "Right");
    EXPECT_EQ(directionToString(Direction::DownRight), "DownRight");
    EXPECT_EQ(directionToString(Direction::Down), "Down");
    EXPECT_EQ(directionToString(Direction::DownLeft), "DownLeft");
    EXPECT_EQ(directionToString(Direction::Left), "Left");
    EXPECT_EQ(directionToString(Direction::UpLeft), "UpLeft");
}

TEST(DirectionTest, RotateLeftEighth) {
    EXPECT_EQ(rotateLeft(Direction::Up, false), Direction::UpLeft);
    EXPECT_EQ(rotateLeft(Direction::UpRight, false), Direction::Up);
    EXPECT_EQ(rotateLeft(Direction::Right, false), Direction::UpRight);
    EXPECT_EQ(rotateLeft(Direction::DownRight, false), Direction::Right);
    EXPECT_EQ(rotateLeft(Direction::Down, false), Direction::DownRight);
    EXPECT_EQ(rotateLeft(Direction::DownLeft, false), Direction::Down);
    EXPECT_EQ(rotateLeft(Direction::Left, false), Direction::DownLeft);
    EXPECT_EQ(rotateLeft(Direction::UpLeft, false), Direction::Left);
}

TEST(DirectionTest, RotateLeftQuarter) {
    EXPECT_EQ(rotateLeft(Direction::Up, true), Direction::Left);
    EXPECT_EQ(rotateLeft(Direction::UpRight, true), Direction::UpLeft);
    EXPECT_EQ(rotateLeft(Direction::Right, true), Direction::Up);
    EXPECT_EQ(rotateLeft(Direction::DownRight, true), Direction::UpRight);
    EXPECT_EQ(rotateLeft(Direction::Down, true), Direction::Right);
    EXPECT_EQ(rotateLeft(Direction::DownLeft, true), Direction::DownRight);
    EXPECT_EQ(rotateLeft(Direction::Left, true), Direction::Down);
    EXPECT_EQ(rotateLeft(Direction::UpLeft, true), Direction::DownLeft);
}

TEST(DirectionTest, RotateRightEighth) {
    EXPECT_EQ(rotateRight(Direction::Up, false), Direction::UpRight);
    EXPECT_EQ(rotateRight(Direction::UpRight, false), Direction::Right);
    EXPECT_EQ(rotateRight(Direction::Right, false), Direction::DownRight);
    EXPECT_EQ(rotateRight(Direction::DownRight, false), Direction::Down);
    EXPECT_EQ(rotateRight(Direction::Down, false), Direction::DownLeft);
    EXPECT_EQ(rotateRight(Direction::DownLeft, false), Direction::Left);
    EXPECT_EQ(rotateRight(Direction::Left, false), Direction::UpLeft);
    EXPECT_EQ(rotateRight(Direction::UpLeft, false), Direction::Up);
}

TEST(DirectionTest, RotateRightQuarter) {
    EXPECT_EQ(rotateRight(Direction::Up, true), Direction::Right);
    EXPECT_EQ(rotateRight(Direction::UpRight, true), Direction::DownRight);
    EXPECT_EQ(rotateRight(Direction::Right, true), Direction::Down);
    EXPECT_EQ(rotateRight(Direction::DownRight, true), Direction::DownLeft);
    EXPECT_EQ(rotateRight(Direction::Down, true), Direction::Left);
    EXPECT_EQ(rotateRight(Direction::DownLeft, true), Direction::UpLeft);
    EXPECT_EQ(rotateRight(Direction::Left, true), Direction::Up);
    EXPECT_EQ(rotateRight(Direction::UpLeft, true), Direction::UpRight);
}

TEST(DirectionTest, GetDirectionDeltaDefault) {
    EXPECT_EQ(getDirectionDelta(Direction::Up), Point(0, -1));
    EXPECT_EQ(getDirectionDelta(Direction::UpRight), Point(1, -1));
    EXPECT_EQ(getDirectionDelta(Direction::Right), Point(1, 0));
    EXPECT_EQ(getDirectionDelta(Direction::DownRight), Point(1, 1));
    EXPECT_EQ(getDirectionDelta(Direction::Down), Point(0, 1));
    EXPECT_EQ(getDirectionDelta(Direction::DownLeft), Point(-1, 1));
    EXPECT_EQ(getDirectionDelta(Direction::Left), Point(-1, 0));
    EXPECT_EQ(getDirectionDelta(Direction::UpLeft), Point(-1, -1));
}

TEST(DirectionTest, GetDirectionDeltaCustomMagnitude) {
    int magnitude = 2;
    EXPECT_EQ(getDirectionDelta(Direction::Up, magnitude), Point(0, -2));
    EXPECT_EQ(getDirectionDelta(Direction::UpRight, magnitude), Point(2, -2));
    EXPECT_EQ(getDirectionDelta(Direction::Right, magnitude), Point(2, 0));
    EXPECT_EQ(getDirectionDelta(Direction::DownRight, magnitude), Point(2, 2));
    EXPECT_EQ(getDirectionDelta(Direction::Down, magnitude), Point(0, 2));
    EXPECT_EQ(getDirectionDelta(Direction::DownLeft, magnitude), Point(-2, 2));
    EXPECT_EQ(getDirectionDelta(Direction::Left, magnitude), Point(-2, 0));
    EXPECT_EQ(getDirectionDelta(Direction::UpLeft, magnitude), Point(-2, -2));
}

TEST(DirectionTest, FullRotationSequence) {
    Direction dir = Direction::Up;
    
    // Rotate left 8 times (should end up back at Up)
    for (int i = 0; i < 8; i++) {
        dir = rotateLeft(dir, false);
    }
    EXPECT_EQ(dir, Direction::Up);
    
    // Rotate right 8 times (should end up back at Up)
    for (int i = 0; i < 8; i++) {
        dir = rotateRight(dir, false);
    }
    EXPECT_EQ(dir, Direction::Up);
    
    // Rotate left 4 times with quarter turns (should end up back at Up)
    dir = Direction::Up;
    for (int i = 0; i < 4; i++) {
        dir = rotateLeft(dir, true);
    }
    EXPECT_EQ(dir, Direction::Up);
    
    // Rotate right 4 times with quarter turns (should end up back at Up)
    dir = Direction::Up;
    for (int i = 0; i < 4; i++) {
        dir = rotateRight(dir, true);
    }
    EXPECT_EQ(dir, Direction::Up);
}

TEST(DirectionTest, OppositeDirections) {
    // One full half-rotation (4 eighth turns or 2 quarter turns) should give the opposite direction
    EXPECT_EQ(rotateLeft(rotateLeft(Direction::Up, true), true), Direction::Down); // Second quarter turn
    
    // Test all opposites
    EXPECT_EQ(rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::Up, false), false), false), false), Direction::Down);
    EXPECT_EQ(rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::UpRight, false), false), false), false), Direction::DownLeft);
    EXPECT_EQ(rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::Right, false), false), false), false), Direction::Left);
    EXPECT_EQ(rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::DownRight, false), false), false), false), Direction::UpLeft);
    EXPECT_EQ(rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::Down, false), false), false), false), Direction::Up);
    EXPECT_EQ(rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::DownLeft, false), false), false), false), Direction::UpRight);
    EXPECT_EQ(rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::Left, false), false), false), false), Direction::Right);
    EXPECT_EQ(rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::UpLeft, false), false), false), false), Direction::DownRight);
}

TEST(DirectionTest, StreamInsertionOperator) {
    std::ostringstream oss;
    oss << Direction::Up;
    EXPECT_EQ(oss.str(), "Up");
}