#pragma once

#include "objects/game_object.h"

/**
 * @class Tank
 * @brief Represents a player-controlled tank in the game
 * 
 * Tank objects handle the tank related actions including forward/backward 
 * movement, rotation, shooting, and managing ammunition.
 */
class Tank : public GameObject {
public:
    /** Maximum number of shells available at the start of the game */
    static int INITIAL_SHELLS;
    static void setInitialShells(int shells);

    /** Number of game steps a tank must wait between consecutive shots */
    static constexpr int SHOOT_COOLDOWN = 4;

    /** Number of game steps a tank must wait before executing a backward move */
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
     * 
     * A tank can shoot if:
     * - It is not destroyed
     * - It has remaining shells
     * - It is not in cooldown
     * 
     * @return true if the tank can shoot, false otherwise
     */
    bool canShoot() const;

    /**
     * @brief Shoot a shell in the direction the tank is facing
     * @return true if the shot was successful, false if the tank can't shoot
     */
    bool shoot();

    /**
     * @brief Decrease cooldown timer
     * 
     * Decreases the shoot cooldown timer if active. If the cooldown reaches
     *  zero, the tank can shoot again.
     */
    void updateCooldowns();


    /**
     * @brief Process a backward movement request
     * 
     * Per requirements:
     * - Takes 2 steps before executing the move on the 3rd step
     * - Can be canceled by a forward movement
     * - Subsequent backward moves only take 1 step (if continuous)
     * 
     * @param newPosition The new position after backward movement
     * @return true if backward movement was initiated or continued, 
     *  false otherwise
     */
    bool requestMoveBackward(const Point& newPosition);

    /**
     * @brief Check if the tank is currently in backward movement mode
     * @return true if the tank is in a backward movement sequence, 
     *  false otherwise
     */
    bool isMovingBackward() const;
    
    /**
     * @brief Get the current backward movement counter
     * 
     * The counter indicates how many steps have passed since the backward
     *  movement was initiated (0 if not in backward movement).
     * 
     * @return The current backward counter value
     */
    int getBackwardCounter() const;
    
    /**
     * @brief Check if the tank is in continuous backward movement
     * @return true if the tank is moving continuously backward,
     *  false otherwise
     */
    bool isContinuousBackward() const;

    /**
     * @brief Reset the backward movement state
     * 
     * Cancels any pending backward movement and resets continuous flag.
     */
    void resetBackwardMovement();


    /**
     * @brief Move the tank forward to the given position
     * 
     * MoveForward action cancels any pending backward movement if active.
     * 
     * @param newPosition The new position after movement
     * @return true if the move was successful, false otherwise
     */
    bool moveForward(const Point& newPosition);

    /**
     * @brief Rotate the tank left
     * 
     * Cannot rotate while in backward movement mode.
     * 
     * @param quarterTurn If true, rotate by 90 degrees (1/4 turn), 
     *  otherwise 45 degrees (1/8 turn)
     * @return true if the rotation was successful, false otherwise
     */
    bool rotateLeft(bool quarterTurn);
    
    /**
     * @brief Rotate the tank right
     * 
     * Cannot rotate while in backward movement mode.
     * 
     * @param quarterTurn If true, rotate by 90 degrees (1/4 turn), 
     *  otherwise 45 degrees (1/8 turn)
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

    /**
     * @brief Update tank state when no action is taken
     * 
     * Update backward movement state and previous position.
     * 
     * @return 
     */
    void doNothing();

private:
    // Execute the actual backward movement to the stored position
    void moveBackward();
    
    // Check if the tank is in backward movement and update the backward
    // movement state
    bool isInBackwardMovement();

    int m_remainingShells;
    int m_shootCooldown;
    int m_backwardCounter;
    bool m_continuousBackward;
    Point m_backwardPosition;
};