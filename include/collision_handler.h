#pragma once
#include <vector>
#include "game_board.h"
#include "tank.h"
#include "shell.h"

class CollisionHandler {
public:
    bool resolveAllCollisions(
        std::vector<Tank>& tanks,
        std::vector<Shell>& shells,
        GameBoard& board
    );

private:
    std::vector<Point> m_explosionPositions;

    void detectPathCrossings(
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

    void checkTankMineCollisions(
        std::vector<Tank>& tanks,
        GameBoard& board
    );

    bool applyExplosions(
        std::vector<Tank>& tanks,
        std::vector<Shell>& shells,
        GameBoard& board
    );

    void markExplosionAt(const Point& position);
};