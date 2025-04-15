#pragma once

#include "utils/point.h"
#include "utils/direction.h"

/**
 * Represents a shell (artillery) fired by a tank.
 * 
 * Shells move at a pace of 2 units per game step in the direction
 * they were fired. They can hit walls, other shells, or tanks.
 */
class Shell {
public:
    /**
     * Creates a new shell fired by a player.
     * 
     * @param playerId The ID of the player who fired the shell
     * @param position The initial position of the shell
     * @param direction The direction the shell is moving
     */
    Shell(int playerId, const Point& position, Direction direction);
    
    /**
     * Destructor.
     */
    ~Shell();
    
private:
    int m_playerId;
    Point m_position;
    Direction m_direction;
    bool m_destroyed;
};