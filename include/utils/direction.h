#pragma once

#include <string>
#include "utils/point.h"
#include <ostream>

/**
 * Represents the 8 possible directions for tank movement and shooting.
 */
enum class Direction {
    Up,       
    UpRight,  
    Right,    
    DownRight,
    Down,     
    DownLeft, 
    Left,     
    UpLeft    
};

/**
 * Represents the 8 possible directions for tank movement and shooting.
 * This is a constant array of all possible directions.
 */
static const Direction ALL_DIRECTIONS[] = {
  Direction::Up, Direction::UpRight, Direction::Right, Direction::DownRight,
  Direction::Down, Direction::DownLeft, Direction::Left, Direction::UpLeft
};

/**
 * Converts a Direction enum value to its string representation.
 * 
 * @param direction The direction to convert
 * @return String representation of the direction (e.g., "Up", "UpRight", etc.)
 */
std::string directionToString(Direction direction);

/**
 * Rotates a direction counter-clockwise.
 * 
 * @param direction The current direction
 * @param quarterTurn If true, rotates by 90 degrees (1/4 turn), otherwise 45 degrees (1/8 turn)
 * @return The new direction after rotation
 */
Direction rotateLeft(Direction direction, bool quarterTurn = false);

/**
 * Rotates a direction clockwise.
 * 
 * @param direction The current direction
 * @param quarterTurn If true, rotates by 90 degrees (1/4 turn), otherwise 45 degrees (1/8 turn)
 * @return The new direction after rotation
 */
Direction rotateRight(Direction direction, bool quarterTurn = false);

/**
 * Gets the delta movement Point for a given direction.
 * 
 * @param direction The direction of movement
 * @param magnitude The magnitude of movement (default 1)
 * @return A Point representing the delta movement in x,y coordinates
 */
Point getDirectionDelta(Direction direction, int magnitude = 1);

/**
 * Overloads the << operator to print the Direction enum value.
 * 
 * @param os The output stream
 * @param dir The direction to print
 * @return The output stream
 */
std::ostream& operator<<(std::ostream& os, Direction dir);