#pragma once

#include "utils/point.h"
#include "utils/direction.h"

/**
 * @class GameObject
 * @brief Base class for all game objects in the tank battle simulation
 * 
 * Provides common functionality for game objects like tanks and shells,
 * including position tracking, direction, player ownership, and destruction
 *  state.
 */
class GameObject {
public:
    /**
     * @brief Constructs a new GameObject
     * 
     * @param playerId The ID of the player who owns this object
     * @param position The initial position of the object
     * @param direction The initial direction of the object
     */
    GameObject(int playerId, const Point& position, Direction direction);
    
    /**
     * @brief Get the player ID of the game object
     * @return The player ID
     */
    int getPlayerId() const;
    
    /**
     * @brief Get the current position of the game object
     * @return The position as a Point
     */
    Point getPosition() const;
    
    /**
     * @brief Get the previous position of the game object
     * @return The previous position as a Point
     */
    Point getPreviousPosition() const;
    
    /**
     * @brief Get the current direction of the game object
     * @return The direction
     */
    Direction getDirection() const;
    
    /**
     * @brief Check if the game object is destroyed
     * @return true if destroyed, false otherwise
     */
    bool isDestroyed() const;
    
    /**
     * @brief Set the position of the game object
     * 
     * Updates the previous position before changing the current position.
     * 
     * @param position The new position
     */
    void setPosition(const Point& position);
    
    /**
     * @brief Set the direction of the game object
     * @param direction The new direction
     */
    void setDirection(Direction direction);
    
    /**
     * @brief Mark the game object as destroyed
     */
    void destroy();
    
    // TODO: consider removing or changing
    /**
     * @brief Set the previous position to the current position
     * 
     * This should be called before changing the current position
     * if not using setPosition() method
     */
    void updatePreviousPosition();

protected:
    int m_playerId;
    Point m_position;
    Point m_previousPosition;
    Direction m_direction;
    bool m_destroyed;
};