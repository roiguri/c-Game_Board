#include "algo/basic_tank_algorithm.h"
#include "objects/tank.h"
#include "utils/point.h"
#include <cassert>

BasicTankAlgorithm::BasicTankAlgorithm(int playerId, int tankIndex)
    : m_playerId(playerId), m_tankIndex(tankIndex), m_gameBoard(0, 0) {}

BasicTankAlgorithm::~BasicTankAlgorithm() = default;

ActionRequest BasicTankAlgorithm::getAction() {
    m_turnsSinceLastUpdate++;
    // 1. Check if BattleInfo is outdated
    if (m_turnsSinceLastUpdate > 3) {
        return ActionRequest::GetBattleInfo;
    }
    // 2. Check if in danger
    if (isInDangerFromShells()) {
        return getActionToSafePosition();
    }
    // 3. Check if can shoot enemy
    if (canShootEnemy()) {
        return ActionRequest::Shoot;
    }
    // TODO: decide what to do if not in danger and not shooting
    return getActionToSafePosition();
}

void BasicTankAlgorithm::setTank(Tank& tank) {
    m_tank = std::ref(tank);
}

void BasicTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    m_turnsSinceLastUpdate = 0;
    auto& impl = dynamic_cast<BattleInfoImpl&>(info); // TODO: make sure this is correct

    m_gameBoard = impl.getGameBoard(); // TODO: verify all copies not references
    m_enemyTanks = impl.getEnemyTankPositions();
    m_friendlyTanks = impl.getFriendlyTankPositions();
    m_shells = impl.getShellPositions();
}

bool BasicTankAlgorithm::canShootEnemy() const {
    const Tank& myTank = m_tank.value().get();
    const Point& myPos = myTank.getPosition();
    Direction myDir = myTank.getDirection();
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

// FIXME: consider changing to check if tanks blocks line of sight
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
        if (m_gameBoard.isWall(current)) {
            return false;
        }
    }
    return false;
}

bool BasicTankAlgorithm::isInDangerFromShells(const Point& position) const {
    for (const Point& shellPos : m_shells) {
        auto dirOpt = getLineOfSightDirection(shellPos, position);
        if (!dirOpt) continue;
        for (const Direction& dir : ALL_DIRECTIONS) {
            if (!checkLineOfSightInDirection(shellPos, position, dir)) continue;
            Point current = shellPos;
            // TODO: consider replacing with a simple distance check
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
    if (!m_tank.has_value()) return false;
    return isInDangerFromShells(m_tank.value().get().getPosition());
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
    if (!m_tank.has_value()) return safePositions;
    const Point& position = m_tank.value().get().getPosition();
    for (const Direction& dir : ALL_DIRECTIONS) {
        Point adj = m_gameBoard.wrapPosition(position + getDirectionDelta(dir));
        if (isPositionSafe(adj)) {
            safePositions.push_back(adj);
        }
    }
    return safePositions;
}

BasicTankAlgorithm::SafeMoveOption BasicTankAlgorithm::getSafeMoveOption(const Point& pos) const {
    const Tank& tank = m_tank.value().get();
    const Point& current = tank.getPosition();
    Direction currentDir = tank.getDirection();
    SafeMoveOption option{pos, ActionRequest::DoNothing, 1000, currentDir};
    if (pos == current) {
        option.action = ActionRequest::DoNothing;
        option.cost = 0;
        return option;
    }
    auto dirOpt = getLineOfSightDirection(current, pos);
    if (!dirOpt) return option;
    Direction targetDir = *dirOpt;
    option.direction = targetDir;
    if (current + getDirectionDelta(targetDir) == pos) {
        // Adjacent in that direction
        if (currentDir == targetDir) {
            option.action = ActionRequest::MoveForward;
            option.cost = 1;
        } else {
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
            if (leftSteps <= rightSteps) {
                option.action = (leftSteps == 2) ? ActionRequest::RotateLeft90 : ActionRequest::RotateLeft45;
                option.cost = leftSteps;
            } else {
                option.action = (rightSteps == 2) ? ActionRequest::RotateRight90 : ActionRequest::RotateRight45;
                option.cost = rightSteps;
            }
            option.cost += 1; // Add 1 for the move after rotation
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