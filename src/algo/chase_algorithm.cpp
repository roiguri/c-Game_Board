#include "algo/chase_algorithm.h"
#include "utils/direction.h"
#include <algorithm>

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

std::vector<Point> ChaseAlgorithm::reconstructPath(
  const std::map<Point, Point>& came_from,
  const Point& start,
  const Point& end) const
{
  std::vector<Point> path;
  Point current = end;

  if (came_from.find(end) == came_from.end()) {
      return path;
  }

  // Trace back from end to start, assuming the path exists in the map
  while (current != start) {
      path.push_back(current);
      current = came_from.at(current);
  }
  std::reverse(path.begin(), path.end());
  return path;
}