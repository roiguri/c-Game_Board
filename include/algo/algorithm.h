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
    bool isInDanger(const GameBoard& gameBoard, const Tank& tank, 
                   const std::vector<Shell>& shells, int lookAheadSteps = 3) const;
    
    bool isInDanger(const GameBoard& gameBoard, const Point& position, 
                   const std::vector<Shell>& shells, int lookAheadSteps = 3) const;
    
    struct SafeMoveOption {
        Point position;
        Action action;
        int stepCost;
        
        bool operator<(const SafeMoveOption& other) const {
            return stepCost < other.stepCost;
        }
    };
    
    Action findOptimalSafeMove(
      const GameBoard& gameBoard, 
      const Tank& tank,
      const Tank& enemyTank,
      const std::vector<Shell>& shells,
      bool avoidEnemySight = false  
    ) const;
    
    std::vector<SafeMoveOption> getSafeMoveOptions(
      const GameBoard& gameBoard, 
      const Tank& tank,
      const Tank& enemyTank,
      const std::vector<Shell>& shells, 
      bool avoidEnemySight = false
    ) const;

    bool isExposedToEnemy(
      const GameBoard& gameBoard, 
      const Point& position,
      const Tank& enemyTank) const;
    
    int calculateMoveCost(const Tank& tank, const Point& targetPos, Direction targetDir) const;
    
    // Targeting utilities
    bool canHitTarget(const GameBoard& gameBoard, const Tank& shooter, 
                     const Point& targetPos) const;
    
    std::optional<Direction> getLineOfSightDirection(const GameBoard& gameBoard, 
                                                   const Point& from, 
                                                   const Point& to) const;
    
    bool checkLineOfSightInDirection(const GameBoard& gameBoard, const Point& from, 
                                   const Point& to, Direction direction) const;
    
    Action getRotationToDirection(Direction current, Direction target) const;
    
    Action evaluateOffensiveOptions(const GameBoard& gameBoard, const Tank& myTank, 
                                   const Tank& enemyTank) const;

    // Expose methods for testing
    friend class AlgorithmTest;
};