#pragma once

#include "Player.h"
#include "BattleInfo.h"
#include <memory>

/**
* @class BasicPlayer
* @brief Basic implementation of player with minimal coordination strategy
* 
* Provides tanks with battle information and implements simple
* coordination through feedback messages.
*/
class BasicPlayer : public Player {
public:
   /**
    * @brief Constructs a new BasicPlayer object
    * 
    * @param player_index The ID of the player (1 or 2)
    * @param x Width of the game board
    * @param y Height of the game board
    * @param max_steps Maximum number of steps for the game
    * @param num_shells Number of shells per tank at game start
    */
   BasicPlayer(int player_index, size_t x, size_t y, 
               size_t max_steps, size_t num_shells);
   
   /**
    * @brief Destructor
    */
   ~BasicPlayer() override;
   
   /**
    * @brief Updates a tank with current battle information
    * 
    * Creates a specialized BattleInfo object and passes it to the
    * tank algorithm's updateBattleInfo method. The algorithm can
    * then downcast it to the specific type.
    * 
    * @param tank The tank algorithm requesting information
    * @param satellite_view Current view of the game board
    */
   void updateTankWithBattleInfo(TankAlgorithm& tank,
                                SatelliteView& satellite_view) override;
   
private:
   /** Player's ID (1 or 2) */
   int m_player_index;
   
   /** Board dimensions */
   size_t m_board_width;
   size_t m_board_height;
   
   /** Game constraints */
   size_t m_max_steps;
   size_t m_num_shells;
   
   /** Reusable battle info object */
   std::unique_ptr<BattleInfo> m_battle_info;
   
   /**
    * @brief Creates and populates the battle info from satellite view
    * 
    * Queries the satellite view to populate a specialized BattleInfo
    * object with current game state.
    * 
    * @param satellite_view The satellite view to query
    */
   void populateBattleInfo(SatelliteView& satellite_view);
   
   /**
    * @brief Processes feedback from a tank
    * 
    * Handles any feedback messages provided by tank algorithms
    * after processing battle info.
    * 
    * @param tank_id ID of the tank providing feedback
    */
   void processTankFeedback(int tank_id);
};