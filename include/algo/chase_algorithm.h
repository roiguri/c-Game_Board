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
     * @brief Struct to represent a node in the BFS path search
     */
    struct PathNode {
      Point position;
      Direction tankDirection;
      Action initialAction;    // First action taken from the start node
      Action arrivalAction;    // Action that led to this node
      int cost;                // Total path cost so far
      bool continuousBackward; // Whether in continuous backward movement
      int backwardCounter;     // Counter for backward movement delay
      
      // For path reconstruction
      PathNode* parent;
      
      // Constructor
      PathNode(Point pos, Direction dir, Action initAction, Action arrAction, 
              int c, bool contBack, int backCount, PathNode* p = nullptr)
          : position(pos), tankDirection(dir), initialAction(initAction), 
            arrivalAction(arrAction), cost(c), continuousBackward(contBack),
            backwardCounter(backCount), parent(p) {}
    };
    
    /**
     * @brief Finds a path to the enemy tank using BFS
     * 
     * @param gameBoard The current game board state
     * @param myTank The tank controlled by this algorithm
     * @param enemyTank The opponent's tank
     * @return The first action to take to follow the path
     */
    Action findPathToEnemy(
      const GameBoard& gameBoard,
      const Tank& myTank,
      const Tank& enemyTank
    );

    /**
     * @brief Runs BFS algorithm to find path to target
     * 
     * @param gameBoard The current game board state
     * @param startNode The starting node for the search
     * @param targetPosition The position we're trying to reach
     * @param maxNodes Maximum number of nodes to explore (prevents infinite loops)
     * @return Pointer to the end node if path found, nullptr otherwise
     */
    PathNode* runBFS(
      const GameBoard& gameBoard,
      PathNode* startNode,
      const Point& targetPosition,
      int maxNodes
    );

    /**
     * @brief Generates all possible next moves from current position
     * 
     * @param gameBoard The current game board state
     * @param currentNode The current node to generate moves from
     * @return Vector of possible next nodes
     */
    std::vector<PathNode*> getNextMoves(
      const GameBoard& gameBoard,
      const PathNode* currentNode
    );

    /**
     * @brief Compares two states to check if they are equivalent
     * 
     * @param pos1 First state tuple (position, direction, continuous backward flag, backward counter)
     * @param pos2 Second state tuple
     * @return true if states are equivalent, false otherwise
     */
    bool isSameState(
      const std::tuple<Point, Direction, bool, int>& pos1,
      const std::tuple<Point, Direction, bool, int>& pos2
    ) const;

    /**
     * @brief Extracts the first action from a path
     * 
     * @param endNode The final node in the found path
     * @return The first action to take from the start position
     */
    Action extractFirstAction(const PathNode* endNode) const;

    /**
     * @brief Cleans up allocated PathNode pointers
     * 
     * @param nodes Vector of nodes to deallocate
     */
    void cleanupNodes(const std::vector<PathNode*>& nodes);
};