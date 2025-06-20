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

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_HorizontalAdjacent) {
  Point a(1, 5);
  Point b(2, 5);
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 1);
  EXPECT_EQ(mp.getY(), 5);
  EXPECT_TRUE(mp.isHalfX());
  EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_VerticalAdjacent) {
  Point a(7, 3);
  Point b(7, 4);
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 7);
  EXPECT_EQ(mp.getY(), 3);
  EXPECT_FALSE(mp.isHalfX());
  EXPECT_TRUE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_HorizontalWraparound) {
  Point a(0, 5);
  Point b(9, 5);  // In a 10-wide board, this is adjacent to (0,5) due to wrapping
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 9);
  EXPECT_EQ(mp.getY(), 5);
  EXPECT_TRUE(mp.isHalfX());
  EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_VerticalWraparound) {
  Point a(5, 0);
  Point b(5, 7);  // In an 8-high board, this is adjacent to (5,0) due to wrapping
  int boardWidth = 10;
  int boardHeight = 8;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 5);
  EXPECT_EQ(mp.getY(), 7);
  EXPECT_FALSE(mp.isHalfX());
  EXPECT_TRUE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_WraparoundReversed) {
  Point a(9, 5);
  Point b(0, 5);  // Same as previous test but reversed order
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 9);
  EXPECT_EQ(mp.getY(), 5);
  EXPECT_TRUE(mp.isHalfX());
  EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_NonAdjacent) {
  Point a(2, 2);
  Point b(5, 5);  // Non-adjacent points
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), -1);
  EXPECT_EQ(mp.getY(), -1);
  EXPECT_FALSE(mp.isHalfX());
  EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, MidpointsMatch_WithBoardDimensions_SameMidpoint) {
  // Two different pairs with same midpoint considering wrapping
  Point a1(0, 3);
  Point a2(9, 2);  // Wraps around (9.5, 2.5) 
  Point b1(9, 3);
  Point b2(0, 2);  // Also produces midpoint at (9.5, 2.5)
  
  int boardWidth = 10;
  int boardHeight = 10;
  
  EXPECT_TRUE(MidPoint::midpointsMatch(a1, a2, b1, b2, boardWidth, boardHeight));
}

TEST_F(MidPointTest, MidpointsMatch_WithBoardDimensions_DifferentMidpoints) {
  Point a1(0, 3);
  Point a2(9, 3);  // Midpoint is (9.5, 3)
  Point b1(1, 3);
  Point b2(2, 3);  // Midpoint is (1.5, 3)
  
  int boardWidth = 10;
  int boardHeight = 10;
  
  EXPECT_FALSE(MidPoint::midpointsMatch(a1, a2, b1, b2, boardWidth, boardHeight));
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_SamePoint) {
  Point a(3, 3);
  Point b(3, 3);
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 3);
  EXPECT_EQ(mp.getY(), 3);
  EXPECT_FALSE(mp.isHalfX());
  EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_NonAdjacentPoints) {
  Point a(1, 1);
  Point b(3, 3);
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  // Should return an invalid midpoint
  EXPECT_EQ(mp.getX(), -1);
  EXPECT_EQ(mp.getY(), -1);
  EXPECT_FALSE(mp.isHalfX());
  EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_DiagonalAdjacent) {
  Point a(1, 1);
  Point b(2, 2);
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 1);
  EXPECT_EQ(mp.getY(), 1);
  EXPECT_TRUE(mp.isHalfX());
  EXPECT_TRUE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_HorizontalSameY) {
  Point a(4, 5);
  Point b(5, 5);
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 4);
  EXPECT_EQ(mp.getY(), 5);
  EXPECT_TRUE(mp.isHalfX());
  EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_VerticalSameX) {
  Point a(7, 2);
  Point b(7, 3);
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 7);
  EXPECT_EQ(mp.getY(), 2);
  EXPECT_FALSE(mp.isHalfX());
  EXPECT_TRUE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_HorizontalWrapLeftToRight) {
  Point a(0, 5);
  Point b(9, 5);
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 9);
  EXPECT_EQ(mp.getY(), 5);
  EXPECT_TRUE(mp.isHalfX());
  EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_HorizontalWrapRightToLeft) {
  Point a(9, 5);
  Point b(0, 5);
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 9);
  EXPECT_EQ(mp.getY(), 5);
  EXPECT_TRUE(mp.isHalfX());
  EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_VerticalWrapTopToBottom) {
  Point a(5, 0);
  Point b(5, 9);
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 5);
  EXPECT_EQ(mp.getY(), 9);
  EXPECT_FALSE(mp.isHalfX());
  EXPECT_TRUE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_VerticalWrapBottomToTop) {
  Point a(5, 9);
  Point b(5, 0);
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 5);
  EXPECT_EQ(mp.getY(), 9);
  EXPECT_FALSE(mp.isHalfX());
  EXPECT_TRUE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_DiagonalWrap) {
  Point a(0, 0);
  Point b(9, 9);
  int boardWidth = 10;
  int boardHeight = 10;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  EXPECT_EQ(mp.getX(), 9);
  EXPECT_EQ(mp.getY(), 9);
  EXPECT_TRUE(mp.isHalfX());
  EXPECT_TRUE(mp.isHalfY());
}

TEST_F(MidPointTest, CalculateMidpoint_WithBoardDimensions_SmallBoard) {
  Point a(0, 0);
  Point b(2, 0);
  int boardWidth = 3;
  int boardHeight = 3;
  
  MidPoint mp = MidPoint::calculateMidpoint(a, b, boardWidth, boardHeight);
  
  // In a 3x3 board, positions 0 and 2 are adjacent due to wraparound
  EXPECT_EQ(mp.getX(), 2);
  EXPECT_EQ(mp.getY(), 0);
  EXPECT_TRUE(mp.isHalfX());
  EXPECT_FALSE(mp.isHalfY());
}

TEST_F(MidPointTest, MidpointsMatch_WithBoardDimensions_DiagonalWraparoundMatch) {
  Point a1(0, 0);
  Point a2(9, 9);
  Point b1(9, 0);
  Point b2(0, 9);
  
  int boardWidth = 10;
  int boardHeight = 10;

  MidPoint mp1 = MidPoint::calculateMidpoint(a1, a2, boardWidth, boardHeight);
  
  EXPECT_EQ(mp1.getX(), 9);
  EXPECT_EQ(mp1.getY(), 9);
  EXPECT_TRUE(mp1.isHalfX());
  EXPECT_TRUE(mp1.isHalfY());

  MidPoint mp2 = MidPoint::calculateMidpoint(b1, b2, boardWidth, boardHeight);
  
  EXPECT_EQ(mp2.getX(), 9);
  EXPECT_EQ(mp2.getY(), 9);
  EXPECT_TRUE(mp2.isHalfX());
  EXPECT_TRUE(mp2.isHalfY());
  
  // Both should have midpoints at corner positions with half flags
  EXPECT_TRUE(MidPoint::midpointsMatch(a1, a2, b1, b2, boardWidth, boardHeight));
}

TEST_F(MidPointTest, MidpointsMatch_WithBoardDimensions_AdjacentDifferentAxes) {
  Point a1(1, 1);
  Point a2(2, 1);  // Horizontal adjacency
  Point b1(3, 2);
  Point b2(3, 3);  // Vertical adjacency
  
  int boardWidth = 10;
  int boardHeight = 10;
  
  EXPECT_FALSE(MidPoint::midpointsMatch(a1, a2, b1, b2, boardWidth, boardHeight));
}

TEST_F(MidPointTest, MidpointsMatch_WithBoardDimensions_InvalidMidpoints) {
  Point a1(1, 1);
  Point a2(3, 3);  // Not adjacent
  Point b1(5, 5);
  Point b2(6, 5);  // Adjacent
  
  int boardWidth = 10;
  int boardHeight = 10;
  
  // First pair isn't adjacent, should return an invalid midpoint
  EXPECT_FALSE(MidPoint::midpointsMatch(a1, a2, b1, b2, boardWidth, boardHeight));
}