#pragma once

#include <optional>

#include "battle_info_impl.h"
#include "UserCommon/utils/point.h"

using namespace UserCommon_098765432_123456789;

namespace Algorithm_098765432_123456789 {

class OffensiveBattleInfo : public BattleInfoImpl {
public:
    OffensiveBattleInfo(int boardWidth, int boardHeight)
        : BattleInfoImpl(boardWidth, boardHeight) {}
    // --- Target Information ---
    void setTargetTankPosition(const Point& position);
    void clearTargetTankPosition();
    std::optional<Point> getTargetTankPosition() const;

private:
    std::optional<Point> m_target_tank_position;
};

} // namespace Algorithm_098765432_123456789