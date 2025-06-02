#include "algo/basic_tank_algorithm.h"
#include "objects/tank.h"
#include "utils/point.h"
#include "utils/direction.h"
#include "bonus/logger/logger.h"
#include <cassert>
#include <iostream>

BasicTankAlgorithm::BasicTankAlgorithm(int playerId, int tankIndex)
    : m_playerId(playerId), m_tankIndex(tankIndex), m_gameBoard(5, 5) {
    m_trackedPosition = Point(0, 0);
    m_trackedDirection = (playerId == 1) ? Direction::Left : Direction::Right;
    m_trackedShells = Tank::INITIAL_SHELLS;
    m_trackedCooldown = 0;
    m_turnsSinceLastUpdate = 4;
}

BasicTankAlgorithm::~BasicTankAlgorithm() = default;

ActionRequest BasicTankAlgorithm::getAction() {
    m_turnsSinceLastUpdate++;
    ActionRequest action;
    // 1. Check if BattleInfo is outdated
    if (m_turnsSinceLastUpdate > 3) {
        action = ActionRequest::GetBattleInfo;
        return action;
    }
    // 2. Check if in danger
    else if (isInDangerFromShells()) {
        action = getActionToSafePosition();
    }
    // 3. Check if can shoot enemy
    else if (canShootEnemy()) {
        action = ActionRequest::Shoot;
    } 
    else {
        action = getActionToSafePosition();
    }
    
    // Debug log with position, direction, and requested action
    std::string debugInfo = "P" + std::to_string(m_playerId) + 
                           "-T" + std::to_string(m_tankIndex) + 
                           " @Tracked Before Update: (" + std::to_string(m_trackedPosition.getX()) + "," + std::to_string(m_trackedPosition.getY()) + ")" +
                           "-" + directionToString(m_trackedDirection);
    LOG_DEBUG(debugInfo);
    
    updateState(action);
    return action;
}

void BasicTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    m_turnsSinceLastUpdate = 0;
    auto& impl = dynamic_cast<BattleInfoImpl&>(info);
    m_trackedPosition = impl.getOwnTankPosition();
    m_gameBoard = impl.getGameBoard();
    m_enemyTanks = impl.getEnemyTankPositions();
    m_friendlyTanks = impl.getFriendlyTankPositions();
    m_shells = impl.getShellPositions();
}

bool BasicTankAlgorithm::canShootEnemy() const {
    const Point& myPos = m_trackedPosition;
    const Direction myDir = m_trackedDirection;
    for (const auto& enemyPos : m_enemyTanks) {
        if (checkLineOfSightInDirection(myPos, enemyPos, myDir)) {
            return true;
        }
    }
    return false;
}

std::optional<Direction> BasicTankAlgorithm::getLineOfSightDirection(const Point& from, const Point& to) const {
    for (const Direction& dir : ALL_DIRECTIONS) {
        if (checkLineOfSightInDirection(from, to, dir)) {
            return dir;
        }
    }
    return std::nullopt;
}

bool BasicTankAlgorithm::checkLineOfSightInDirection(const Point& from, const Point& to, Direction direction) const {
    if (from == to) {
        return true;
    }
    Point current = from;
    Point delta = getDirectionDelta(direction);
    int maxSteps = m_gameBoard.getWidth() + m_gameBoard.getHeight();
    int steps = 0;
    while (steps < maxSteps) {
        current = m_gameBoard.wrapPosition(current + delta);
        steps++;
        if (current == to) {
            return true;
        }
        // Check for walls blocking line of sight
        if (m_gameBoard.isWall(current)) {
            return false;
        }
        // Check for tanks blocking line of sight
        if (isTankAtPosition(current)) {
            return false;
        }
    }
    return false;
}

bool BasicTankAlgorithm::isTankAtPosition(const Point& position) const {
    // Check enemy tanks
    for (const auto& tankPos : m_enemyTanks) {
        if (tankPos == position) {
            return true;
        }
    }
    
    // Check friendly tanks
    for (const auto& tankPos : m_friendlyTanks) {
        if (tankPos == position) {
            return true;
        }
    }
    
    return false;
}

bool BasicTankAlgorithm::isInDangerFromShells(const Point& position) const {
    for (const Point& shellPos : m_shells) {
        // Filter out shells that are too far away
        if (GameBoard::stepDistance(shellPos, position, m_gameBoard.getWidth(), m_gameBoard.getHeight()) > 4) {
            continue;
        }

        for (const Direction& dir : ALL_DIRECTIONS) {
            if (!checkLineOfSightInDirection(shellPos, position, dir)) continue;
            Point current = shellPos;
            for (int step = 1; step < 4; ++step) {
                current = m_gameBoard.wrapPosition(current + getDirectionDelta(dir));
                if (current == position) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool BasicTankAlgorithm::isInDangerFromShells() const {
    return isInDangerFromShells(m_trackedPosition);
}

bool BasicTankAlgorithm::isPositionSafe(const Point& position) const {
    // Check for wall
    if (!m_gameBoard.canMoveTo(position)) {
        return false;
    }
    // Check for mine
    if (m_gameBoard.getCellType(position.getX(), position.getY()) == GameBoard::CellType::Mine) {
        return false;
    }
    // Check for tanks (enemy or friendly)
    for (const auto& tankPos : m_enemyTanks) {
        if (tankPos == position) return false;
    }
    for (const auto& tankPos : m_friendlyTanks) {
        if (tankPos == position) return false;
    }
    // Check for shell danger at this position
    if (isInDangerFromShells(position)) {
        return false;
    }
    return true;
}

std::vector<Point> BasicTankAlgorithm::getSafePositions() const {
    std::vector<Point> safePositions;
    for (const Direction& dir : ALL_DIRECTIONS) {
        Point adj = m_gameBoard.wrapPosition(m_trackedPosition + getDirectionDelta(dir));
        if (isPositionSafe(adj)) {
            safePositions.push_back(adj);
        }
    }
    return safePositions;
}

ActionRequest BasicTankAlgorithm::getRotationToDirection(Direction current, Direction target) {
    if (current == target) return ActionRequest::DoNothing;
    if (target == rotateRight(current, false)) return ActionRequest::RotateRight45;
    if (target == rotateLeft(current, false)) return ActionRequest::RotateLeft45;
    if (target == rotateRight(current, true)) return ActionRequest::RotateRight90;
    if (target == rotateLeft(current, true)) return ActionRequest::RotateLeft90;
    // Fallback: choose the shortest rotation
    int stepsClockwise = 0, stepsCounterClockwise = 0;
    Direction tempDir = current;
    while (tempDir != target && stepsClockwise < 8) {
        tempDir = rotateRight(tempDir, false);
        stepsClockwise++;
    }
    tempDir = current;
    while (tempDir != target && stepsCounterClockwise < 8) {
        tempDir = rotateLeft(tempDir, false);
        stepsCounterClockwise++;
    }
    return (stepsClockwise <= stepsCounterClockwise) ? ActionRequest::RotateRight90 : ActionRequest::RotateLeft90;
}

BasicTankAlgorithm::SafeMoveOption BasicTankAlgorithm::getSafeMoveOption(const Point& pos) const {
    const Point& current = m_trackedPosition;
    const Direction currentDir = m_trackedDirection;
    SafeMoveOption option{pos, ActionRequest::DoNothing, 1000, currentDir};
    if (pos == current) {
        option.action = ActionRequest::DoNothing;
        option.cost = 0;
        return option;
    }
    auto dirOpt = getLineOfSightDirection(current, pos);
    if (!dirOpt) return option;
    Direction targetDir = dirOpt.value();
    option.direction = targetDir;
    if (current + getDirectionDelta(targetDir) == pos) {
        // Adjacent in that direction
        if (currentDir == targetDir) {
            option.action = ActionRequest::MoveForward;
            option.cost = 1;
        } else {
            option.action = getRotationToDirection(currentDir, targetDir);
            // Estimate cost: 1 for move + 1 for each 45-degree rotation
            int leftSteps = 0, rightSteps = 0;
            Direction temp = currentDir;
            while (temp != targetDir && leftSteps < 8) {
                temp = rotateLeft(temp, false);
                leftSteps++;
            }
            temp = currentDir;
            while (temp != targetDir && rightSteps < 8) {
                temp = rotateRight(temp, false);
                rightSteps++;
            }
            option.cost = std::min(leftSteps, rightSteps) + 1;
        }
    }
    return option;
}

std::vector<BasicTankAlgorithm::SafeMoveOption> BasicTankAlgorithm::getSafeMoveOptions(const std::vector<Point>& positions) const {
    std::vector<SafeMoveOption> options;
    for (const auto& pos : positions) {
        options.push_back(getSafeMoveOption(pos));
    }
    return options;
}

ActionRequest BasicTankAlgorithm::getActionToSafePosition() const {
    auto safePositions = getSafePositions();
    if (safePositions.empty()) {
        return ActionRequest::DoNothing;
    }
    auto options = getSafeMoveOptions(safePositions);
    auto best = std::min_element(options.begin(), options.end());
    return best->action;
}

void BasicTankAlgorithm::updateState(ActionRequest lastAction) {
    if (m_trackedCooldown > 0) m_trackedCooldown--;
    switch (lastAction) {
        case ActionRequest::MoveForward: {
            Point delta = getDirectionDelta(m_trackedDirection);
            m_trackedPosition = m_gameBoard.wrapPosition(m_trackedPosition + delta);
            break;
        }
        case ActionRequest::RotateLeft90:
            m_trackedDirection = rotateLeft(m_trackedDirection, true);
            break;
        case ActionRequest::RotateLeft45:
            m_trackedDirection = rotateLeft(m_trackedDirection, false);
            break;
        case ActionRequest::RotateRight90:
            m_trackedDirection = rotateRight(m_trackedDirection, true);
            break;
        case ActionRequest::RotateRight45:
            m_trackedDirection = rotateRight(m_trackedDirection, false);
            break;
        case ActionRequest::Shoot:
            if (m_trackedShells > 0) m_trackedShells--;
            m_trackedCooldown = Tank::SHOOT_COOLDOWN;
            break;
        default:
            break; // Ignore MoveBackward, GetBattleInfo, DoNothing for now
    }
    if (m_trackedCooldown < 0) m_trackedCooldown = 0;
} 