#ifndef DIRECTION_H
#define DIRECTION_H

#include <string>
#include "point.h"

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

#endif // DIRECTION_H