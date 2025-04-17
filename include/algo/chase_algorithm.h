#pragma once

#include "algo/algorithm.h"
#include <queue>
#include <unordered_set>

class ChaseAlgorithmTest;

/**
 * @brief Algorithm that actively chases the enemy tank
 * 
 * Uses BFS pathfinding to pursue the enemy while avoiding danger
 */
class ChaseAlgorithm : public Algorithm {
public:
    /**
     * @brief Constructor
     */
    ChaseAlgorithm();
    
    /**
     * @brief Destructor
     */
    ~ChaseAlgorithm() override;
    
    /**
     * @brief Determines the next action for pursuing the enemy tank
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
    // Expose the test class to allow access to private members
    // for unit testing
    friend class ChaseAlgorithmTest;

    /**
     * @brief Checks if the tank can shoot the enemy
     * 
     * @param gameBoard The game board
     * @param myTank The tank controlled by this algorithm
     * @param enemyTank The opponent's tank
     * @return true if the enemy can be shot, false otherwise
     */
    bool canShootEnemy(
        const GameBoard& gameBoard,
        const Tank& myTank,
        const Tank& enemyTank
    ) const;
};