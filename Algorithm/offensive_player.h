#pragma once

#include <optional>

#include "basic_player.h"
#include "offensive_battle_info.h"

using namespace UserCommon_318835816_211314471;

namespace Algorithm_318835816_211314471 {

/**
 * @class Player_318835816_211314471_A
 * @brief Player that coordinates all tanks to target a single enemy tank.
 *
 * Inherits from BasicPlayer and uses OffensiveBattleInfo to communicate
 * target information to all tanks. If the current target is destroyed, selects
 * the closest enemy tank to the last known target position.
 */
class Player_318835816_211314471_A : public BasicPlayer {
public:
    Player_318835816_211314471_A(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells);
    ~Player_318835816_211314471_A() override;

    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satelliteView) override;

protected:
    /**
     * @brief Populates the offensive battle info from satellite view and manages target selection.
     *
     * @param satelliteView The satellite view to query
     */
    void populateBattleInfo(SatelliteView& satelliteView) override;

    /**
     * @brief Updates the current target tank and sets it in the battle info.
     */
    void updateTarget();

    /**
     * @brief Selects a new target tank based on current enemy positions.
     *
     * @param enemyTanks The list of enemy tank positions
     * @param reference The reference point for closest search
     * @return The selected target position, or std::nullopt if none
     */
    std::optional<Point> selectNewTarget(const std::vector<Point>& enemyTanks, const Point& reference) const;

    /**
     * @brief The last known target tank position (if any)
     */
    std::optional<Point> m_currentTarget;

    /**
     * @brief The offensive battle info object
     */
    OffensiveBattleInfo m_offensiveBattleInfo;
};

} // namespace Algorithm_318835816_211314471
