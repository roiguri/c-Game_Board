#pragma once

#include "utils/action.h"
#include "game_board.h"
#include "tank.h"
#include "shell.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @brief Abstract base class for tank battle algorithms
 * 
 * Provides the interface and common utilities for implementing different
 * tank strategy algorithms.
 */
class Algorithm {
public:
    /**
     * @brief Constructor
     */
    Algorithm();
    
    /**
     * @brief Virtual destructor
     */
    virtual ~Algorithm();
    
    /**
     * @brief Determines the next action for a tank based on the current game state
     * 
     * @param gameBoard The current game board state
     * @param myTank The tank controlled by this algorithm
     * @param enemyTank The opponent's tank
     * @param shells All active shells on the board
     * @return The action to perform
     */
    virtual Action getNextAction(
        const GameBoard& gameBoard,
        const Tank& myTank,
        const Tank& enemyTank,
        const std::vector<Shell>& shells
    ) = 0;
    
    /**
     * @brief Factory method to create algorithm instances
     * 
     * @param type String identifier for the algorithm type
     * @return Pointer to the new algorithm instance
     */
    static Algorithm* createAlgorithm(const std::string& type);

private:
    // TODO: private members will be added in future commits
};