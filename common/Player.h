#pragma once

#include "TankAlgorithm.h"
#include "SatelliteView.h"

/**
 * @class Player
 * @brief Base class for player coordination in the tank battle game
 * 
 * Coordinates tank algorithms and provides game information to them.
 * Each player object manages one or more tanks for a specific player
 * and facilitates information exchange through battle info objects.
 */
class Player {  
public:
    /**
     * @class Player
     * @brief Constructs a new Player object
     * 
     * @param player_index The ID of the player (1 or 2)
     * @param x Width of the game board
     * @param y Height of the game board
     * @param max_steps Maximum number of steps for the game
     * @param num_shells Number of shells per tank at game start
     */  
    Player(int player_index, size_t x, size_t y, size_t max_steps,
                                                 size_t num_shells) {}
                                                 
    /**
     * @brief Virtual destructor
     */
    virtual ~Player() {}  

    /**
     * @brief Updates a tank with current battle information
     * 
     * Called when a tank algorithm requests battle info.
     * The player can use this method to coordinate tank movements
     * by providing specialized battle info.
     * 
     * @param tank The tank algorithm requesting information
     * @param satellite_view Current view of the game board
     */    
    virtual void updateTankWithBattleInfo(TankAlgorithm& tank,
                                          SatelliteView& satellite_view) = 0;  
};