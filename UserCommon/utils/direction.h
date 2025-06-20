#pragma once

#include <string>
#include "utils/point.h"
#include <ostream>
#include <optional>

/**
 * @class Direction
 * @brief Represents the 8 possible directions for movement and orientation
 *
 * Defines all cardinal and intercardinal directions used for game movement,
 * rotation, and projectile travel in the game.
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
 * @brief Array of all possible directions.
 * 
 * The array contains all the directions in a clockwise order, starting from Up.
 */
static constexpr Direction ALL_DIRECTIONS[] = {
  Direction::Up, Direction::UpRight, Direction::Right, Direction::DownRight,
  Direction::Down, Direction::DownLeft, Direction::Left, Direction::UpLeft
};

/**
 * @brief Converts a Direction enum value to its string representation.
 * 
 * @param direction The direction to convert
 * @return String representation of the direction (e.g., "Up", "UpRight", etc.)
 */
std::string directionToString(Direction direction);

/**
 * @brief Rotates a direction counter-clockwise.
 * 
 * @param direction The current direction
 * @param quarterTurn If true, rotates by 90 degrees (1/4 turn), otherwise 45
 *  degrees (1/8 turn)
 * @return The new direction after rotation
 */
Direction rotateLeft(Direction direction, bool quarterTurn = false);

/**
 * @brief Rotates a direction clockwise.
 * 
 * @param direction The current direction
 * @param quarterTurn If true, rotates by 90 degrees (1/4 turn), otherwise 45
 *  degrees (1/8 turn)
 * @return The new direction after rotation
 */
Direction rotateRight(Direction direction, bool quarterTurn = false);

/**
 * @brief Gets the delta movement Point for a given direction.
 * 
 * @param direction The direction of movement
 * @param magnitude The magnitude of movement (default 1)
 * @return A Point representing the delta movement in x,y coordinates
 */
Point getDirectionDelta(Direction direction, int magnitude = 1);

/**
 * @brief Calculates the direction needed to move from one point to another
 *   adjacent point.
 * 
 * @pre Points must be directly adjacent (including diagonals)
 * 
 * @param from The starting point
 * @param to The target point
 * @return Direction to move from 'from' to 'to', or nullopt if points are
 *  not adjacent
 */
std::optional<Direction> getDirectionToPoint(
  const Point& from, const Point& to);


/**
 * @brief Overloads the << operator to print the Direction enum value.
 * 
 * @param os The output stream
 * @param dir The direction to print
 * @return The output stream
 */
std::ostream& operator<<(std::ostream& os, Direction dir);