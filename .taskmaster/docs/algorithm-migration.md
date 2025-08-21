# Algorithm Adaptation Plan for Assignment 3

## **Overview**
Adapt the existing OffensivePlayer and OffensiveTankAlgorithm classes to meet Assignment 3 requirements by adding namespace wrappers, renaming classes, and setting up registration system.

## **Phase 1: Add Namespaces and Rename Classes (15 minutes)**

### **File: Algorithm/players/offensive/offensive_player.h**

**Changes:**
1. Add namespace wrapper around the class
2. Rename `OffensivePlayer` → `Player_098765432_123456789_A`
3. Add UserCommon namespace usage

```cpp
#pragma once

#include <optional>

#include "Algorithm/players/basic/basic_player.h"
#include "Algorithm/players/offensive/offensive_battle_info.h"

using namespace UserCommon_318835816_211314471;

namespace Algorithm_098765432_123456789 {

/**
 * @class Player_098765432_123456789_A
 * @brief Player that coordinates all tanks to target a single enemy tank.
 *
 * Inherits from BasicPlayer and uses OffensiveBattleInfo to communicate
 * target information to all tanks. If the current target is destroyed, selects
 * the closest enemy tank to the last known target position.
 */
class Player_098765432_123456789_A : public BasicPlayer {
public:
    Player_098765432_123456789_A(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells);
    ~Player_098765432_123456789_A() override;

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

} // namespace Algorithm_098765432_123456789
```

### **File: Algorithm/players/offensive/offensive_player.cpp**

**Changes:**
1. Add namespace wrapper around implementation
2. Update constructor and method names
3. Add registration macro at end

```cpp
#include <cmath>
#include <limits>

#include "Algorithm/players/offensive/offensive_player.h"

using namespace UserCommon_318835816_211314471;

namespace Algorithm_098765432_123456789 {

Player_098765432_123456789_A::Player_098765432_123456789_A(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
    : BasicPlayer(playerIndex, x, y, maxSteps, numShells),
      m_offensiveBattleInfo(static_cast<int>(x), static_cast<int>(y)) {}

Player_098765432_123456789_A::~Player_098765432_123456789_A() = default;

void Player_098765432_123456789_A::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satelliteView) {
    populateBattleInfo(satelliteView);
    tank.updateBattleInfo(m_offensiveBattleInfo);
}

void Player_098765432_123456789_A::populateBattleInfo(SatelliteView& satelliteView) {
    // Use BasicPlayer's static parsing utility to populate offensive battle info directly
    BasicPlayer::parseSatelliteViewToBattleInfo(m_offensiveBattleInfo, satelliteView, 
                                                m_playerIndex, m_boardWidth, m_boardHeight);
    updateTarget();
}

void Player_098765432_123456789_A::updateTarget() {
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

std::optional<Point> Player_098765432_123456789_A::selectNewTarget(const std::vector<Point>& enemyTanks, const Point& reference) const {
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

} // namespace Algorithm_098765432_123456789

// Registration at global scope
using namespace Algorithm_098765432_123456789;
REGISTER_PLAYER(Player_098765432_123456789_A);
```

### **File: Algorithm/players/offensive/offensive_tank_algorithm.h**

**Changes:**
1. Add namespace wrapper
2. Rename `OffensiveTankAlgorithm` → `TankAlgorithm_098765432_123456789_A`
3. Add UserCommon namespace usage

```cpp
#pragma once

#include <optional>
#include <vector>

#include "Algorithm/players/basic/basic_tank_algorithm.h"
#include "Algorithm/players/offensive/offensive_battle_info.h"

using namespace UserCommon_318835816_211314471;

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
```

### **File: Algorithm/players/offensive/offensive_tank_algorithm.cpp**

**Changes:**
1. Add namespace wrapper around implementation
2. Update constructor and method names
3. Add registration macro at end

```cpp
#include <algorithm>
#include <queue>
#include <unordered_set>

#include "Algorithm/players/offensive/offensive_tank_algorithm.h"

using namespace UserCommon_318835816_211314471;

namespace Algorithm_098765432_123456789 {

TankAlgorithm_098765432_123456789_A::TankAlgorithm_098765432_123456789_A(int playerId, int tankIndex)
    : BasicTankAlgorithm(playerId, tankIndex) {}

TankAlgorithm_098765432_123456789_A::~TankAlgorithm_098765432_123456789_A() = default;

void TankAlgorithm_098765432_123456789_A::updateBattleInfo(BattleInfo& info) {
    // Call parent updateBattleInfo first
    BasicTankAlgorithm::updateBattleInfo(info);
    
    // Try to downcast to OffensiveBattleInfo
    auto* offensiveInfo = dynamic_cast<OffensiveBattleInfo*>(&info);
    if (offensiveInfo) {
        auto newTarget = offensiveInfo->getTargetTankPosition();
        if (newTarget != m_targetPosition) {
            m_previousTargetPosition = m_targetPosition;
            m_targetPosition = newTarget;
            m_currentPath.clear(); // Force path recalculation
        }
    }
}

ActionRequest TankAlgorithm_098765432_123456789_A::getAction() {
    m_turnsSinceLastUpdate++;
    
    // 1. Check if BattleInfo is outdated
    if (m_turnsSinceLastUpdate > 3) {
        return ActionRequest::GetBattleInfo;
    }
    
    // 2. Check if in danger from shells
    if (isInDangerFromShells()) {
        ActionRequest safeAction = getActionToSafePosition();
        if (safeAction != ActionRequest::DoNothing) {
            return safeAction;
        }
    }
    
    // 3. Try to shoot if enemy in line of sight
    if (canShootEnemyInCurrentDirection()) {
        return ActionRequest::Shoot;
    }
    
    // 4. Try to turn to shoot if target in line of sight
    if (m_targetPosition.has_value()) {
        auto turnAction = turnToShootAction();
        if (turnAction.has_value()) {
            return turnAction.value();
        }
    }
    
    // 5. Follow BFS path to target
    if (m_targetPosition.has_value()) {
        updatePathToTarget();
        auto pathAction = followCurrentPath();
        if (pathAction.has_value()) {
            return pathAction.value();
        }
    }
    
    // 6. Fallback: move to safe position
    ActionRequest safeAction = getActionToSafePosition();
    return (safeAction != ActionRequest::DoNothing) ? safeAction : ActionRequest::DoNothing;
}

// ... rest of the implementation methods stay the same but with updated class name

} // namespace Algorithm_098765432_123456789

// Registration at global scope
using namespace Algorithm_098765432_123456789;
REGISTER_TANK_ALGORITHM(TankAlgorithm_098765432_123456789_A);
```

## **Phase 2: Update Build Configuration (15 minutes)**

### **File: Algorithm/CMakeLists.txt**

**Key changes:**
1. Change library name to `Algorithm_098765432_123456789`
2. Ensure shared library output: `Algorithm_098765432_123456789.so`
3. Add proper compiler flags for shared library

## **Phase 3: Verification (10 minutes)**

### **Build Test:**
```bash
cd Algorithm
make clean
make
# Should produce: Algorithm_098765432_123456789.so
```

### **Quick Checklist:**
- ✅ Files compile without errors
- ✅ Shared library (.so) is created with correct name
- ✅ Registration macros are at global scope (outside namespace)
- ✅ Class names follow assignment convention
- ✅ All UserCommon references use the namespace

## **Key Notes**

- **BasicPlayer and BasicTankAlgorithm**: Keep original names (they're helper classes)
- **OffensiveBattleInfo**: Keep original name (helper class)
- **Only rename**: Final classes that get registered
- **UserCommon namespace**: Use `using namespace UserCommon_318835816_211314471;`
- **Registration**: Must be at global scope, outside any namespace

## **Total Time Estimate: ~40 minutes**

This plan maintains your existing inheritance structure while meeting all Assignment 3 requirements for namespace organization and dynamic loading.