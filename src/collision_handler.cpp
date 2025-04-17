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



bool CollisionHandler::checkShellTankCollision(Shell& shell, std::vector<Tank>& tanks, std::vector<Point>& explosionPositions) {
    if (shell.isDestroyed()) {
        return false;
    }

    Point shellPosition = shell.getPosition();
    bool tankDestroyed = false;

    for (auto& tank : tanks) {
        if (tank.isDestroyed()) {
            continue;
        }

        if (tank.getPosition() == shellPosition) {
            shell.destroy();
            tank.destroy();
            markExplosionAt(shellPosition, explosionPositions);
            tankDestroyed = true;
            // Continue checking other tanks in case of stacking
        }
    }
    return tankDestroyed;
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



void CollisionHandler::applyExplosions(const std::vector<Point>& explosionPositions, std::vector<Shell>& shells, std::vector<Tank>& tanks) {
    for (const Point& pos : explosionPositions) {
        // Destroy all shells at this position
        for (auto& shell : shells) {
            if (!shell.isDestroyed() && shell.getPosition() == pos) {
                shell.destroy();
            }
        }

        // Destroy all tanks at this position
        for (auto& tank : tanks) {
            if (!tank.isDestroyed() && tank.getPosition() == pos) {
                tank.destroy();
            }
        }
    }
}
