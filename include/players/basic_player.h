#pragma once

#include "Player.h"
#include "BattleInfo.h"
#include "players/battle_info_impl.h"
#include <memory>

/**
* @class BasicPlayer
* @brief Basic implementation of player with minimal coordination strategy
* 
* Provides tanks with battle information.
*/
class BasicPlayer : public Player {
public:
   /**
    * @brief Constructs a new BasicPlayer object
    * 
    * @param playerIndex The ID of the player (1 or 2)
    * @param x Width of the game board
    * @param y Height of the game board
    * @param maxSteps Maximum number of steps for the game
    * @param numShells Number of shells per tank at game start
    */
   BasicPlayer(int playerIndex, size_t x, size_t y, 
               size_t maxSteps, size_t numShells);
   
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
    * @param satelliteView Current view of the game board
    */
   void updateTankWithBattleInfo(TankAlgorithm& tank,
                                SatelliteView& satelliteView) override;
   
protected:
   /** Player's ID (1 or 2) */
   int m_playerIndex;
   
   /** Board dimensions */
   size_t m_boardWidth;
   size_t m_boardHeight;
   
   /** Game constraints */
   size_t m_maxSteps;
   size_t m_numShells;
   
   /** Reusable battle info object */
   BattleInfoImpl m_battleInfo;
   
   /**
    * @brief Creates and populates the battle info from satellite view
    * 
    * Queries the satellite view to populate a specialized BattleInfo
    * object with current game state.
    * 
    * @param satelliteView The satellite view to query
    */
   virtual void populateBattleInfo(SatelliteView& satelliteView);
   
   friend class BasicPlayerTest;
};