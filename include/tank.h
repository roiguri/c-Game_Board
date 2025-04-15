#pragma once

#include "point.h"
#include "direction.h"

/**
 * @class Tank
 * @brief Represents a tank in the game with its properties and actions
 */
class Tank {
public:
    /**
     * @brief Constructs a new Tank object
     * 
     * @param playerId The ID of the player owning this tank
     * @param position The initial position of the tank
     * @param direction The initial direction of the tank
     */
    Tank(int playerId, const Point& position, Direction direction);
    
    /**
     * @brief Destroys the Tank object
     */
    ~Tank();
    
    /**
     * @brief Get the player ID of the tank
     * @return The player ID
     */
    int getPlayerId() const;
    
    /**
     * @brief Get the current position of the tank
     * @return The position as a Point
     */
    Point getPosition() const;
    
    /**
     * @brief Get the current direction of the tank
     * @return The direction
     */
    Direction getDirection() const;
    
    /**
     * @brief Set the position of the tank
     * @param position The new position
     */
    void setPosition(const Point& position);
    
    /**
     * @brief Set the direction of the tank
     * @param direction The new direction
     */
    void setDirection(Direction direction);

private:
    int m_playerId;
    Point m_position;
    Direction m_direction;
};