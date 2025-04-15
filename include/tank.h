#pragma once

#include "utils/point.h"
#include "utils/direction.h"

/**
 * @class Tank
 * @brief Represents a tank in the game with its properties and actions
 */
class Tank {
public:
    static constexpr int INITIAL_SHELLS = 16;

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
     * @brief Get the number of remaining shells
     * @return The shell count
     */
    int getRemainingShells() const;
    
    /**
     * @brief Check if the tank is destroyed
     * @return true if destroyed, false otherwise
     */
    bool isDestroyed() const;
    
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

    /**
     * @brief Decrement the number of shells
     */
    void decrementShells();
    
    /**
     * @brief Mark the tank as destroyed
     */
    void destroy();

    // Tank Actions
    
    /**
     * @brief Move the tank forward to the given position
     * @param newPosition The new position after movement
     * @return true if the move was successful, false otherwise
     */
    bool moveForward(const Point& newPosition);
    
    // Utility methods
    /**
     * @brief Calculate the next position for forward movement
     * @return The next position if the tank would move forward
     */
    Point getNextForwardPosition() const;
    
    /**
     * @brief Calculate the next position for backward movement
     * @return The next position if the tank would move backward
     */
    Point getNextBackwardPosition() const;

private:
    int m_playerId;
    Point m_position;
    Direction m_direction;
    int m_remainingShells;
    bool m_destroyed;
};