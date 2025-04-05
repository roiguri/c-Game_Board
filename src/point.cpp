#include "point.h"
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
std::string Point::toString() const {
  return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
}
// TODO: check if the method is necessary
int Point::manhattanDistance(const Point& a, const Point& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}
double Point::euclideanDistance(const Point& a, const Point& b) {
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}