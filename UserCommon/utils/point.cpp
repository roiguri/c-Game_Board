#include <cmath>

#include "point.h"

namespace UserCommon_098765432_123456789 {

Point::Point() : m_x(0), m_y(0) {}
Point::Point(int x, int y) : m_x(x), m_y(y) {}

bool Point::operator==(const Point& other) const {
    return m_x == other.m_x && m_y == other.m_y;
}

bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}

Point Point::operator+(const Point& other) const {
    return Point(m_x + other.m_x, m_y + other.m_y);
}

Point Point::operator-(const Point& other) const {
    return Point(m_x - other.m_x, m_y - other.m_y);
}

bool Point::operator<(const Point& other) const {
  if (m_x != other.m_x) {
      return m_x < other.m_x;
  }
  return m_y < other.m_y;
}

std::string Point::toString() const {
  return "(" + std::to_string(m_x) + "," + std::to_string(m_y) + ")";
}

std::ostream& operator<<(std::ostream& os, const Point& pt) {
    os << pt.toString();
    return os;
}

double Point::euclideanDistance(const Point& a, const Point& b) {
  return std::sqrt(std::pow(a.m_x - b.m_x, 2) + std::pow(a.m_y - b.m_y, 2));
}

} // namespace UserCommon_098765432_123456789
