#include "utils/midpoint.h"
#include <cmath>

MidPoint::MidPoint() : Point(), m_halfX(false), m_halfY(false) {}

MidPoint::MidPoint(int x, int y, bool halfX, bool halfY) 
    : Point(x, y), m_halfX(halfX), m_halfY(halfY) {}

MidPoint::MidPoint(const Point& point) 
    : Point(point), m_halfX(false), m_halfY(false) {}

bool MidPoint::isHalfX() const {
    return m_halfX;
}

bool MidPoint::isHalfY() const {
    return m_halfY;
}

MidPoint MidPoint::calculateMidpoint(const Point& a, const Point& b) {
    int xDiff = b.getX() - a.getX();
    int yDiff = b.getY() - a.getY();
    
    // Check if the points are adjacent or one step apart
    bool xIsOdd = (std::abs(xDiff) == 1);
    bool yIsOdd = (std::abs(yDiff) == 1);
    
    // Calculate the midpoint coordinates
    int midX = (a.getX() + b.getX()) / 2;
    int midY = (a.getY() + b.getY()) / 2;
    
    return MidPoint(midX, midY, xIsOdd, yIsOdd);
}

bool MidPoint::midpointsMatch(const Point& a1, const Point& a2, 
                             const Point& b1, const Point& b2) {
    // Calculate both midpoints
    MidPoint midpoint1 = calculateMidpoint(a1, a2);
    MidPoint midpoint2 = calculateMidpoint(b1, b2);
    
    // Compare them
    return midpoint1 == midpoint2;
}

bool MidPoint::operator==(const MidPoint& other) const {
    return getX() == other.getX() && 
           getY() == other.getY() && 
           m_halfX == other.m_halfX && 
           m_halfY == other.m_halfY;
}

bool MidPoint::operator!=(const MidPoint& other) const {
    return !(*this == other);
}

std::string MidPoint::toString() const {
    std::string xStr = std::to_string(getX()) + (m_halfX ? ".5" : "");
    std::string yStr = std::to_string(getY()) + (m_halfY ? ".5" : "");
    return "(" + xStr + "," + yStr + ")";
}

std::ostream& operator<<(std::ostream& os, const MidPoint& mp) {
    os << mp.toString();
    return os;
}