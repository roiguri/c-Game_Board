#pragma once
#include <vector>
#include "game_board.h"
#include "tank.h"
#include "shell.h"

class CollisionHandler {
public:
    // Resolve all collisions
    // Returns true if any tank was destroyed
    static bool resolveAllCollisions(GameBoard& board, std::vector<Shell>& shells, std::vector<Tank>& tanks);

    // Shell collisions
    
    // Check and handle shell-wall collisions
    // Returns true if any tank was destroyed(can't happen)
    static bool checkShellWallCollision(GameBoard& board, Shell& shell, std::vector<Point>& explosionPositions);
    
    // Check for shell-shell collisions
    // Returns true if any tank was destroyed(can't happen)
    static bool checkShellShellCollision(Shell& shell, std::vector<Shell>& otherShells, std::vector<Point>& explosionPositions);

    // Check for shell-tank collisions
    // Returns true if any tank was destroyed
    static bool checkShellTankCollision(Shell& shell, std::vector<Tank>& tanks, std::vector<Point>& explosionPositions);

    // Check and handle all shell collisions (shell-shell and shell-tank)
    // Returns true if any tank was destroyed
    static bool checkShellCollisions(GameBoard& board, std::vector<Shell>& shells, std::vector<Tank>& tanks, std::vector<Point>& explosionPositions);
    
    // Check and handle tank-tank collisions
    // Returns true if any tank was destroyed
    static bool checkTankCollisions(std::vector<Tank>& tanks, std::vector<Point>& explosionPositions);
    
    // Check and handle tank-mine collisions
    // Returns true if any tank was destroyed
    static bool checkTankMineCollisions(GameBoard& board, std::vector<Tank>& tanks, std::vector<Point>& explosionPositions);

    
private:
    // Helper methods

    // Marks an explosion at the given position
    static void markExplosionAt(const Point& position, std::vector<Point>& explosionPositions);

    // Destroys all tanks, shells, and mines at positions where explosions occurred
    // Returns true if any tank was destroyed
    static bool applyExplosions(const std::vector<Point>& explosionPositions, std::vector<Shell>& shells, std::vector<Tank>& tanks, GameBoard& board);
};