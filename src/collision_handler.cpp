#include "collision_handler.h"
#include <map>

bool CollisionHandler::resolveAllCollisions(GameBoard& board, std::vector<Shell>& shells, std::vector<Tank>& tanks) {
    std::vector<Point> explosionPositions;
    bool tankDestroyed = false;


    // Check for collisions between shells tanks, mines and walls
    tankDestroyed |= checkShellCollisions(board, shells, tanks, explosionPositions);
    tankDestroyed |= checkTankCollisions(tanks, explosionPositions);
    tankDestroyed |= checkTankMineCollisions(board, tanks, explosionPositions);

    // destroy objects at explosion positions
    tankDestroyed |= applyExplosions(explosionPositions, shells, tanks, board);

    return tankDestroyed;
}



bool CollisionHandler::checkShellWallCollision(GameBoard& board, Shell& shell, std::vector<Point>& explosionPositions) {
    if (shell.isDestroyed()) {
        return false;
    }
    
    Point shellPosition = shell.getPosition();
    
    // if shell is at a wall position, destroy it, damage the wall, and mark explosion
    if (board.isWall(shellPosition)) {
        shell.destroy();
        board.damageWall(shellPosition);
        markExplosionAt(shellPosition, explosionPositions);
    }
    return false; // No tanks were destroyed by shell-wall collisions
}



bool CollisionHandler::checkShellShellCollision(Shell& shell, std::vector<Shell>& otherShells, std::vector<Point>& explosionPositions) {
    if (shell.isDestroyed()) {
        return false;
    }
    
    Point shellPosition = shell.getPosition();
    bool collisionDetected = false;
    
    // Check for collisions with other shells
    for (auto& otherShell : otherShells) {
        if (otherShell.isDestroyed()) {
            continue;
        }
        
        // Skip if it's the same shell (comparing memory addresses)
        if (&shell == &otherShell) {
            continue;
        }
        
        // if the other shell is at the same position, destroy it
        if (otherShell.getPosition() == shellPosition) {
            otherShell.destroy();
            collisionDetected = true;
            break; // if other shells on the same position, they will be destroyed by applyExplosions
        }
    }

    // If a collision was detected, destroy the original shell too and mark explosion
    if (collisionDetected) {
        shell.destroy();
        markExplosionAt(shellPosition, explosionPositions);
    }
    
    return false; // No tanks were directly destroyed by shell-shell collisions
}



bool CollisionHandler::checkShellTankCollision(Shell& shell, std::vector<Tank>& tanks, std::vector<Point>& explosionPositions) {
    if (shell.isDestroyed()) {
        return false;
    }

    Point shellPosition = shell.getPosition();
    bool tankDestroyed = false;

    // Check for collisions with tanks
    for (auto& tank : tanks) {
        if (tank.isDestroyed()) {
            continue;
        }

        // if the tank is at the same position as the shell, destroy it 
        if (tank.getPosition() == shellPosition) {
            shell.destroy();
            tank.destroy();
            markExplosionAt(shellPosition, explosionPositions);
            tankDestroyed = true;
            break; //other tanks at the same position will be exploded by applyExplosions
        }
    }
    return tankDestroyed;
}



bool CollisionHandler::checkShellCollisions(GameBoard& board, std::vector<Shell>& shells, std::vector<Tank>& tanks, std::vector<Point>& explosionPositions) {
    bool tankDestroyed = false;

    // Check for all shell related collisions
    for (auto& shell : shells) {
        tankDestroyed |= checkShellWallCollision(board, shell, explosionPositions);
        tankDestroyed |= checkShellShellCollision(shell, shells, explosionPositions);
        tankDestroyed |= checkShellTankCollision(shell, tanks, explosionPositions);
    }

    return tankDestroyed;  
}


//TODO pointers?
bool CollisionHandler::checkTankCollisions(std::vector<Tank>& tanks, std::vector<Point>& explosionPositions) {
    std::map<Point, std::vector<Tank*>> tanksByPosition;
    bool tankDestroyed = false;

    // Group tanks by their positions
    for (auto& tank : tanks) {
        if (!tank.isDestroyed()) {
            tanksByPosition[tank.getPosition()].push_back(&tank);
        }
    }

    // check for positions with more than one tank
    for (const auto& [pos, tankList] : tanksByPosition) {
        if (tankList.size() > 1) {
            for (auto* tank : tankList) {
                tank->destroy();
            }
            markExplosionAt(pos, explosionPositions);
            tankDestroyed = true;
        }
    }
    return tankDestroyed;
}



bool CollisionHandler::checkTankMineCollisions(GameBoard& board, std::vector<Tank>& tanks, std::vector<Point>& explosionPositions) {
    bool tankDestroyed = false;

    // Check for tank-mine collisions
    for (auto& tank : tanks) {
        if (tank.isDestroyed()){
            continue;
        } 

        Point tankPosition = tank.getPosition();

        // if the tank is at the same position as a mine, destroy it, remove the mine, and mark explosion
        if (board.getCellType(tankPosition) == GameBoard::CellType::Mine) {
            tank.destroy();
            board.setCellType(tankPosition, GameBoard::CellType::Empty); 
            markExplosionAt(tankPosition, explosionPositions);
            tankDestroyed = true;
        }
    }
    return tankDestroyed;
}




//Helper Functions

void CollisionHandler::markExplosionAt(const Point& position, std::vector<Point>& explosionPositions) {
    // Check if this position is already marked
    for (const auto& pos : explosionPositions) {
        if (pos == position) {
            return; 
        }
    }
    explosionPositions.push_back(position);
}


bool CollisionHandler::applyExplosions(const std::vector<Point>& explosionPositions, std::vector<Shell>& shells, std::vector<Tank>& tanks, GameBoard& board) {
    bool tankDestroyed = false;

    for (const Point& pos : explosionPositions) {
        
        // Destroy all shells at the explosion position
        for (auto& shell : shells) {
            if (!shell.isDestroyed() && shell.getPosition() == pos) {
                shell.destroy();
            }
        }

        // Destroy all tanks at the explosion position
        for (auto& tank : tanks) {
            if (!tank.isDestroyed() && tank.getPosition() == pos) {
                tank.destroy();
                tankDestroyed = true;
            }
        }

        // Remove mines at the explosion position
        if (board.getCellType(pos) == GameBoard::CellType::Mine) {
            board.setCellType(pos, GameBoard::CellType::Empty);
        }
    }

    return tankDestroyed;
}

