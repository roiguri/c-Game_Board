#include "collision_handler.h"

bool CollisionHandler::checkShellCollisions(GameBoard& board,
                                          std::vector<Shell>& shells,
                                          std::vector<Tank>& tanks) {
    // This method will be implemented in a future commit
    return false;
}

bool CollisionHandler::checkTankCollisions(std::vector<Tank>& tanks) {
    // This method will be implemented in a future commit
    return false;
}

bool CollisionHandler::checkTankMineCollisions(GameBoard& board,
                                             std::vector<Tank>& tanks) {
    // This method will be implemented in a future commit
    return false;
}



//TODO decide what to do with 2 shells & wall collision. currently, it if the first one destroys the wall, the second survives
// returns true if tank was destroyed
bool CollisionHandler::checkShellWallCollision(GameBoard& board, Shell& shell) {

    if (shell.isDestroyed()) {
        return false;
    }
    
    Point position = shell.getPosition();
    
    if (board.isWall(position)) {
        shell.destroy();  
        board.damageWall(position);
        return false; // No tank was destroyed in this collision
    }
    
    return false; // No tank was destroyed in this collision
}

bool CollisionHandler::checkShellTankCollision(Shell& shell, std::vector<Tank>& tanks) {
    // This method will be implemented in a future commit
    return false;
}