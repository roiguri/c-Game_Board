#pragma once

#include "utils/action.h"
#include "utils/point.h"
#include "utils/direction.h"
#include "game_board.h"
#include "objects/tank.h"
#include "objects/shell.h"
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
protected:
    // Is position safe from enemy shells, mines, and optionaly enemy sight
    bool isPositionSafe(
      const GameBoard& gameBoard, 
      const Point& position,
      const Tank& enemyTank,
      const std::vector<Shell>& shells,
      bool avoidEnemySight = false
    ) const;

    // Is position is safe from enemy shells, for the next `lookAheadSteps`
    bool isInDangerFromShells(const GameBoard& gameBoard, const Tank& tank, 
              const std::vector<Shell>& shells, int lookAheadSteps = 3) const;
    bool isInDangerFromShells(const GameBoard& gameBoard, const Point& position, 
              const std::vector<Shell>& shells, int lookAheadSteps = 3) const;
    
    // Represents a possible safe move with its cost
    struct SafeMoveOption {
        Point position;
        Action action;
        int stepCost;
        
        bool operator<(const SafeMoveOption& other) const {
            return stepCost < other.stepCost;
        }
    };
    
    // Finds the optimal safe move based on current game state
    Action findOptimalSafeMove(
      const GameBoard& gameBoard, 
      const Tank& tank,
      const Tank& enemyTank,
      const std::vector<Shell>& shells,
      bool avoidEnemySight = false  
    ) const;
    
    // Gets all possible safe moves with their costs
    std::vector<SafeMoveOption> getSafeMoveOptions(
      const GameBoard& gameBoard, 
      const Tank& tank,
      const Tank& enemyTank,
      const std::vector<Shell>& shells, 
      bool avoidEnemySight = false
    ) const;

    // Checks if a position is in enemy's line of sight
    bool isExposedToEnemy(
      const GameBoard& gameBoard, 
      const Point& position,
      const Tank& enemyTank) const;
    
    // Calculates number of steps required to rotate and move in target
    //  direction
    int calculateMoveCost(const Tank& tank, Direction targetDir) const;
    
    // Checks if tank can shoot and hit target at given position
    bool canHitTarget(const GameBoard& gameBoard, const Tank& shooter, 
                     const Point& targetPos) const;
    
    // Gets direction needed for line of sight between two positions,
    //  if possible
    std::optional<Direction> getLineOfSightDirection(
      const GameBoard& gameBoard, 
      const Point& from, 
      const Point& to
    ) const;
    
    // Checks if there's line of sight between points in specific direction
    bool checkLineOfSightInDirection(
      const GameBoard& gameBoard, 
      const Point& from, 
      const Point& to,
      Direction direction
    ) const;
    
    // Gets optimal rotation action to face target direction
    Action getRotationToDirection(Direction current, Direction target) const;
    
    // Evaluates possible offensive actions against enemy tank
    Action evaluateOffensiveOptions(const GameBoard& gameBoard, const Tank& myTank, 
                                   const Tank& enemyTank) const;

    // Expose methods for testing
    friend class AlgorithmTest;
};