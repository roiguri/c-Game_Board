#include "collision_handler.h"
#include <map>

bool CollisionHandler::checkShellWallCollision(GameBoard& board, Shell& shell) {
    // If the shell is already destroyed, no need to check for collisions
    if (shell.isDestroyed()) {
        return false;
    }
    
    // Get the shell's position
    Point shellPosition = shell.getPosition();
    
    // Check if there's a wall at the shell's position
    if (board.isWall(shellPosition)) {
        // Destroy the shell
        shell.destroy();
        
        // Damage the wall and return whether it was destroyed
        return board.damageWall(shellPosition);
    }
    
    return false; // No wall collision
}

bool CollisionHandler::checkShellCollisions(GameBoard& board, std::vector<Shell>& shells, std::vector<Tank>& tanks) {
    // To be implemented  
    return false;
}

bool CollisionHandler::checkTankCollisions(std::vector<Tank>& tanks) {
    // To be implemented
    return false;
}

bool CollisionHandler::checkTankMineCollisions(GameBoard& board, std::vector<Tank>& tanks) {
    // To be implemented
    return false;
}

bool CollisionHandler::checkShellShellCollision(Shell& shell, std::vector<Shell>& otherShells) {
    // To be implemented
    return false;
}

bool CollisionHandler::checkShellTankCollision(Shell& shell, std::vector<Tank>& tanks) {
    // To be implemented
    return false;
}