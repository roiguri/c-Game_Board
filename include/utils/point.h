#pragma once

#include <string>
#include <cmath>
#include <ostream>

/**
 * @class Point
 * @brief Represents a 2D coordinate in the game world
 * 
 * Provides basic 2D point functionality with operations for comparing,
 * combining points, and calculating distances between points.
 */
class Point {
public:
    Point();
    Point(int x, int y);
    
    int x;
    int y;
    
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
    Point operator+(const Point& other) const;
    Point operator-(const Point& other) const;
    bool operator<(const Point& other) const; // In order to use points as keys.
    std::string toString() const;
    static int manhattanDistance(const Point& a, const Point& b);
    static double euclideanDistance(const Point& a, const Point& b);

    friend std::ostream& operator<<(std::ostream& os, const Point& pt);
};