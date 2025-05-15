#include "algo/offensive_tank_algorithm.h"
#include "players/offensive_battle_info.h"
#include "game_board.h"
#include "utils/direction.h"
#include <queue>
#include <set>
#include <algorithm>
#include <iostream>

OffensiveTankAlgorithm::OffensiveTankAlgorithm(int playerId, int tankIndex)
    : BasicTankAlgorithm(playerId, tankIndex) {}

OffensiveTankAlgorithm::~OffensiveTankAlgorithm() = default;

void OffensiveTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    // Downcast to OffensiveBattleInfo
    auto* offensiveInfo = dynamic_cast<OffensiveBattleInfo*>(&info);
    if (offensiveInfo && offensiveInfo->getTargetTankPosition().has_value()) {
        m_targetPosition = offensiveInfo->getTargetTankPosition();
    } else {
        m_targetPosition = std::nullopt;
    }
    // Call base to update tracked state
    BasicTankAlgorithm::updateBattleInfo(info);
}

ActionRequest OffensiveTankAlgorithm::getAction() {
    // 1. Update battle info if necessary
    m_turnsSinceLastUpdate++;
    if (m_turnsSinceLastUpdate > 3) {
        return ActionRequest::GetBattleInfo;
    }
    ActionRequest action = ActionRequest::DoNothing;
    // 2. Avoid if in danger (from shells)
    if (isInDangerFromShells()) {
        action = getActionToSafePosition();
    } else if (canShootEnemy()) {
        action = ActionRequest::Shoot;
    } else if (m_targetPosition) {
        auto turnAction = turnToShootAction();
        if (turnAction.has_value()) {
            action = turnAction.value();
        } else {
            updatePathToTarget();
            auto moveAction = followCurrentPath();
            if (moveAction.has_value()) {
                action = moveAction.value();
            }
        }
    }
    updateState(action);
    return action;
}

std::optional<ActionRequest> OffensiveTankAlgorithm::turnToShootAction() const {
    if (!m_targetPosition) return std::nullopt;
    auto dirOpt = getLineOfSightDirection(m_trackedPosition, m_targetPosition.value());
    if (dirOpt.has_value() && dirOpt.value() != m_trackedDirection) {
        // Use utility to get the rotation action
        return getRotationToDirection(m_trackedDirection, dirOpt.value());
    }
    return std::nullopt;
}

void OffensiveTankAlgorithm::updatePathToTarget() {
    if (!m_targetPosition) {
        m_currentPath.clear();
        return;
    }
    if (m_currentPath.empty() || m_currentPath.back() != m_targetPosition.value() ||
        (m_trackedPosition != m_currentPath.front() && !m_currentPath.empty())) {
        m_currentPath = findPathBFS(m_trackedPosition, m_targetPosition.value());
    }
}

std::optional<ActionRequest> OffensiveTankAlgorithm::followCurrentPath() {
    if (m_currentPath.empty()) return std::nullopt;
    Point nextPoint = m_currentPath.front();
    if (m_trackedPosition == nextPoint) {
        if (m_currentPath.size() > 1) {
            m_currentPath.erase(m_currentPath.begin());
            nextPoint = m_currentPath.front();
        } else {
            m_currentPath.clear();
            return std::nullopt;
        }
    }
    // Determine direction to next point
    auto dirOpt = getLineOfSightDirection(m_trackedPosition, nextPoint);
    if (!dirOpt.has_value()) return std::nullopt;
    if (dirOpt.value() != m_trackedDirection) {
        return getRotationToDirection(m_trackedDirection, dirOpt.value());
    }
    return ActionRequest::MoveForward;
}

std::vector<Point> OffensiveTankAlgorithm::findPathBFS(const Point& start, const Point& target) const {
    if (start == target) return {};
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
            Point neighbor = m_gameBoard.wrapPosition(current + getDirectionDelta(dir));
            if (visited.count(neighbor) > 0 ||
                !m_gameBoard.canMoveTo(neighbor) ||
                m_gameBoard.isMine(neighbor)) {
                continue;
            }
            visited.insert(neighbor);
            cameFrom[neighbor] = current;
            q.push(neighbor);
        }
    }
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
    return {};
}
