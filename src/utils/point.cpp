#include "utils/point.h"
#include <cmath>

Point::Point() : x(0), y(0) {}
Point::Point(int x, int y) : x(x), y(y) {}
bool Point::operator==(const Point& other) const {
    return x == other.x && y == other.y;
}
bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}
Point Point::operator+(const Point& other) const {
    return Point(x + other.x, y + other.y);
}
Point Point::operator-(const Point& other) const {
    return Point(x - other.x, y - other.y);
}
bool Point::operator<(const Point& other) const {
  // First compare x-coordinates, then y-coordinates
  if (x != other.x) {
      return x < other.x;
  }
  return y < other.y;
}
std::string Point::toString() const {
  return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
}

std::ostream& operator<<(std::ostream& os, const Point& pt) {
    os << pt.toString();
    return os;
}