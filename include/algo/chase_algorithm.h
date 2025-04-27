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
    // Stores current path being followed
    std::vector<Point> m_currentPath;

    // Last known position of target for path recalculation
    Point m_lastTargetPosition; 

    // Recalculates path if target has moved significantly or tank deviated 
    //  from its route
    void updatePathToTarget(
      const GameBoard& gameBoard, 
      const Point& start, 
      const Point& target
    );
    
    // Determines next action to follow current path while avoiding danger
    Action followCurrentPath(
      const GameBoard& gameBoard, 
      const Tank& myTank, 
      const std::vector<Shell>& shells
    );
    
    // Finds shortest path between points using breadth-first search
    std::vector<Point> findPathBFS(
      const GameBoard& gameBoard, 
      const Point& start, 
      const Point& target
    ) const;
    
    // Expose the test class to allow access to private members
    // for unit testing
    friend class ChaseAlgorithmTest;
};                              