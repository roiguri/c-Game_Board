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



bool CollisionHandler::checkShellShellCollision(Shell& shell, std::vector<Shell>& otherShells, std::vector<Point>& explosionPositions) {
    // If the shell is already destroyed, no need to check for collisions
    if (shell.isDestroyed()) {
        return false;
    }
    
    // Get the shell's position
    Point shellPosition = shell.getPosition();
    bool collisionDetected = false;
    
    // Check for collisions with other shells
    for (auto& otherShell : otherShells) {
        // Skip if the other shell is already destroyed
        if (otherShell.isDestroyed()) {
            continue;
        }
        
        // Skip if it's the same shell (comparing memory addresses)
        if (&shell == &otherShell) {
            continue;
        }
        
        // Check if the shells are at the same position
        if (otherShell.getPosition() == shellPosition) {
            // Collision detected - destroy the other shell
            otherShell.destroy();
            printf("Shell destroyed at position: (%d, %d)\n", shellPosition.x, shellPosition.y);
            printf("is shell2 destroyed: %d", otherShell.isDestroyed());
            collisionDetected = true;
            // Mark this position as having an explosion
            markExplosionAt(shellPosition, explosionPositions);
            // Don't break - continue checking all shells to destroy all at this position
        }
    }
    
    // If a collision was detected, destroy the original shell too
    if (collisionDetected) {
        shell.destroy();
    }
    
    return false; // No tanks were directly destroyed by shell-shell collisions
}



bool CollisionHandler::checkShellTankCollision(Shell& shell, std::vector<Tank>& tanks) {
    // To be implemented
    return false;
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


void CollisionHandler::markExplosionAt(const Point& position, std::vector<Point>& explosionPositions) {
    // Check if this position is already marked
    for (const auto& pos : explosionPositions) {
        if (pos == position) {
            return; // Position already marked
        }
    }
    
    // Add this position to the list of explosion positions
    explosionPositions.push_back(position);
}