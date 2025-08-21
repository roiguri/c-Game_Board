#include "offensive_battle_info.h"

using namespace UserCommon_318835816_211314471;

namespace Algorithm_318835816_211314471 {

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

} // namespace Algorithm_318835816_211314471