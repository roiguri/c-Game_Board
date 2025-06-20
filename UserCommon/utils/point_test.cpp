#include <gtest/gtest.h>
#include <cmath>

#include "utils/point.h"

TEST(PointTest, DefaultConstructor) {
    Point p;
    EXPECT_EQ(p.getX(), 0);
    EXPECT_EQ(p.getY(), 0);
}

TEST(PointTest, ParameterizedConstructor) {
    Point p(5, 10);
    EXPECT_EQ(p.getX(), 5);
    EXPECT_EQ(p.getY(), 10);
}

TEST(PointTest, EqualityOperator) {
    Point p1(3, 4);
    Point p2(3, 4);
    Point p3(5, 4);
    
    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 == p3);
}

TEST(PointTest, InequalityOperator) {
    Point p1(3, 4);
    Point p2(3, 4);
    Point p3(5, 4);
    
    EXPECT_FALSE(p1 != p2);
    EXPECT_TRUE(p1 != p3);
}

TEST(PointTest, AdditionOperator) {
    Point p1(3, 4);
    Point p2(2, 5);
    Point result = p1 + p2;
    
    EXPECT_EQ(result.getX(), 5);
    EXPECT_EQ(result.getY(), 9);
}

TEST(PointTest, SubtractionOperator) {
    Point p1(7, 12);
    Point p2(3, 5);
    Point result = p1 - p2;
    
    EXPECT_EQ(result.getX(), 4);
    EXPECT_EQ(result.getY(), 7);
}

TEST(PointTest, LessThanOperator) {
  // Test x coordinate comparison
  Point p1(3, 5);
  Point p2(4, 2);  // Larger x-coordinate
  EXPECT_TRUE(p1 < p2);
  EXPECT_FALSE(p2 < p1);
  
  // Test y coordinate comparison when x is the same
  Point p3(3, 5);
  Point p4(3, 7);  // Same x, larger y-coordinate
  EXPECT_TRUE(p3 < p4);
  EXPECT_FALSE(p4 < p3);
  
  // Test equality case (neither is less than the other)
  Point p5(3, 5);
  Point p6(3, 5);
  EXPECT_FALSE(p5 < p6);
  EXPECT_FALSE(p6 < p5);
}

TEST(PointTest, ToString) {
    Point p(7, -3);
    EXPECT_EQ(p.toString(), "(7,-3)");
}

TEST(PointTest, StreamInsertionOperator) {
    std::ostringstream oss;
    Point p(3, 4);
    oss << p;
    EXPECT_EQ(oss.str(), "(3,4)");
}

TEST(PointTest, EuclideanDistance) {
  Point p1(0, 0);
  Point p2(3, 4);
  
  double distance = Point::euclideanDistance(p1, p2);
  EXPECT_DOUBLE_EQ(distance, 5.0); // sqrt(3² + 4²) = sqrt(9 + 16) = sqrt(25) = 5
}

TEST(PointTest, EuclideanDistanceFloatingPoint) {
  Point p1(1, 1);
  Point p2(4, 5);
  
  double distance = Point::euclideanDistance(p1, p2);
  EXPECT_DOUBLE_EQ(distance, 5.0); // sqrt((4-1)² + (5-1)²) = sqrt(9 + 16) = sqrt(25) = 5
}

TEST(PointTest, ZeroDifference) {
    Point p3(42, 42);
  Point p4(42, 42);
  
  EXPECT_DOUBLE_EQ(Point::euclideanDistance(p3, p4), 0.0);
}
