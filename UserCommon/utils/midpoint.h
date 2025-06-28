// midpoint.h
#pragma once

#include "point.h"

namespace UserCommon_098765432_123456789 {

/**
 * @class MidPoint
 * @brief Represents a point with potential half-coordinate values
 * 
 * Extends the Point class to represent midpoints between grid positions
 * without using floating-point arithmetic, by tracking if x and/or y 
 * coordinates have a .5 component.
 */
class MidPoint : public Point {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a midpoint at (0,0) with no half components
     */
    MidPoint();
    
    /**
     * @brief Constructs a midpoint with specified coordinates
     * 
     * @param x The x-coordinate
     * @param y The y-coordinate
     * @param halfX Whether the x-coordinate has a .5 component
     * @param halfY Whether the y-coordinate has a .5 component
     */
    MidPoint(int x, int y, bool halfX = false, bool halfY = false);
    
    /**
     * @brief Copy constructor from Point
     * 
     * @param point The point to copy (will have no half components)
     */
    explicit MidPoint(const Point& point);
    
    /**
     * @brief Check if the x-coordinate has a half component
     * 
     * @return true if x has a .5 component, false otherwise
     */
    bool isHalfX() const;
    
    /**
     * @brief Check if the y-coordinate has a half component
     * 
     * @return true if y has a .5 component, false otherwise
     */
    bool isHalfY() const;
    
    /**
     * @brief Calculate the midpoint between two points
     * 
     * @param a First point
     * @param b Second point
     * @return The midpoint between a and b
     */
    static MidPoint calculateMidpoint(const Point& a, const Point& b);

    /**
     * @brief Calculate the midpoint between two points considering board
     *  wrapping
     *
     * @param a First point
     * @param b Second point
     * @param boardWidth The width of the board for wrapping calculation
     * @param boardHeight The height of the board for wrapping calculation
     * @return The midpoint between a and b, considering wraparound
     */
    static MidPoint calculateMidpoint(
      const Point& a, 
      const Point& b, 
      int boardWidth, 
      int boardHeight
    );
    
    /**
     * @brief Check if the midpoints between two pairs of points are the same
     * 
     * @param a1 First point of first pair
     * @param a2 Second point of first pair
     * @param b1 First point of second pair
     * @param b2 Second point of second pair
     * @return true if the midpoints are the same, false otherwise
     */
    static bool midpointsMatch(const Point& a1, const Point& a2, 
                               const Point& b1, const Point& b2);

    /**
     * @brief Check if the midpoints between two pairs of points are the same
     *  considering board wrapping
     * 
     * @param a1 First point of first pair
     * @param a2 Second point of first pair
     * @param b1 First point of second pair
     * @param b2 Second point of second pair
     * @param boardWidth The width of the board for wrapping calculation
     * @param boardHeight The height of the board for wrapping calculation
     * @return true if the midpoints are the same, false otherwise
     */
    static bool midpointsMatch(
      const Point& a1, 
      const Point& a2, 
      const Point& b1, 
      const Point& b2,
      int boardWidth, 
      int boardHeight
    );
    
    /**
     * @brief Compares two midpoints for equality
     * 
     * @param other The midpoint to compare with
     * @return true if the midpoints are equal, false otherwise
     */
    bool operator==(const MidPoint& other) const;
    
    /**
     * @brief Compares two midpoints for inequality
     * 
     * @param other The midpoint to compare with
     * @return true if the midpoints are not equal, false otherwise
     */
    bool operator!=(const MidPoint& other) const;

    /**
     * @brief Compares two midpoints for less-than relation
     * 
     * @param other The midpoint to compare with
     * @return true if this midpoint is less than the other, false otherwise
     */
    bool operator<(const MidPoint& other) const;
    
    /**
     * @brief Converts to string representation
     * 
     * @return String in format "(x.5,y.5)" where .5 components are shown if present
     */
    std::string toString() const;

    friend std::ostream& operator<<(std::ostream& os, const MidPoint& mp);

private:
    bool m_halfX;
    bool m_halfY;
};

} // namespace UserCommon_098765432_123456789