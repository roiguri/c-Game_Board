#pragma once

#include <string>
#include <cmath>
#include <ostream>

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
};

// TODO: is this the right way?
std::ostream& operator<<(std::ostream& os, const Point& pt);