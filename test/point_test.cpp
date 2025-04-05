#include "point.h"
#include <gtest/gtest.h>
#include <cmath>

TEST(PointTest, DefaultConstructor) {
    Point p;
    EXPECT_EQ(0, p.x);
    EXPECT_EQ(0, p.y);
}

TEST(PointTest, ParameterizedConstructor) {
    Point p(5, 10);
    EXPECT_EQ(5, p.x);
    EXPECT_EQ(10, p.y);
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
    
    EXPECT_EQ(5, result.x);
    EXPECT_EQ(9, result.y);
}

TEST(PointTest, SubtractionOperator) {
    Point p1(7, 12);
    Point p2(3, 5);
    Point result = p1 - p2;
    
    EXPECT_EQ(4, result.x);
    EXPECT_EQ(7, result.y);
}

TEST(PointTest, ToString) {
    Point p(7, -3);
    EXPECT_EQ("(7,-3)", p.toString());
}

TEST(PointTest, ManhattanDistance) {
    Point p1(1, 2);
    Point p2(4, 6);
    
    int distance = Point::manhattanDistance(p1, p2);
    EXPECT_EQ(7, distance); // |4-1| + |6-2| = 3 + 4 = 7
}

TEST(PointTest, ManhattanDistanceWithNegativeCoordinates) {
    Point p1(-2, 3);
    Point p2(5, -1);
    
    int distance = Point::manhattanDistance(p1, p2);
    EXPECT_EQ(11, distance); // |5-(-2)| + |(-1)-3| = 7 + 4 = 11
}

TEST(PointTest, EuclideanDistance) {
    Point p1(0, 0);
    Point p2(3, 4);
    
    double distance = Point::euclideanDistance(p1, p2);
    EXPECT_DOUBLE_EQ(5.0, distance); // sqrt(3² + 4²) = sqrt(9 + 16) = sqrt(25) = 5
}

TEST(PointTest, EuclideanDistanceFloatingPoint) {
    Point p1(1, 1);
    Point p2(4, 5);
    
    double distance = Point::euclideanDistance(p1, p2);
    EXPECT_DOUBLE_EQ(5.0, distance); // sqrt((4-1)² + (5-1)²) = sqrt(9 + 16) = sqrt(25) = 5
}

TEST(PointTest, EdgeCases) {
    // Testing with large values
    Point p1(1000000, 2000000);
    Point p2(3000000, 5000000);
    
    EXPECT_EQ(5000000, Point::manhattanDistance(p1, p2));
    
    // Testing with zero difference
    Point p3(42, 42);
    Point p4(42, 42);
    
    EXPECT_EQ(0, Point::manhattanDistance(p3, p4));
    EXPECT_DOUBLE_EQ(0.0, Point::euclideanDistance(p3, p4));
}