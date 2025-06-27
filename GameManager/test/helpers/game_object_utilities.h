#pragma once

#include <vector>
#include <algorithm>
#include <gtest/gtest.h>

#include "UserCommon/objects/shell.h"
#include "UserCommon/objects/tank.h"
#include "UserCommon/utils/point.h"
#include "UserCommon/utils/direction.h"

namespace GameManager_098765432_123456789 {

using namespace UserCommon_098765432_123456789;

/**
 * @brief Utility class for creating and manipulating game objects in tests
 */
class GameObjectUtilities {
public:
    // ==================== SHELL UTILITIES ====================
    
    /**
     * @brief Create a shell with specified or default properties
     * @param playerId Player ID for the shell (default: 1)
     * @param position Position of the shell (default: (0,0))
     * @param direction Direction of the shell (default: Right)
     * @param isDestroyed Whether the shell should be destroyed (default: false)
     */
    static Shell createShell(int playerId = 1, 
                           Point position = Point(0, 0), 
                           Direction direction = Direction::Right,
                           bool isDestroyed = false) {
        Shell shell(playerId, position, direction);
        if (isDestroyed) {
            shell.destroy();
        }
        return shell;
    }
    
    /**
     * @brief Create a destroyed shell (convenience function)
     */
    static Shell createDestroyedShell(int playerId = 1, 
                                    Point position = Point(0, 0), 
                                    Direction direction = Direction::Right) {
        return createShell(playerId, position, direction, true);
    }
    
    /**
     * @brief Create multiple shells at different positions
     * @param positions Vector of positions for the shells
     * @param playerId Player ID for all shells (default: 1)
     * @param direction Direction for all shells (default: Right)
     * @param isDestroyed Whether all shells should be destroyed (default: false)
     */
    static std::vector<Shell> createShellsAtPositions(const std::vector<Point>& positions,
                                                    int playerId = 1,
                                                    Direction direction = Direction::Right,
                                                    bool isDestroyed = false) {
        std::vector<Shell> shells;
        shells.reserve(positions.size());
        for (const auto& position : positions) {
            shells.push_back(createShell(playerId, position, direction, isDestroyed));
        }
        return shells;
    }
    
    /**
     * @brief Verify that a shell matches expected properties
     */
    static void verifyShell(const Shell& actual, const Shell& expected, 
                          const std::string& context = "Shell") {
        EXPECT_EQ(actual.getPosition(), expected.getPosition())
            << context << " position mismatch";
        EXPECT_EQ(actual.getPlayerId(), expected.getPlayerId())
            << context << " player ID mismatch";
        EXPECT_EQ(actual.getDirection(), expected.getDirection())
            << context << " direction mismatch";
        EXPECT_EQ(actual.isDestroyed(), expected.isDestroyed())
            << context << " destroyed state mismatch";
    }
    
    // ==================== COLLECTION UTILITIES ====================
    
    /**
     * @brief Verify that a shell collection matches expected shells
     */
    template<typename Container1, typename Container2>
    static void verifyShellCollection(const Container1& actual, const Container2& expected) {
        ASSERT_EQ(actual.size(), expected.size()) << "Shell collection size mismatch";
        
        auto actualIt = actual.begin();
        auto expectedIt = expected.begin();
        size_t index = 0;
        
        while (actualIt != actual.end() && expectedIt != expected.end()) {
            verifyShell(*actualIt, *expectedIt, "Shell[" + std::to_string(index) + "]");
            ++actualIt;
            ++expectedIt;
            ++index;
        }
    }
    
    /**
     * @brief Count shells matching a predicate
     */
    template<typename Container, typename Predicate>
    static size_t countShells(const Container& shells, Predicate pred) {
        return std::count_if(shells.begin(), shells.end(), pred);
    }
    
    /**
     * @brief Count active (non-destroyed) shells
     */
    template<typename Container>
    static size_t countActiveShells(const Container& shells) {
        return countShells(shells, [](const Shell& s) { return !s.isDestroyed(); });
    }
    
    /**
     * @brief Count destroyed shells
     */
    template<typename Container>
    static size_t countDestroyedShells(const Container& shells) {
        return countShells(shells, [](const Shell& s) { return s.isDestroyed(); });
    }

private:
    GameObjectUtilities() = delete; // Static utility class only
};

} // namespace GameManager_098765432_123456789 