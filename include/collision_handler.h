#pragma once
#include <vector>
#include "game_board.h"
#include "objects/tank.h"
#include "objects/shell.h"
#include "utils/midpoint.h"

/**
 * @class CollisionHandler
 * @brief Manages collision detection and resolution between game objects
 * 
 * Handles different types of collisions including path crossings, 
 * same position collisions, shell-wall interactions, and tank-mine
 * interactions.
 */
class CollisionHandler {
public:
    /**
     * @brief Resolves all types of collisions in the game
     * 
     * Detects and handles path crossings, position collisions, shell-wall
     * interactions, and tank-mine interactions.
     * 
     * @param tanks The tanks in the game that might be involved in collisions
     * @param shells The shells in the game that might be involved in collisions
     * @param board The game board containing walls and mines
     * @return true if any tank was destroyed during collision resolution, 
     *  false otherwise
     */
    bool resolveAllCollisions(
        std::vector<Tank>& tanks,
        std::vector<Shell>& shells,
        GameBoard& board
    );

private:
    // TODO: consider changing to a set
    std::vector<MidPoint> m_pathExplosions;   
    std::vector<Point> m_positionExplosions;
    int m_boardWidth;
    int m_boardHeight;


    void detectPathCollisions(
        std::vector<Tank>& tanks,
        std::vector<Shell>& shells
    );

    void detectPositionCollisions(
        std::vector<Tank>& tanks,
        std::vector<Shell>& shells
    );

    void checkShellWallCollisions(
        std::vector<Shell>& shells,
        GameBoard& board
    );

    bool checkTankMineCollisions(
        std::vector<Tank>& tanks,
        GameBoard& board
    );

    bool applyPathExplosions(std::vector<Tank>& tanks,
      std::vector<Shell>& shells);

    bool applyPositionExplosions(std::vector<Tank>& tanks,
              std::vector<Shell>& shells,
              GameBoard& board);

    void markPositionExplosionAt(const Point& pos);

    void markPathExplosionAt(const Point& from, const Point& to);

    bool pathCrossedMidpoint(
      const Point& prev, 
      const Point& curr, 
      const MidPoint& midpoint
    ) const;

    // For Test purposes
    friend class CollisionHandlerTest;
};