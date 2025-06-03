#pragma once

#include "PlayerFactory.h"

/**
 * @class BasicPlayerFactory
 * @brief Factory for creating BasicPlayer instances
 * 
 * Implements the PlayerFactory interface to create BasicPlayer objects
 * for use in the tank battle game.
 */
class BasicPlayerFactory : public PlayerFactory {
public:
    /**
     * @brief Constructor
     */
    BasicPlayerFactory() = default;
    
    /**
     * @brief Destructor
     */
    ~BasicPlayerFactory() override = default;
    
    /**
     * @brief Creates a new BasicPlayer instance
     * 
     * @param playerIndex The ID of the player (1 or 2)
     * @param x Width of the game board
     * @param y Height of the game board
     * @param maxSteps Maximum number of steps for the game
     * @param numShells Number of shells per tank at game start
     * @return A unique pointer to the created BasicPlayer
     */
    std::unique_ptr<Player> create(
        int playerIndex, 
        size_t x, size_t y,  
        size_t maxSteps, 
        size_t numShells) const override;
};