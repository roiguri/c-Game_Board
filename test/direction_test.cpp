#include "gtest/gtest.h"
#include "direction.h"
#include "point.h"

TEST(DirectionTest, DirectionToString) {
    EXPECT_EQ("Up", directionToString(Direction::Up));
    EXPECT_EQ("UpRight", directionToString(Direction::UpRight));
    EXPECT_EQ("Right", directionToString(Direction::Right));
    EXPECT_EQ("DownRight", directionToString(Direction::DownRight));
    EXPECT_EQ("Down", directionToString(Direction::Down));
    EXPECT_EQ("DownLeft", directionToString(Direction::DownLeft));
    EXPECT_EQ("Left", directionToString(Direction::Left));
    EXPECT_EQ("UpLeft", directionToString(Direction::UpLeft));
}

TEST(DirectionTest, RotateLeftEighth) {
    EXPECT_EQ(Direction::UpLeft, rotateLeft(Direction::Up, false));
    EXPECT_EQ(Direction::Up, rotateLeft(Direction::UpRight, false));
    EXPECT_EQ(Direction::UpRight, rotateLeft(Direction::Right, false));
    EXPECT_EQ(Direction::Right, rotateLeft(Direction::DownRight, false));
    EXPECT_EQ(Direction::DownRight, rotateLeft(Direction::Down, false));
    EXPECT_EQ(Direction::Down, rotateLeft(Direction::DownLeft, false));
    EXPECT_EQ(Direction::DownLeft, rotateLeft(Direction::Left, false));
    EXPECT_EQ(Direction::Left, rotateLeft(Direction::UpLeft, false));
}

TEST(DirectionTest, RotateLeftQuarter) {
    EXPECT_EQ(Direction::Left, rotateLeft(Direction::Up, true));
    EXPECT_EQ(Direction::UpLeft, rotateLeft(Direction::UpRight, true));
    EXPECT_EQ(Direction::Up, rotateLeft(Direction::Right, true));
    EXPECT_EQ(Direction::UpRight, rotateLeft(Direction::DownRight, true));
    EXPECT_EQ(Direction::Right, rotateLeft(Direction::Down, true));
    EXPECT_EQ(Direction::DownRight, rotateLeft(Direction::DownLeft, true));
    EXPECT_EQ(Direction::Down, rotateLeft(Direction::Left, true));
    EXPECT_EQ(Direction::DownLeft, rotateLeft(Direction::UpLeft, true));
}

TEST(DirectionTest, RotateRightEighth) {
    EXPECT_EQ(Direction::UpRight, rotateRight(Direction::Up, false));
    EXPECT_EQ(Direction::Right, rotateRight(Direction::UpRight, false));
    EXPECT_EQ(Direction::DownRight, rotateRight(Direction::Right, false));
    EXPECT_EQ(Direction::Down, rotateRight(Direction::DownRight, false));
    EXPECT_EQ(Direction::DownLeft, rotateRight(Direction::Down, false));
    EXPECT_EQ(Direction::Left, rotateRight(Direction::DownLeft, false));
    EXPECT_EQ(Direction::UpLeft, rotateRight(Direction::Left, false));
    EXPECT_EQ(Direction::Up, rotateRight(Direction::UpLeft, false));
}

TEST(DirectionTest, RotateRightQuarter) {
    EXPECT_EQ(Direction::Right, rotateRight(Direction::Up, true));
    EXPECT_EQ(Direction::DownRight, rotateRight(Direction::UpRight, true));
    EXPECT_EQ(Direction::Down, rotateRight(Direction::Right, true));
    EXPECT_EQ(Direction::DownLeft, rotateRight(Direction::DownRight, true));
    EXPECT_EQ(Direction::Left, rotateRight(Direction::Down, true));
    EXPECT_EQ(Direction::UpLeft, rotateRight(Direction::DownLeft, true));
    EXPECT_EQ(Direction::Up, rotateRight(Direction::Left, true));
    EXPECT_EQ(Direction::UpRight, rotateRight(Direction::UpLeft, true));
}

TEST(DirectionTest, GetDirectionDeltaDefault) {
    EXPECT_EQ(Point(0, -1), getDirectionDelta(Direction::Up));
    EXPECT_EQ(Point(1, -1), getDirectionDelta(Direction::UpRight));
    EXPECT_EQ(Point(1, 0), getDirectionDelta(Direction::Right));
    EXPECT_EQ(Point(1, 1), getDirectionDelta(Direction::DownRight));
    EXPECT_EQ(Point(0, 1), getDirectionDelta(Direction::Down));
    EXPECT_EQ(Point(-1, 1), getDirectionDelta(Direction::DownLeft));
    EXPECT_EQ(Point(-1, 0), getDirectionDelta(Direction::Left));
    EXPECT_EQ(Point(-1, -1), getDirectionDelta(Direction::UpLeft));
}

TEST(DirectionTest, GetDirectionDeltaCustomMagnitude) {
    int magnitude = 2;
    EXPECT_EQ(Point(0, -2), getDirectionDelta(Direction::Up, magnitude));
    EXPECT_EQ(Point(2, -2), getDirectionDelta(Direction::UpRight, magnitude));
    EXPECT_EQ(Point(2, 0), getDirectionDelta(Direction::Right, magnitude));
    EXPECT_EQ(Point(2, 2), getDirectionDelta(Direction::DownRight, magnitude));
    EXPECT_EQ(Point(0, 2), getDirectionDelta(Direction::Down, magnitude));
    EXPECT_EQ(Point(-2, 2), getDirectionDelta(Direction::DownLeft, magnitude));
    EXPECT_EQ(Point(-2, 0), getDirectionDelta(Direction::Left, magnitude));
    EXPECT_EQ(Point(-2, -2), getDirectionDelta(Direction::UpLeft, magnitude));
}

TEST(DirectionTest, FullRotationSequence) {
    Direction dir = Direction::Up;
    
    // Rotate left 8 times (should end up back at Up)
    for (int i = 0; i < 8; i++) {
        dir = rotateLeft(dir, false);
    }
    EXPECT_EQ(Direction::Up, dir);
    
    // Rotate right 8 times (should end up back at Up)
    for (int i = 0; i < 8; i++) {
        dir = rotateRight(dir, false);
    }
    EXPECT_EQ(Direction::Up, dir);
    
    // Rotate left 4 times with quarter turns (should end up back at Up)
    dir = Direction::Up;
    for (int i = 0; i < 4; i++) {
        dir = rotateLeft(dir, true);
    }
    EXPECT_EQ(Direction::Up, dir);
    
    // Rotate right 4 times with quarter turns (should end up back at Up)
    dir = Direction::Up;
    for (int i = 0; i < 4; i++) {
        dir = rotateRight(dir, true);
    }
    EXPECT_EQ(Direction::Up, dir);
}

TEST(DirectionTest, OppositeDirections) {
    // One full half-rotation (4 eighth turns or 2 quarter turns) should give the opposite direction
    EXPECT_EQ(Direction::Down, rotateLeft(rotateLeft(Direction::Up, true), true)); // Second quarter turn
    
    // Test all opposites
    EXPECT_EQ(Direction::Down, rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::Up, false), false), false), false));
    EXPECT_EQ(Direction::DownLeft, rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::UpRight, false), false), false), false));
    EXPECT_EQ(Direction::Left, rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::Right, false), false), false), false));
    EXPECT_EQ(Direction::UpLeft, rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::DownRight, false), false), false), false));
    EXPECT_EQ(Direction::Up, rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::Down, false), false), false), false));
    EXPECT_EQ(Direction::UpRight, rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::DownLeft, false), false), false), false));
    EXPECT_EQ(Direction::Right, rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::Left, false), false), false), false));
    EXPECT_EQ(Direction::DownRight, rotateLeft(rotateLeft(rotateLeft(rotateLeft(Direction::UpLeft, false), false), false), false));
}