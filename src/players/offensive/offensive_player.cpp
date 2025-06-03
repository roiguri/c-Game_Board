#include "players/offensive/offensive_player.h"
#include <limits>
#include <cmath>

OffensivePlayer::OffensivePlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
    : BasicPlayer(playerIndex, x, y, maxSteps, numShells),
      m_offensiveBattleInfo(static_cast<int>(x), static_cast<int>(y)) {}

OffensivePlayer::~OffensivePlayer() = default;

void OffensivePlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satelliteView) {
    populateBattleInfo(satelliteView);
    tank.updateBattleInfo(m_offensiveBattleInfo);
}

void OffensivePlayer::populateBattleInfo(SatelliteView& satelliteView) {
    // Use BasicPlayer's static parsing utility to populate offensive battle info directly
    BasicPlayer::parseSatelliteViewToBattleInfo(m_offensiveBattleInfo, satelliteView, 
                                                m_playerIndex, m_boardWidth, m_boardHeight);
    updateTarget();
}

void OffensivePlayer::updateTarget() {
    const auto& enemyTanks = m_offensiveBattleInfo.getEnemyTankPositions();
    bool targetDestroyed = true;
    if (m_currentTarget.has_value()) {
        // Check if the current target still exists
        for (const auto& pos : enemyTanks) {
            if (pos == m_currentTarget.value()) {
                targetDestroyed = false;
                break;
            }
        }
    }
    if (enemyTanks.empty()) {
        m_currentTarget = std::nullopt;
        m_offensiveBattleInfo.clearTargetTankPosition();
    } else if (!m_currentTarget.has_value() || targetDestroyed) {
        // Select new target: closest to last known target or to (0,0) if none
        Point reference = m_currentTarget.value_or(Point(0, 0));
        m_currentTarget = selectNewTarget(enemyTanks, reference);
        if (m_currentTarget.has_value()) {
            m_offensiveBattleInfo.setTargetTankPosition(m_currentTarget.value());
        } else {
            m_offensiveBattleInfo.clearTargetTankPosition();
        }
    } else {
        // Target still alive, keep it
        m_offensiveBattleInfo.setTargetTankPosition(m_currentTarget.value());
    }
}

std::optional<Point> OffensivePlayer::selectNewTarget(const std::vector<Point>& enemyTanks, const Point& reference) const {
    if (enemyTanks.empty()) return std::nullopt;
    double minDist = std::numeric_limits<double>::max();
    std::optional<Point> closest;
    for (const auto& pos : enemyTanks) {
        int dist = GameBoard::stepDistance(pos, reference, m_boardWidth, m_boardHeight);
        if (dist < minDist) {
            minDist = dist;
            closest = pos;
        }
    }
    return closest;
} 