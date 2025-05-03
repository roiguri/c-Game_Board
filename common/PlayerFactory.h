#pragma once

#include "Player.h"
#include <memory>

/**
 * @class PlayerFactory
 * @brief Factory interface for creating Player objects.
 * 
 * Provides an abstract factory method to create Player instances.
 */
class PlayerFactory {  
public:
    /**
     * @brief Virtual destructor
     */  
    virtual ~PlayerFactory() {}  
    
    /**
     * @brief Creates a new Player instance
     * 
     * Factory method that instantiates and returns a Player object
     * with the specified parameters.
     * 
     * @param player_index The ID of the player (1 or 2)
     * @param x Width of the game board
     * @param y Height of the game board
     * @param max_steps Maximum number of steps for the game
     * @param num_shells Number of shells per tank at game start
     * @return std::unique_ptr<Player> A unique pointer to the created Player object
     */
    virtual std::unique_ptr<Player> create(
        int player_index, 
        size_t x, size_t y,  
        size_t max_steps, 
        size_t num_shells) const = 0;  
};