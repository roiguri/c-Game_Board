#pragma once

#include "game_object.h"

namespace UserCommon_318835816_211314471 {

/**
 * @class Shell
 * @brief Represents a shell (artillery) fired by a tank
 * 
 * Shells move at a pace of 2 units per game step in the direction
 * they were fired. They can hit walls, other shells, or tanks.
 */
class Shell : public GameObject {
  public:
      /**
       * @brief Creates a new shell fired by a player.
       * 
       * @param playerId The ID of the player who fired the shell
       * @param position The initial position of the shell
       * @param direction The direction the shell is moving
       */
      Shell(int playerId, const Point& position, Direction direction);
  
      /**
       * @brief Calculates the next position of the shell based on its
       *  direction.
       * 
       * This method will be called twice consequentally.
       * 
       * @return The next position of the shell
       */
      Point getNextPosition() const;
  };

} // namespace UserCommon_318835816_211314471