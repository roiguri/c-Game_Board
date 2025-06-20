#pragma once

#include "players/basic/basic_tank_algorithm.h"
#include "players/offensive/offensive_battle_info.h"
#include <optional>
#include <vector>

/**
 * @class OffensiveTankAlgorithm
 * @brief Tank algorithm that chases and attacks a designated target tank.
 *
 * Priorities:
 *   1. Update battle info if necessary
 *   2. Avoid if in danger (from shells)
 *   3. Shoot if possible
 *   4. Turn to shoot if in line of sight
 *   5. Chase using BFS
 */
class OffensiveTankAlgorithm : public BasicTankAlgorithm {
public:
    OffensiveTankAlgorithm(int playerId, int tankIndex);
    ~OffensiveTankAlgorithm() override;

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
