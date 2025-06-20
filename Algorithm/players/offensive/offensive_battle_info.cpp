#include "players/offensive/offensive_battle_info.h"

// --- Target Information Implementation ---

void OffensiveBattleInfo::setTargetTankPosition(const Point& position) {
    m_target_tank_position = position;
}

void OffensiveBattleInfo::clearTargetTankPosition() {
    m_target_tank_position = std::nullopt;
}

std::optional<Point> OffensiveBattleInfo::getTargetTankPosition() const {
    return m_target_tank_position;
}