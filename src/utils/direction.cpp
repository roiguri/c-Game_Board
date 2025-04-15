#include "utils/direction.h"

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

Direction rotateLeft(Direction direction, bool quarterTurn) {
    int step = quarterTurn ? 2 : 1; // 2 steps for 1/4 turn, 1 step for 1/8 turn
    int directionIndex = static_cast<int>(direction);
    
    // Rotate left by adding steps (modulo 8 to wrap around)
    directionIndex = (directionIndex - step + 8) % 8;  // (add 8 and modulo 8 to handle negative values)
    
    return static_cast<Direction>(directionIndex);
}

Direction rotateRight(Direction direction, bool quarterTurn) {
    int step = quarterTurn ? 2 : 1; // 2 steps for 1/4 turn, 1 step for 1/8 turn
    int directionIndex = static_cast<int>(direction);
    
    // Rotate right by subtracting steps
    directionIndex = (directionIndex + step) % 8;
    
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