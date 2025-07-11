#include <algorithm>
#include <iostream>
#include <queue>
#include <set>

#include "offensive_battle_info.h"
#include "offensive_tank_algorithm.h"
#include "UserCommon/game_board.h"
#include "UserCommon/bonus/logger/logger.h"
#include "UserCommon/utils/direction.h"
#include "common/TankAlgorithmRegistration.h"

using namespace UserCommon_098765432_123456789;

namespace Algorithm_098765432_123456789 {

TankAlgorithm_098765432_123456789_A::TankAlgorithm_098765432_123456789_A(int playerId, int tankIndex)
    : BasicTankAlgorithm(playerId, tankIndex) {}

TankAlgorithm_098765432_123456789_A::~TankAlgorithm_098765432_123456789_A() = default;

void TankAlgorithm_098765432_123456789_A::updateBattleInfo(BattleInfo& info) {
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

ActionRequest TankAlgorithm_098765432_123456789_A::getAction() {
    ActionRequest action = ActionRequest::GetBattleInfo;
    // 1. Update battle info if necessary
    m_turnsSinceLastUpdate++;
    if (m_turnsSinceLastUpdate > 3) {
        // Update battle info is default action
    } else {
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
    }
    
    // Debug log with position and direction
    std::string debugInfo = "P" + std::to_string(m_playerId) + 
                           "-T" + std::to_string(m_tankIndex) + 
                           " @Tracked Before Update: (" + std::to_string(m_trackedPosition.getX()) + "," + std::to_string(m_trackedPosition.getY()) + ")" +
                           "-" + directionToString(m_trackedDirection);
    LOG_DEBUG(debugInfo);
    
    updateState(action);
    return action;
}

std::optional<ActionRequest> TankAlgorithm_098765432_123456789_A::turnToShootAction() const {
    if (!m_targetPosition) return std::nullopt;
    auto dirOpt = getLineOfSightDirection(m_trackedPosition, m_targetPosition.value());
    if (dirOpt.has_value() && dirOpt.value() != m_trackedDirection) {
        // Use utility to get the rotation action
        return getRotationToDirection(m_trackedDirection, dirOpt.value());
    }
    return std::nullopt;
}

void TankAlgorithm_098765432_123456789_A::updatePathToTarget() {
    // Check if we've reached the target
    if (m_trackedPosition == m_targetPosition.value()) {
        m_currentPath.clear();
        m_previousTargetPosition = std::nullopt;
        return;
    }

    // Check if target moved more than 1 cell
    bool targetMovedSignificantly = false;
    if (m_previousTargetPosition.has_value() && m_targetPosition.has_value()) {
        int distance = GameBoard::stepDistance(
            m_previousTargetPosition.value(),
            m_targetPosition.value(),
            m_gameBoard.getWidth(),
            m_gameBoard.getHeight()
        );
        targetMovedSignificantly = (distance > 1);
    }

    bool needRecalculation = 
        m_currentPath.empty() ||
        isTankOffPath() ||
        targetMovedSignificantly ||
        !isFirstStepValid();

    if (needRecalculation) {
        m_currentPath = findPathBFS(m_trackedPosition, m_targetPosition.value());
        m_previousTargetPosition = m_targetPosition;
    }
}

bool TankAlgorithm_098765432_123456789_A::isTankOffPath() const {
    if (m_currentPath.empty()) return false;
    
    // Tank is on path if current position is at the first point of path,
    // or one step away from the first point of path
    Point nextStep = m_currentPath.front();
    auto dirOpt = getDirectionToPoint(m_trackedPosition, nextStep);
    return !dirOpt.has_value(); // If not adjacent, tank is off path
}

bool TankAlgorithm_098765432_123456789_A::isFirstStepValid() const {
    if (m_currentPath.empty()) return false;
    
    Point nextPoint = m_currentPath.front();
    return isPositionSafe(nextPoint);
}

std::optional<ActionRequest> TankAlgorithm_098765432_123456789_A::followCurrentPath() {
    if (!m_currentPath.empty() && m_trackedPosition == m_currentPath.front()) {
        m_currentPath.erase(m_currentPath.begin());
    }
    
    if (m_currentPath.empty()) return std::nullopt;
    
    Point nextPoint = m_currentPath.front();

    // Determine direction to next point
    auto dirOpt = getDirectionToPoint(m_trackedPosition, nextPoint);
    if (!dirOpt.has_value()) return std::nullopt;
    if (dirOpt.value() != m_trackedDirection) {
        return getRotationToDirection(m_trackedDirection, dirOpt.value());
    }

    m_currentPath.erase(m_currentPath.begin());
    return ActionRequest::MoveForward;
}

std::vector<Point> TankAlgorithm_098765432_123456789_A::findPathBFS(const Point& start, const Point& target) const {
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

            if (current == start && !isPositionSafe(neighbor)) {
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

} // namespace Algorithm_098765432_123456789

// Registration at global scope
using namespace Algorithm_098765432_123456789;
REGISTER_TANK_ALGORITHM(TankAlgorithm_098765432_123456789_A);

