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
    Action action = Action::None;
    
    action = getTryToAvoidShellsAction(gameBoard, myTank, shells);
    if (action != Action::None) {
        return action;
    }
    
    action = getTryToShootAction(gameBoard, myTank, enemyTank);
    if (action != Action::None) {
        return action;
    }
    
    return action;
}