#pragma once

#include "objects/game_object.h"

/**
 * @class Tank
 * @brief Represents a tank in the game with its properties and actions
 */
class Tank : public GameObject {
public:
    static constexpr int INITIAL_SHELLS = 16;
    static constexpr int SHOOT_COOLDOWN = 4;
    static constexpr int BACKWARD_DELAY = 2;

    /**
     * @brief Constructs a new Tank object
     * 
     * @param playerId The ID of the player owning this tank
     * @param position The initial position of the tank
     * @param direction The initial direction of the tank
     */
    Tank(int playerId, const Point& position, Direction direction);


    /**
     * @brief Get the number of remaining shells
     * @return The shell count
     */
    int getRemainingShells() const;

    /**
     * @brief Decrement the number of shells
     */
    void decrementShells();


    /**
     * @brief Check if the tank can shoot
     * @return true if the tank can shoot, false otherwise
     */
    bool canShoot() const;

    /**
     * @brief Shoot a shell in the direction the tank is facing
     * @return true if the shot was successful, false if the tank can't shoot
     */
    bool shoot();

    /**
     * @brief Update cooldown timers after each game step
     */
    void updateCooldowns();


    /**
     * @brief Process a backward movement request
     * 
     * Per requirements:
     * - Takes 2 steps before moving
     * - Can be canceled by a forward movement
     * - Subsequent backward moves only take 1 step (if continuous)
     * 
     * @param newPosition The new position after backward movement
     * @return true if backward movement was initiated or continued, false otherwise
     */
    bool requestMoveBackward(const Point& newPosition);

    /**
     * @brief Check if the tank is currently in backward movement mode
     * @return true if the tank is in a backward movement sequence, false otherwise
     */
    bool isMovingBackward() const;
    
    /**
     * @brief Get the current backward movement counter
     * @return The current backward counter value
     */
    int getBackwardCounter() const;
    
    /**
     * @brief Check if the tank is in continuous backward movement
     * @return true if the tank is moving continuously backward, false otherwise
     */
    bool isContinuousBackward() const;

    /**
     * @brief Reset the backward movement state
     */
    void resetBackwardMovement();


    /**
     * @brief Move the tank forward to the given position
     * @param newPosition The new position after movement
     * @return true if the move was successful, false otherwise
     */
    bool moveForward(const Point& newPosition);

    /**
     * @brief Rotate the tank left
     * @param quarterTurn If true, rotate by 90 degrees (1/4 turn), otherwise 45 degrees (1/8 turn)
     * @return true if the rotation was successful, false otherwise
     */
    bool rotateLeft(bool quarterTurn);
    
    /**
     * @brief Rotate the tank right
     * @param quarterTurn If true, rotate by 90 degrees (1/4 turn), otherwise 45 degrees (1/8 turn)
     * @return true if the rotation was successful, false otherwise
     */
    bool rotateRight(bool quarterTurn);
    
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
    void moveBackward();
    bool isInBackwardMovement();

    int m_remainingShells;
    int m_shootCooldown;
    int m_backwardCounter;
    bool m_continuousBackward;
    Point m_backwardPosition;
};