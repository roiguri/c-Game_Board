#include "players/offensive_player.h"
#include <limits>
#include <cmath>

OffensivePlayer::OffensivePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : BasicPlayer(player_index, x, y, max_steps, num_shells),
      m_offensiveBattleInfo(static_cast<int>(x), static_cast<int>(y)) {}

OffensivePlayer::~OffensivePlayer() = default;

void OffensivePlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    populateBattleInfo(satellite_view);
    tank.updateBattleInfo(m_offensiveBattleInfo);
}

void OffensivePlayer::populateBattleInfo(SatelliteView& satellite_view) {
    m_offensiveBattleInfo.clear();
    // Use BasicPlayer's logic to populate the board, tanks, shells, etc.
    for (size_t y = 0; y < m_board_height; ++y) {
        for (size_t x = 0; x < m_board_width; ++x) {
            char obj = satellite_view.getObjectAt(x, y);
            GameBoard::CellType cellType = GameBoard::CellType::Empty;
            switch (obj) {
                case '#':
                    cellType = GameBoard::CellType::Wall;
                    break;
                case '@':
                    cellType = GameBoard::CellType::Mine;
                    break;
                case '%':
                    cellType = GameBoard::CellType::Empty;
                    m_offensiveBattleInfo.setOwnTankPosition(Point(x, y));
                    break;
                case '*':
                    cellType = GameBoard::CellType::Empty;
                    m_offensiveBattleInfo.addShellPosition(Point(x, y));
                    break;
                case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                    cellType = GameBoard::CellType::Empty;
                    int tankPlayerId = obj - '0';
                    if (tankPlayerId == m_player_index) {
                        m_offensiveBattleInfo.addFriendlyTankPosition(Point(x, y));
                    } else {
                        m_offensiveBattleInfo.addEnemyTankPosition(Point(x, y));
                    }
                    break;
                }
                case ' ':
                default:
                    cellType = GameBoard::CellType::Empty;
                    break;
            }
            m_offensiveBattleInfo.setCellType(x, y, cellType);
        }
    }
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
        int dist = GameBoard::stepDistance(pos, reference, m_board_width, m_board_height);
        if (dist < minDist) {
            minDist = dist;
            closest = pos;
        }
    }
    return closest;
} 