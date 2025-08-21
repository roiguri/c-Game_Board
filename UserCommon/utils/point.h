#pragma once

#include <cmath>
#include <ostream>
#include <string>

namespace UserCommon_318835816_211314471 {

/**
 * @class Point
 * @brief Represents a 2D coordinate in the game world
 * 
 * Provides basic 2D point functionality with operations for comparing,
 * combining points, and calculating distances between points.
 */
class Point {
public:
    /**
     * @brief Default constructor
     * 
     * Initializes x and y to zero.
     */
    Point();

    /**
     * @brief Constructs a point with specific coordinates
     * 
     * @param x The x-coordinate
     * @param y The y-coordinate
     */
    Point(int x, int y);
    
    // Getters
    int getX() const { return m_x; }
    int getY() const { return m_y; }
    
    // Operators
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
    Point operator+(const Point& other) const;
    Point operator-(const Point& other) const;
    bool operator<(const Point& other) const;

    /**
     * @brief Converts the point to a string representation
     * 
     * @return String in format "(x,y)"
     */
    std::string toString() const;

    /**
     * @brief Calculates the Euclidean distance between two points
     * 
     * @param a First point
     * @param b Second point
     * @return The distance between the two points
     */
    static double euclideanDistance(const Point& a, const Point& b);

    friend std::ostream& operator<<(std::ostream& os, const Point& pt);
private: 
  int m_x;
  int m_y;
};

} // namespace UserCommon_318835816_211314471