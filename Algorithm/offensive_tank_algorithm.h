#pragma once

#include <optional>
#include <vector>

#include "basic_tank_algorithm.h"
#include "offensive_battle_info.h"

using namespace UserCommon_098765432_123456789;

namespace Algorithm_098765432_123456789 {

/**
 * @class TankAlgorithm_098765432_123456789_A
 * @brief Tank algorithm that chases and attacks a designated target tank.
 *
 * Priorities:
 *   1. Update battle info if necessary
 *   2. Avoid if in danger (from shells)
 *   3. Shoot if possible
 *   4. Turn to shoot if in line of sight
 *   5. Chase using BFS
 */
class TankAlgorithm_098765432_123456789_A : public BasicTankAlgorithm {
public:
    TankAlgorithm_098765432_123456789_A(int playerId, int tankIndex);
    ~TankAlgorithm_098765432_123456789_A() override;

    void updateBattleInfo(BattleInfo& info) override;
    ActionRequest getAction() override;

private:
    // Most recent target tank position
    std::optional<Point> m_targetPosition;
    // Current BFS path to target
    std::vector<Point> m_currentPath;
    // Previous target tank position
    std::optional<Point> m_previousTargetPosition;

    // --- Helpers ---
    /**
     * @brief Returns the turn action needed to align with the target, if possible.
     */
    std::optional<ActionRequest> turnToShootAction() const;

    /**
     * @brief Recalculates the BFS path to the target if needed.
     */
    void updatePathToTarget();

    /**
     * @brief Returns the next action to follow the current BFS path.
     */
    std::optional<ActionRequest> followCurrentPath();

    /**
     * @brief Finds the shortest path to the target using BFS, considering board wrapping and obstacles.
     */
    std::vector<Point> findPathBFS(const Point& start, const Point& target) const;

    /**
     * @brief Checks if the first step of the current path is valid.
     */
    bool isFirstStepValid() const;

    /**
     * @brief Checks if the tank is off the current path.
     */
    bool isTankOffPath() const;

    friend class OffensiveTankAlgorithmTest;
};

} // namespace Algorithm_098765432_123456789
