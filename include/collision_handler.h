#pragma once
#include <vector>
#include "game_board.h"
#include "tank.h"
#include "shell.h"

class CollisionHandler {
public:
    // Shell collisions
    
    // Check and handle shell-wall collisions
    // Returns true if wall was destroyed
    static bool checkShellWallCollision(GameBoard& board, Shell& shell);
    
    // Check and handle all shell collisions (shell-shell and shell-tank)
    // Returns true if any tank was destroyed
    static bool checkShellCollisions(GameBoard& board, 
                                   std::vector<Shell>& shells, 
                                   std::vector<Tank>& tanks);
    
    // Tank collisions
    
    // Check and handle tank-tank collisions
    // Returns true if any tank was destroyed
    static bool checkTankCollisions(std::vector<Tank>& tanks);
    
    // Check and handle tank-mine collisions
    // Returns true if any tank was destroyed
    static bool checkTankMineCollisions(GameBoard& board, 
                                      std::vector<Tank>& tanks);

private:
    // Helper methods
    
    // Check for shell-shell collisions
    // Returns true if any collisions were detected
    static bool checkShellShellCollision(Shell& shell, std::vector<Shell>& otherShells);
    
    // Check for shell-tank collisions
    // Returns true if any tank was destroyed
    static bool checkShellTankCollision(Shell& shell, std::vector<Tank>& tanks);
};