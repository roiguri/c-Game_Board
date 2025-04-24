#include "gtest/gtest.h"
#include "utils/midpoint.h"

class MidPointTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

TEST_F(MidPointTest, DefaultConstructor) {
    MidPoint mp;
    EXPECT_EQ(mp.getX(), 0);
    EXPECT_EQ(mp.getY(), 0);
    EXPECT_FALSE(mp.isHalfX());
    EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, ParameterizedConstructor) {
    MidPoint mp(3, 4, true, false);
    EXPECT_EQ(mp.getX(), 3);
    EXPECT_EQ(mp.getY(), 4);
    EXPECT_TRUE(mp.isHalfX());
    EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, PointConstructor) {
    Point p(5, 6);
    MidPoint mp(p);
    EXPECT_EQ(mp.getX(), 5);
    EXPECT_EQ(mp.getY(), 6);
    EXPECT_FALSE(mp.isHalfX());
    EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_HorizontalAdjacent) {
    Point a(1, 5);
    Point b(2, 5);
    
    MidPoint mp = MidPoint::calculateMidpoint(a, b);
    
    EXPECT_EQ(mp.getX(), 1);
    EXPECT_EQ(mp.getY(), 5);
    EXPECT_TRUE(mp.isHalfX());
    EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_VerticalAdjacent) {
    Point a(7, 3);
    Point b(7, 4);
    
    MidPoint mp = MidPoint::calculateMidpoint(a, b);
    
    EXPECT_EQ(mp.getX(), 7);
    EXPECT_EQ(mp.getY(), 3);
    EXPECT_FALSE(mp.isHalfX());
    EXPECT_TRUE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_DiagonalAdjacent) {
    Point a(2, 2);
    Point b(3, 3);
    
    MidPoint mp = MidPoint::calculateMidpoint(a, b);
    
    EXPECT_EQ(mp.getX(), 2);
    EXPECT_EQ(mp.getY(), 2);
    EXPECT_TRUE(mp.isHalfX());
    EXPECT_TRUE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_SamePoint) {
    Point a(4, 4);
    
    MidPoint mp = MidPoint::calculateMidpoint(a, a);
    
    EXPECT_EQ(mp.getX(), 4);
    EXPECT_EQ(mp.getY(), 4);
    EXPECT_FALSE(mp.isHalfX());
    EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, MidpointsMatch_SameMidpoint) {
    // Two different pairs of points with the same midpoint
    Point a1(1, 1);
    Point a2(2, 2);
    Point b1(1, 2);
    Point b2(2, 1);
    
    EXPECT_TRUE(MidPoint::midpointsMatch(a1, a2, b1, b2));
}

TEST_F(MidPointTest, MidpointsMatch_SameMidpointSwitched) {
  // Two different pairs of points with the same midpoint
  Point a1(1, 1);
  Point a2(2, 2);
  Point b1(2, 2);
  Point b2(1, 1);
  
  EXPECT_TRUE(MidPoint::midpointsMatch(a1, a2, b1, b2));
}

TEST_F(MidPointTest, MidpointsMatch_DifferentMidpoints) {
    Point a1(1, 1);
    Point a2(2, 2);
    Point b1(3, 3);
    Point b2(4, 4);
    
    EXPECT_FALSE(MidPoint::midpointsMatch(a1, a2, b1, b2));
}

TEST_F(MidPointTest, EqualityOperator) {
    MidPoint mp1(3, 4, true, false);
    MidPoint mp2(3, 4, true, false);
    MidPoint mp3(3, 4, false, false);
    
    EXPECT_TRUE(mp1 == mp2);
    EXPECT_FALSE(mp1 == mp3);
}

TEST_F(MidPointTest, InequalityOperator) {
    MidPoint mp1(3, 4, true, false);
    MidPoint mp2(3, 4, true, false);
    MidPoint mp3(3, 4, false, false);
    
    EXPECT_FALSE(mp1 != mp2);
    EXPECT_TRUE(mp1 != mp3);
}

TEST_F(MidPointTest, ToString) {
    MidPoint mp1(3, 4, true, true);
    MidPoint mp2(5, 6, false, true);
    MidPoint mp3(7, 8, false, false);
    
    EXPECT_EQ(mp1.toString(), "(3.5,4.5)");
    EXPECT_EQ(mp2.toString(), "(5,6.5)");
    EXPECT_EQ(mp3.toString(), "(7,8)");
}

TEST_F(MidPointTest, StreamOperator) {
    MidPoint mp(3, 4, true, false);
    std::ostringstream oss;
    oss << mp;
    EXPECT_EQ(oss.str(), "(3.5,4)");
}