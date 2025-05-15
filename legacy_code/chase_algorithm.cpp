#include "algo/chase_algorithm.h"
#include "utils/direction.h"
#include <algorithm>
#include <set>

ChaseAlgorithm::ChaseAlgorithm() : m_lastTargetPosition(-1, -1) {}

ChaseAlgorithm::~ChaseAlgorithm() {}

Action ChaseAlgorithm::getNextAction(
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
    
    // Priority 3: Rotate to face enemy if we have line of sight
    auto lineOfSightDir = getLineOfSightDirection(
      gameBoard, myTank.getPosition(), enemyTank.getPosition());
    if (lineOfSightDir.has_value()) {
        Direction targetDir = lineOfSightDir.value();
        if (targetDir != myTank.getDirection()) {
            return getRotationToDirection(myTank.getDirection(), targetDir);
        }
    }
    
    // Priority 4: Chase the enemy
    updatePathToTarget(
      gameBoard, myTank.getPosition(), enemyTank.getPosition());
    
    if (!m_currentPath.empty()) {
        Action nextAction = followCurrentPath(gameBoard, myTank, shells);
        if (nextAction != Action::None) {
            return nextAction;
        }
    }
    
    // Priority 5: Rotate to scan
    return (myTank.getPosition().getX() % 2 == 0) ? 
            Action::RotateRightEighth : Action::RotateLeftEighth;
}

void ChaseAlgorithm::updatePathToTarget(
  const GameBoard& gameBoard, 
  const Point& start, 
  const Point& target
) {
    bool needNewPath = false;
    if (m_currentPath.empty()) {
        needNewPath = true;
    } else if (Point::euclideanDistance(m_lastTargetPosition, target) > 1.5) {
        needNewPath = true;
    } else {
      Point nextPos = m_currentPath.front();
      if (std::abs(start.getX() - nextPos.getX()) > 1 || 
          std::abs(start.getY() - nextPos.getY()) > 1) {
          needNewPath = true;
      }
    }

    if (needNewPath) {
        m_currentPath = findPathBFS(gameBoard, start, target);
        m_lastTargetPosition = target;
    }
}

Action ChaseAlgorithm::followCurrentPath(
  const GameBoard& gameBoard, 
  const Tank& myTank,
  const std::vector<Shell>& shells
) {
    if (m_currentPath.empty()) {
        return Action::None;
    }
    
    Point nextPoint = m_currentPath.front();

    if (myTank.getPosition() == nextPoint) {
      // Remove this point and move to the next one
      if (m_currentPath.size() > 1) {
          m_currentPath.erase(m_currentPath.begin());
          nextPoint = m_currentPath.front();
      } else {
          m_currentPath.clear();
          return Action::None;
      }
    }

    if (isInDangerFromShells(gameBoard, nextPoint, shells, 2)) {
        return Action::None;
    }
    
    Direction targetDirection = getLineOfSightDirection(
      gameBoard, myTank.getPosition(), nextPoint).value();
    if (targetDirection != myTank.getDirection()) {
        return getRotationToDirection(myTank.getDirection(), targetDirection);
    }
    return Action::MoveForward;
}

std::vector<Point> ChaseAlgorithm::findPathBFS(const GameBoard& gameBoard, 
                                               const Point& start, 
                                               const Point& target) const {
    if (start == target) {
        return std::vector<Point>();
    }
    
    std::queue<Point> q;
    std::map<Point, Point> cameFrom;
    std::set<Point> visited;
    
    q.push(start);
    visited.insert(start);
    
    bool foundPath = false;
    
    while (!q.empty() && !foundPath) {
        Point current = q.front();
        q.pop();
        
        if (current == target) {
            foundPath = true;
            break;
        }
        
        for (const Direction& dir : ALL_DIRECTIONS) {
            Point neighbor = 
              gameBoard.wrapPosition(current + getDirectionDelta(dir));
            
            if (visited.count(neighbor) > 0 || 
                !gameBoard.canMoveTo(neighbor) || 
                gameBoard.isMine(neighbor)) {
                continue;
            }
            
            visited.insert(neighbor);
            cameFrom[neighbor] = current;
            q.push(neighbor);
        }
    }
    
    // Reconstruct the path if we found one
    if (foundPath) {
        std::vector<Point> path;
        Point current = target;
        
        while (current != start) {
            path.push_back(current);
            current = cameFrom[current];
        }
        
        std::reverse(path.begin(), path.end());
        return path;
    }
    return std::vector<Point>();
}