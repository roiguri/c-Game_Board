#include "algo/chase_algorithm.h"
#include "utils/direction.h"

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
    // TODO: Will be implemented in a later commit
    return Action::None;
}

std::vector<Point> ChaseAlgorithm::getValidNeighbors(const Point& current, const GameBoard& gameBoard) const {
  std::vector<Point> neighbors;
  neighbors.reserve(8);

  // TODO: consider adding an iterator on all neighbor points 
  for (const Direction& dir : ALL_DIRECTIONS) {
    Point delta = getDirectionDelta(dir);
    Point potentialNeighbor = current + delta;

    Point wrappedNeighbor = gameBoard.wrapPosition(potentialNeighbor); //

    GameBoard::CellType cellType = gameBoard.getCellType(wrappedNeighbor); //
    if (cellType != GameBoard::CellType::Wall && cellType != GameBoard::CellType::Mine) { //
        neighbors.push_back(wrappedNeighbor);
    }
  }
  return neighbors;
}