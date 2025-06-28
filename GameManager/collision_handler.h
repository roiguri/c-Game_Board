#pragma once

#include <set>
#include <vector>

#include "game_board.h"
#include "objects/shell.h"
#include "objects/tank.h"
#include "utils/midpoint.h"

namespace GameManager_098765432_123456789 {

using namespace UserCommon_098765432_123456789;

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
     */
    void resolveAllCollisions(
        std::vector<Tank>& tanks,
        std::vector<Shell>& shells,
        GameBoard& board
    );

private:
    std::set<MidPoint> m_pathExplosions;   
    std::set<Point> m_positionExplosions;
    int m_boardWidth;
    int m_boardHeight;


    // Detects path collisions caused by object crossing paths
    void detectPathCollisions(
        std::vector<Tank>& tanks,
        std::vector<Shell>& shells
    );

    // Detects position collisions where two objects occupy the same space
    void detectPositionCollisions(
        std::vector<Tank>& tanks,
        std::vector<Shell>& shells
    );

    // Checks for shell-wall collisions and marks explosions
    void checkShellWallCollisions(
        std::vector<Shell>& shells,
        GameBoard& board
    );

    // Checks for tank-mine collisions and marks explosions
    void checkTankMineCollisions(
        std::vector<Tank>& tanks,
        GameBoard& board
    );

    // Applies path explosions to tanks and shells
    // and marks them as destroyed if they cross a marked path
    void applyPathExplosions(
      std::vector<Tank>& tanks,
      std::vector<Shell>& shells
    );

    // Applies position explosions to tanks and shells
    // and marks them as destroyed if they occupy a marked position
    void applyPositionExplosions(
      std::vector<Tank>& tanks,
      std::vector<Shell>& shells,
      GameBoard& board
    );

    // Marks a position for explosion
    void markPositionExplosionAt(const Point& pos);

    // Marks a path for explosion between two points
    void markPathExplosionAt(const Point& from, const Point& to);

    // For Test purposes
    friend class CollisionHandlerTest;
};

} // namespace GameManager_098765432_123456789