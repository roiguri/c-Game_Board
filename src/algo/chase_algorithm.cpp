#include "algo/chase_algorithm.h"

ChaseAlgorithm::ChaseAlgorithm() : Algorithm() {
    // Constructor implementation
}

ChaseAlgorithm::~ChaseAlgorithm() {
    // Destructor implementation
}

Action ChaseAlgorithm::getNextAction(
    const GameBoard& gameBoard,
    const Tank& myTank,
    const Tank& enemyTank,
    const std::vector<Shell>& shells
) {
    // TODO: not implemented
    return Action::None;
}