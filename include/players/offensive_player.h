#pragma once

#include "players/basic_player.h"
#include "players/offensive_battle_info.h"
#include <optional>

/**
 * @class OffensivePlayer
 * @brief Player that coordinates all tanks to target a single enemy tank.
 *
 * Inherits from BasicPlayer and uses OffensiveBattleInfo to communicate
target information to all tanks. If the current target is destroyed, selects
the closest enemy tank to the last known target position.
 */
class OffensivePlayer : public BasicPlayer {
public:
    OffensivePlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells);
    ~OffensivePlayer() override;

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
