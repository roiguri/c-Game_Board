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
    // Priority 1: Avoid shells
    if (isInDangerFromShells(gameBoard, myTank, shells)) {
        Action safeAction = 
          findOptimalSafeMove(gameBoard, myTank, enemyTank, shells, false);
        if (safeAction != Action::None) {
            return safeAction;
        }
    }
    
    // Priority 2: Shoot if in direction + have line of sight
    if (canHitTarget(gameBoard, myTank, enemyTank.getPosition())) {
        return Action::Shoot;
    }
    
    // Priority 3: If enemy has line of sight, avoid it
    if (isExposedToEnemy(gameBoard, myTank.getPosition(), enemyTank)) {
        Action evasiveAction = 
          findOptimalSafeMove(gameBoard, myTank, enemyTank, shells, true);
        if (evasiveAction != Action::None) {
            return evasiveAction;
        }
    }

    // Priority 4: If can move forward safely, do so
    Point nextPosition = myTank.getNextForwardPosition();
    if (isPositionSafe(gameBoard, nextPosition, enemyTank,
                       shells, /* avoidEnemySight */ true)) {
        return Action::MoveForward;
    }
    
    // Priority 5: Rotate to scan
    return (myTank.getPosition().getX() % 2 == 0) ? 
            Action::RotateRightEighth : Action::RotateLeftEighth;
}