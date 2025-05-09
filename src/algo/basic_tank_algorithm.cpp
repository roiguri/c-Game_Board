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
        return ActionRequest::DoNothing;
    }
    // 3. Check if can shoot enemy
    if (canShootEnemy()) {
        return ActionRequest::Shoot;
    }
    return ActionRequest::DoNothing;
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
    // Example: if any enemy tank is present
    return !m_enemyTanks.empty();
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

// TODO: consider optimizing performance
bool BasicTankAlgorithm::isInDangerFromShells() const {
    if (!m_tank.has_value()) return false;
    const Point& position = m_tank.value().get().getPosition();

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