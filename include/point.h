#ifndef POINT_H
#define POINT_H
#include <string>
#include <cmath>

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
      std::string toString() const;
      
      static int manhattanDistance(const Point& a, const Point& b);
      static double euclideanDistance(const Point& a, const Point& b);
  };

#endif // POINT_H