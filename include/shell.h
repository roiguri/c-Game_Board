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

    // State accessors
    
    /**
     * Gets the ID of the player who fired this shell.
     * 
     * @return The player ID
     */
    int getPlayerId() const;
    
    /**
     * Gets the current position of the shell.
     * 
     * @return The current position
     */
    Point getPosition() const;
    
    /**
     * Gets the direction in which the shell is moving.
     * 
     * @return The movement direction
     */
    Direction getDirection() const;
    
    /**
     * Checks if the shell has been destroyed.
     * 
     * @return true if the shell has been destroyed, false otherwise
     */
    bool isDestroyed() const;

    // State mutators
    
    /**
     * Sets the position of the shell.
     * 
     * @param position The new position
     */
    void setPosition(const Point& position);
    
    /**
     * Marks the shell as destroyed, typically after a collision.
     */
    void destroy();

    /**
     * Calculates the next position of the shell based on its direction.
     * This method will be called twice consequentally.
     * 
     * @return The next position of the shell
     */
    Point getNextPosition() const;

    /**
     * @brief Get the previous position of the shell
     * 
     * @return The position before the last movement
     */
    Point getPreviousPosition() const;

    /**
     * @brief Set the previous position to the current position
     * This should be called before changing the current position
     */
    void updatePreviousPosition();
    
private:
    int m_playerId;
    Point m_position;
    Direction m_direction;
    bool m_destroyed;
    Point m_previousPosition;
};