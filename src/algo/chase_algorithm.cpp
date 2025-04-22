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
    // Priority 1: Avoid shells
    if (isInDanger(gameBoard, myTank, shells)) {
        Action safeAction = findOptimalSafeMove(gameBoard, myTank, enemyTank, shells, false);
        if (safeAction != Action::None) {
            return safeAction;
        }
    }
    
    // Priority 2: Shoot if in direction + have line of sight
    if (canHitTarget(gameBoard, myTank, enemyTank.getPosition())) {
        return Action::Shoot;
    }
    
    // TODO: Consider removing this priority
    // Priority 3: Rotate to face enemy if we have line of sight
    auto lineOfSightDir = getLineOfSightDirection(gameBoard, myTank.getPosition(), enemyTank.getPosition());
    if (lineOfSightDir.has_value()) {
        Direction targetDir = lineOfSightDir.value();
        if (targetDir != myTank.getDirection()) {
            return getRotationToDirection(myTank.getDirection(), targetDir);
        }
    }
    
    // Priority 4: Chase the enemy
    updatePathToTarget(gameBoard, myTank.getPosition(), enemyTank.getPosition());
    
    if (!m_currentPath.empty()) {
        return followCurrentPath(gameBoard, myTank);
    }
    
    return (myTank.getPosition().x % 2 == 0) ? 
            Action::RotateRightQuarter : Action::RotateLeftQuarter;
}

void ChaseAlgorithm::updatePathToTarget(const GameBoard& gameBoard, const Point& start, 
                                       const Point& target) {
    // Recalculate path if:
    // 1. Current path is empty
    // 2. Target has moved since last calculation
    bool needNewPath = m_currentPath.empty() || target != m_lastTargetPosition;
    
    if (needNewPath) {
        m_currentPath = findPathBFS(gameBoard, start, target);
        m_lastTargetPosition = target;
    }
    
    // Check if the next step in our path is still valid
    // TODO: change to check if the step is still safe
    if (!m_currentPath.empty()) {
        Point nextStep = m_currentPath.front();
        if (!gameBoard.canMoveTo(nextStep)) {
            // Path is now blocked, need to recalculate
            m_currentPath.clear();
            m_lastTargetPosition = Point(-1, -1);
        }
    }
}

Action ChaseAlgorithm::followCurrentPath(const GameBoard& gameBoard, const Tank& myTank) const {
    if (m_currentPath.empty()) {
        return Action::None;
    }
    
    Point nextPoint = m_currentPath.front();
    
    // TODO: consider this
    // Check if the next point is directly adjacent to our current position
    auto directionOpt = getLineOfSightDirection(gameBoard, myTank.getPosition(), nextPoint);
    if (!directionOpt.has_value()) {
        return Action::None;
    }
    
    Direction targetDirection = directionOpt.value();
    if (targetDirection != myTank.getDirection()) {
        return getRotationToDirection(myTank.getDirection(), targetDirection);
    }
    return Action::MoveForward;
}

// TODO: make sure that we always check next step for danger
std::vector<Point> ChaseAlgorithm::findPathBFS(const GameBoard& gameBoard, const Point& start, 
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
        
        // Explore all valid neighbors
        for (const Direction& dir : ALL_DIRECTIONS) {
            Point neighbor = gameBoard.wrapPosition(current + getDirectionDelta(dir));
            
            // Skip if already visited or can't move to this position
            if (visited.count(neighbor) > 0 || !gameBoard.canMoveTo(neighbor)) {
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
        
        // Reverse to get path from start to target
        std::reverse(path.begin(), path.end());
        return path;
    }
    return std::vector<Point>();
}