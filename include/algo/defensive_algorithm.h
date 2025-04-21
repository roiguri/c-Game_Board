#pragma once

#include "algo/algorithm.h"

/**
 * @brief Algorithm that focuses on defensive gameplay
 * 
 * Prioritizes survival by avoiding dangers and only shoots when it's safe to do so.
 */
class DefensiveAlgorithm : public Algorithm {
public:
    /**
     * @brief Constructor
     */
    DefensiveAlgorithm();
    
    /**
     * @brief Destructor
     */
    ~DefensiveAlgorithm() override;
    
    /**
     * @brief Determines the next action based on defensive strategy
     * 
     * Prioritizes:
     * 1. Avoiding danger from shells
     * 2. Shooting when opportunity arises
     * 3. Doing nothing when safe
     * 
     * @param gameBoard The current game board state
     * @param myTank The tank controlled by this algorithm
     * @param enemyTank The opponent's tank
     * @param shells All active shells on the board
     * @return The action to perform
     */
    Action getNextAction(
        const GameBoard& gameBoard,
        const Tank& myTank,
        const Tank& enemyTank,
        const std::vector<Shell>& shells
    ) override;
private:
    /**
     * @brief Find an action to move out of the enemy's line of sight
     * 
     * Checks adjacent positions and chooses one that:
     * 1. Is not in the enemy's line of sight
     * 2. Is not in danger from shells
     * 3. Requires minimal rotation to reach
     * 
     * @param gameBoard The current game board state
     * @param myTank The tank controlled by this algorithm
     * @param enemyTank The opponent's tank
     * @param shells All active shells on the board
     * @return Action to move out of line of sight, or Action::None if not possible
     */
    Action moveOutOfLineOfSight(
      const GameBoard& gameBoard,
      const Tank& myTank,
      const Tank& enemyTank,
      const std::vector<Shell>& shells
  ) const;
};