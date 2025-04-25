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
    if (isInDangerFromShells(gameBoard, myTank, shells)) {
        Action safeAction = findOptimalSafeMove(gameBoard, myTank, enemyTank, shells, false);
        if (safeAction != Action::None) {
            return safeAction;
        }
    }
    
    if (canHitTarget(gameBoard, myTank, enemyTank.getPosition())) {
        return Action::Shoot;
    }
    
    if (isExposedToEnemy(gameBoard, myTank.getPosition(), enemyTank)) {
        Action evasiveAction = findOptimalSafeMove(gameBoard, myTank, enemyTank, shells, true);
        if (evasiveAction != Action::None) {
            return evasiveAction;
        }
    }

    Point nextPosition = myTank.getNextForwardPosition();
    if (isPositionSafe(gameBoard, nextPosition, enemyTank,
                       shells, /* avoidEnemySight */ true)) {
        return Action::MoveForward;
    }
    
    return (myTank.getPosition().getX() % 2 == 0) ? 
            Action::RotateRightEighth : Action::RotateLeftEighth;
}