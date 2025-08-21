#include "direction.h"

namespace UserCommon_318835816_211314471 {

std::string directionToString(Direction direction) {
    switch (direction) {
        case Direction::Up:       return "Up";
        case Direction::UpRight:  return "UpRight";
        case Direction::Right:    return "Right";
        case Direction::DownRight:return "DownRight";
        case Direction::Down:     return "Down";
        case Direction::DownLeft: return "DownLeft";
        case Direction::Left:     return "Left";
        case Direction::UpLeft:   return "UpLeft";
        default:                  return "Unknown";
    }
}

Direction getRotateLeftDirection(Direction direction, bool quarterTurn) {
    int step = quarterTurn ? 2 : 1;
    // Cast to int to perform modular arithmetic on ordered enum values
    int directionIndex = static_cast<int>(direction);
    
    directionIndex = (directionIndex - step + 8) % 8;
    
    // Cast back to Direction enum after ensuring valid range with modulo
    return static_cast<Direction>(directionIndex);
}

Direction getRotateRightDirection(Direction direction, bool quarterTurn) {
    int step = quarterTurn ? 2 : 1;
    // Cast to int to perform modular arithmetic on ordered enum values
    int directionIndex = static_cast<int>(direction);
    
    directionIndex = (directionIndex + step) % 8;
    
    // Cast back to Direction enum after ensuring valid range with modulo
    return static_cast<Direction>(directionIndex);
}

Point getDirectionDelta(Direction direction, int magnitude) {
    switch (direction) {
        case Direction::Up:
            return Point(0, -magnitude);
        case Direction::UpRight:
            return Point(magnitude, -magnitude);
        case Direction::Right:
            return Point(magnitude, 0);
        case Direction::DownRight:
            return Point(magnitude, magnitude);
        case Direction::Down:
            return Point(0, magnitude);
        case Direction::DownLeft:
            return Point(-magnitude, magnitude);
        case Direction::Left:
            return Point(-magnitude, 0);
        case Direction::UpLeft:
            return Point(-magnitude, -magnitude);
        default:
            return Point(0, 0);
    }
}

std::optional<Direction> getDirectionToPoint(const Point& from, const Point& to) {
  if (from == to) {
      return std::nullopt;
  }

  for (const Direction& dir : ALL_DIRECTIONS) {
      Point expectedTo = from + getDirectionDelta(dir);
      if (expectedTo == to) {
          return dir;
      }
  }
  return std::nullopt;
}

std::ostream& operator<<(std::ostream& os, Direction dir) {
  os << directionToString(dir);
  return os;
}

} // namespace UserCommon_318835816_211314471