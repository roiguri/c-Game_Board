#pragma once

#include "players/basic/battle_info_impl.h"
#include "utils/point.h"
#include <optional>

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