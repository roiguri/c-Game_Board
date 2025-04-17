#pragma once

#include "utils/action.h"
#include "utils/point.h"
#include "utils/direction.h"
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
protected:
    /**
     * @brief Checks for a direct, unobstructed line of sight along the 8 primary directions.
     *
     * Determines if there is a clear path between 'from' and 'to' strictly along
     * horizontal, vertical, or 45-degree diagonal lines by checking each direction outwards.
     * Considers board wrapping and obstacles (Walls).
     *
     * @param gameBoard The game board containing layout and obstacles.
     * @param from The starting point.
     * @param to The target point.
     * @return true if a clear line of sight exists along one of the 8 directions, false otherwise.
     */
    bool hasDirectLineOfSight(
      const GameBoard& gameBoard,
      const Point& from,
      const Point& to
  ) const;
};