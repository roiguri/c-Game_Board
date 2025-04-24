#pragma once
#include <vector>
#include "game_board.h"
#include "objects/tank.h"
#include "objects/shell.h"

class CollisionHandler {
public:
    bool resolveAllCollisions(
        std::vector<Tank>& tanks,
        std::vector<Shell>& shells,
        GameBoard& board
    );

private:
    // For Test purposes
    friend class CollisionHandlerTest;

    // TODO: consider changing to a set
    std::vector<std::pair<float, float>> m_pathExplosions;   
    std::vector<Point> m_positionExplosions;
    int m_boardWidth;
    int m_boardHeight;


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
    bool pathCrossedMidpoint(const Point& prev, const Point& curr, float mx, float my);
    std::pair<float, float> computeMidpoint(const Point& a, const Point& b);
};