#include "gtest/gtest.h"
#include "utils/direction.h"
#include "utils/point.h"

using namespace UserCommon_098765432_123456789;

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
    EXPECT_EQ(getRotateLeftDirection(Direction::Up, false), Direction::UpLeft);
    EXPECT_EQ(getRotateLeftDirection(Direction::UpRight, false), Direction::Up);
    EXPECT_EQ(getRotateLeftDirection(Direction::Right, false), Direction::UpRight);
    EXPECT_EQ(getRotateLeftDirection(Direction::DownRight, false), Direction::Right);
    EXPECT_EQ(getRotateLeftDirection(Direction::Down, false), Direction::DownRight);
    EXPECT_EQ(getRotateLeftDirection(Direction::DownLeft, false), Direction::Down);
    EXPECT_EQ(getRotateLeftDirection(Direction::Left, false), Direction::DownLeft);
    EXPECT_EQ(getRotateLeftDirection(Direction::UpLeft, false), Direction::Left);
}

TEST(DirectionTest, RotateLeftQuarter) {
    EXPECT_EQ(getRotateLeftDirection(Direction::Up, true), Direction::Left);
    EXPECT_EQ(getRotateLeftDirection(Direction::UpRight, true), Direction::UpLeft);
    EXPECT_EQ(getRotateLeftDirection(Direction::Right, true), Direction::Up);
    EXPECT_EQ(getRotateLeftDirection(Direction::DownRight, true), Direction::UpRight);
    EXPECT_EQ(getRotateLeftDirection(Direction::Down, true), Direction::Right);
    EXPECT_EQ(getRotateLeftDirection(Direction::DownLeft, true), Direction::DownRight);
    EXPECT_EQ(getRotateLeftDirection(Direction::Left, true), Direction::Down);
    EXPECT_EQ(getRotateLeftDirection(Direction::UpLeft, true), Direction::DownLeft);
}

TEST(DirectionTest, RotateRightEighth) {
    EXPECT_EQ(getRotateRightDirection(Direction::Up, false), Direction::UpRight);
    EXPECT_EQ(getRotateRightDirection(Direction::UpRight, false), Direction::Right);
    EXPECT_EQ(getRotateRightDirection(Direction::Right, false), Direction::DownRight);
    EXPECT_EQ(getRotateRightDirection(Direction::DownRight, false), Direction::Down);
    EXPECT_EQ(getRotateRightDirection(Direction::Down, false), Direction::DownLeft);
    EXPECT_EQ(getRotateRightDirection(Direction::DownLeft, false), Direction::Left);
    EXPECT_EQ(getRotateRightDirection(Direction::Left, false), Direction::UpLeft);
    EXPECT_EQ(getRotateRightDirection(Direction::UpLeft, false), Direction::Up);
}

TEST(DirectionTest, RotateRightQuarter) {
    EXPECT_EQ(getRotateRightDirection(Direction::Up, true), Direction::Right);
    EXPECT_EQ(getRotateRightDirection(Direction::UpRight, true), Direction::DownRight);
    EXPECT_EQ(getRotateRightDirection(Direction::Right, true), Direction::Down);
    EXPECT_EQ(getRotateRightDirection(Direction::DownRight, true), Direction::DownLeft);
    EXPECT_EQ(getRotateRightDirection(Direction::Down, true), Direction::Left);
    EXPECT_EQ(getRotateRightDirection(Direction::DownLeft, true), Direction::UpLeft);
    EXPECT_EQ(getRotateRightDirection(Direction::Left, true), Direction::Up);
    EXPECT_EQ(getRotateRightDirection(Direction::UpLeft, true), Direction::UpRight);
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
        dir = getRotateLeftDirection(dir, false);
    }
    EXPECT_EQ(dir, Direction::Up);
    
    // Rotate right 8 times (should end up back at Up)
    for (int i = 0; i < 8; i++) {
        dir = getRotateRightDirection(dir, false);
    }
    EXPECT_EQ(dir, Direction::Up);
    
    // Rotate left 4 times with quarter turns (should end up back at Up)
    dir = Direction::Up;
    for (int i = 0; i < 4; i++) {
        dir = getRotateLeftDirection(dir, true);
    }
    EXPECT_EQ(dir, Direction::Up);
    
    // Rotate right 4 times with quarter turns (should end up back at Up)
    dir = Direction::Up;
    for (int i = 0; i < 4; i++) {
        dir = getRotateRightDirection(dir, true);
    }
    EXPECT_EQ(dir, Direction::Up);
}

TEST(DirectionTest, OppositeDirections) {
    // One full half-rotation (4 eighth turns or 2 quarter turns) should give the opposite direction
    EXPECT_EQ(getRotateLeftDirection(getRotateLeftDirection(Direction::Up, true), true), Direction::Down); // Second quarter turn
    
    // Test all opposites
    EXPECT_EQ(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(Direction::Up, false), false), false), false), Direction::Down);
    EXPECT_EQ(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(Direction::UpRight, false), false), false), false), Direction::DownLeft);
    EXPECT_EQ(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(Direction::Right, false), false), false), false), Direction::Left);
    EXPECT_EQ(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(Direction::DownRight, false), false), false), false), Direction::UpLeft);
    EXPECT_EQ(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(Direction::Down, false), false), false), false), Direction::Up);
    EXPECT_EQ(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(Direction::DownLeft, false), false), false), false), Direction::UpRight);
    EXPECT_EQ(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(Direction::Left, false), false), false), false), Direction::Right);
    EXPECT_EQ(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(getRotateLeftDirection(Direction::UpLeft, false), false), false), false), Direction::DownRight);
}

TEST(DirectionTest, StreamInsertionOperator) {
    std::ostringstream oss;
    oss << Direction::Up;
    EXPECT_EQ(oss.str(), "Up");
}

TEST(DirectionTest, GetDirectionToPoint_AdjacentPoints) {
  Point center(5, 5);

  // Test all 8 adjacent directions
  struct TestCase {
      Point target;
      Direction expectedDir;
  };

  TestCase testCases[] = {
      {Point(5, 4), Direction::Up},     
      {Point(6, 4), Direction::UpRight}, 
      {Point(6, 5), Direction::Right},  
      {Point(6, 6), Direction::DownRight},
      {Point(5, 6), Direction::Down}, 
      {Point(4, 6), Direction::DownLeft},
      {Point(4, 5), Direction::Left},   
      {Point(4, 4), Direction::UpLeft}   
  };

  for (const auto& tc : testCases) {
      std::optional<Direction> result = getDirectionToPoint(center, tc.target);
      ASSERT_TRUE(result.has_value());
      EXPECT_EQ(result.value(), tc.expectedDir);
  }
}

TEST(DirectionTest, GetDirectionToPoint_SamePoint) {
  Point p1(10, 10);
  Point p2(10, 10);

  std::optional<Direction> result = getDirectionToPoint(p1, p2);

  EXPECT_FALSE(result.has_value());
}

TEST(DirectionTest, GetDirectionToPoint_NonAdjacentPoints) {
  Point p1(3, 3);

  Point nonAdjacent[] = {
      Point(3, 5),
      Point(5, 3),
      Point(1, 1),
      Point(5, 5),
      Point(4, 1)
  };

  for (const auto& p2 : nonAdjacent) {
      std::optional<Direction> result = getDirectionToPoint(p1, p2);
      EXPECT_FALSE(result.has_value());
  }
}
