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

    // State members
    std::vector<Point> m_currentPath;
    Point m_lastTargetPosition; 

    /**
     * @brief Gets valid neighboring points for pathfinding.
     *
     * Given a point, finds all adjacent points (8 directions, considering wrapping)
     * that are traversable (not Wall or Mine).
     *
     * @param current The point to find neighbors for.
     * @param gameBoard The game board state.
     * @return A vector of valid neighbor points.
     */
    std::vector<Point> getValidNeighbors(const Point& current, const GameBoard& gameBoard) const;

    /**
     * @brief Reconstructs the path found by BFS.
     *
     * Traces back from the end point to the start point using the predecessor map
     * generated during the BFS search.
     *
     * @param came_from A map where came_from[p] is the point from which BFS reached p.
     * @param start The starting point of the BFS search.
     * @param end The target end point found by BFS.
     * @return A vector of points representing the path from start (exclusive) to end (inclusive),
     * or an empty vector if the path cannot be reconstructed.
     */
    std::vector<Point> reconstructPath(
      const std::map<Point, Point>& came_from,
      const Point& start,
      const Point& end) const;

    // TODO: Consider optimize with minimum rotations/weights included
      /**
     * @brief Performs a Breadth-First Search to find a path between two points.
     *
     * Finds the shortest path (in terms of steps) from start to end, avoiding
     * walls and mines. Uses getValidNeighbors and reconstructPath helpers.
     *
     * @param start The starting point for the search.
     * @param end The target end point for the search.
     * @param gameBoard The game board state.
     * @return A vector of points representing the path from start (exclusive)
     * to end (inclusive), or an empty vector if no path is found.
     */
    std::vector<Point> calculatePathBFS(
      const Point& start,
      const Point& end,
      const GameBoard& gameBoard) const;

    /**
     * @brief Updates and validates the stored path to the enemy tank.
     *
     * @param gameBoard The current game board state.
     * @param myTank The tank controlled by this algorithm (used for start position).
     * @param enemyTank The opponent's tank (used for end position and checking movement).
     */
    void updateAndValidatePath(const GameBoard& gameBoard, const Tank& myTank, const Tank& enemyTank);

};