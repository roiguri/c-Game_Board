#include "algo/chase_algorithm.h"
#include "utils/direction.h"
#include <algorithm>
#include <set>

ChaseAlgorithm::ChaseAlgorithm() : m_lastTargetPosition(-1, -1) {}

ChaseAlgorithm::~ChaseAlgorithm() {
    // TODO: remove if not needed
}

Action ChaseAlgorithm::getNextAction(
    const GameBoard& gameBoard,
    const Tank& myTank,
    const Tank& enemyTank,
    const std::vector<Shell>& shells
) {
    Action action;

    // Priority 1: Avoid danger from shells
    action = getTryToAvoidShellsAction(gameBoard, myTank, shells);
    if (action != Action::None) {
        return action;
    }
    
    // Priority 2: Shoot if you can hit the enemy
    action = getTryToShootAction(gameBoard, myTank, enemyTank);
    if (action != Action::None) {
        return action;
    }

    // Priority 3: Chase the enemy
    updateAndValidatePath(gameBoard, myTank, enemyTank);
    if (!m_currentPath.empty()) {
      const Point& nextStep = m_currentPath.front();
      std::optional<Direction> targetDirOpt = getDirectionToPoint(myTank.getPosition(), nextStep);

      if (targetDirOpt.has_value()) {
          Direction targetDir = targetDirOpt.value();
          if (myTank.getDirection() == targetDir) {
              m_currentPath.erase(m_currentPath.begin());
              return Action::MoveForward;
          } else {
              Action rotationAction = getFirstRotationAction(myTank.getDirection(), targetDir);
              if (rotationAction != Action::None) {
                  return rotationAction;
              }
          }
      } else {
           m_currentPath.clear();
           m_lastTargetPosition = Point(-1,-1);
      }
    }
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

std::vector<Point> ChaseAlgorithm::calculatePathBFS(
  const Point& start,
  const Point& end,
  const GameBoard& gameBoard) const
{
  if (start == end) {
      return {};
  }

  std::queue<Point> q;                
  std::map<Point, Point> came_from;   
  std::set<Point> visited;         

  q.push(start);
  visited.insert(start);

  while (!q.empty()) {
      Point current = q.front();
      q.pop();

      if (current == end) {
          return reconstructPath(came_from, start, end);
      }

      std::vector<Point> neighbors = getValidNeighbors(current, gameBoard);
      for (const Point& neighbor : neighbors) {
          if (visited.count(neighbor) == 0) {
              visited.insert(neighbor);       
              came_from[neighbor] = current;  
              q.push(neighbor);               
          }
      }
  }
  return {};
}

void ChaseAlgorithm::updateAndValidatePath(const GameBoard& gameBoard, const Tank& myTank, const Tank& enemyTank) {
  // TODO: consider changing sensitivity to enemy tank movement / walls position
  bool needsNewPath = m_currentPath.empty() || (enemyTank.getPosition() != m_lastTargetPosition);
  if (needsNewPath) {
      m_currentPath = calculatePathBFS(myTank.getPosition(), enemyTank.getPosition(), gameBoard);
      m_lastTargetPosition = enemyTank.getPosition();
  }

  if (!m_currentPath.empty()) {
      const Point& nextStep = m_currentPath.front();
      if (!gameBoard.canMoveTo(nextStep)) {
          m_currentPath.clear();
          m_lastTargetPosition = Point(-1, -1);
      }
  }
}