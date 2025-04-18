#include "algo/defensive_algorithm.h"

DefensiveAlgorithm::DefensiveAlgorithm() {
    // Nothing to initialize
}

DefensiveAlgorithm::~DefensiveAlgorithm() {
    // Nothing to clean up
}

Action DefensiveAlgorithm::getNextAction(
    const GameBoard& gameBoard,
    const Tank& myTank,
    const Tank& enemyTank,
    const std::vector<Shell>& shells
) {
    if (myTank.isDestroyed()) {
        return Action::None;
    }
    
    // TODO: can increase the number of steps to check
    if (isPositionInDangerFromShells(gameBoard, myTank.getPosition(), shells)) {
        Action safeAction = findSafeAction(gameBoard, myTank, shells);
        if (safeAction != Action::None) {
            return safeAction;
        }
    }
    
    if (canShootEnemy(gameBoard, myTank, enemyTank)) {
        return Action::Shoot;
    }
    
    return Action::None;
}