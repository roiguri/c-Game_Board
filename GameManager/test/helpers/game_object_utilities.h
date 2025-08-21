#pragma once

#include <vector>
#include <algorithm>
#include <gtest/gtest.h>

#include "UserCommon/objects/shell.h"
#include "UserCommon/objects/tank.h"
#include "UserCommon/utils/point.h"
#include "UserCommon/utils/direction.h"

namespace GameManager_318835816_211314471 {

using namespace UserCommon_318835816_211314471;

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

    // ==================== TANK UTILITIES ====================
    
    /**
     * @brief Get initial direction for a player ID (matches GameManager logic)
     */
    static Direction getInitialDirection(int playerId) {
        // Only support 2 players
        if (playerId == 1) {
            return Direction::Left;
        } else if (playerId == 2) {
            return Direction::Right;
        }
        return Direction::Left; // Default fallback
    }
    
    /**
     * @brief Create a tank with specified or default properties
     * Uses getInitialDirection to determine direction based on player ID
     * @param playerId Player ID for the tank (default: 1)
     * @param position Position of the tank (default: (0,0))
     * @param isDestroyed Whether the tank should be destroyed (default: false)
     */
    static Tank createTank(int playerId = 1, 
                          Point position = Point(0, 0), 
                          bool isDestroyed = false) {
        Direction direction = getInitialDirection(playerId);
        Tank tank(playerId, position, direction);
        if (isDestroyed) {
            tank.destroy();
        }
        return tank;
    }
    
    /**
     * @brief Create a destroyed tank (convenience function)
     */
    static Tank createDestroyedTank(int playerId = 1, 
                                   Point position = Point(0, 0)) {
        return createTank(playerId, position, true);
    }
    
    /**
     * @brief Create multiple tanks at different positions
     * @param positions Vector of positions for the tanks
     * @param playerId Player ID for all tanks (default: 1)
     * @param isDestroyed Whether all tanks should be destroyed (default: false)
     */
    static std::vector<Tank> createTanksAtPositions(const std::vector<Point>& positions,
                                                   int playerId = 1,
                                                   bool isDestroyed = false) {
        std::vector<Tank> tanks;
        tanks.reserve(positions.size());
        for (const auto& position : positions) {
            tanks.push_back(createTank(playerId, position, isDestroyed));
        }
        return tanks;
    }
    
    /**
     * @brief Create tanks from player ID and position pairs
     * @param playerPositions Vector of pairs containing (playerId, position)
     * @param isDestroyed Whether all tanks should be destroyed (default: false)
     */
    static std::vector<Tank> createTanksFromPlayerPositions(const std::vector<std::pair<int, Point>>& playerPositions,
                                                           bool isDestroyed = false) {
        std::vector<Tank> tanks;
        tanks.reserve(playerPositions.size());
        for (const auto& [playerId, position] : playerPositions) {
            tanks.push_back(createTank(playerId, position, isDestroyed));
        }
        return tanks;
    }
    
    /**
     * @brief Verify that a tank matches expected properties
     */
    static void verifyTank(const Tank& actual, const Tank& expected, 
                          const std::string& context = "Tank") {
        EXPECT_EQ(actual.getPosition(), expected.getPosition())
            << context << " position mismatch";
        EXPECT_EQ(actual.getPlayerId(), expected.getPlayerId())
            << context << " player ID mismatch";
        EXPECT_EQ(actual.getDirection(), expected.getDirection())
            << context << " direction mismatch";
        EXPECT_EQ(actual.isDestroyed(), expected.isDestroyed())
            << context << " destroyed state mismatch";
        EXPECT_EQ(actual.getRemainingShells(), expected.getRemainingShells())
            << context << " remaining shells mismatch";
    }
    
    /**
     * @brief Verify that a tank collection matches expected tanks
     */
    template<typename Container1, typename Container2>
    static void verifyTankCollection(const Container1& actual, const Container2& expected) {
        ASSERT_EQ(actual.size(), expected.size()) << "Tank collection size mismatch";
        
        auto actualIt = actual.begin();
        auto expectedIt = expected.begin();
        size_t index = 0;
        
        while (actualIt != actual.end() && expectedIt != expected.end()) {
            verifyTank(*actualIt, *expectedIt, "Tank[" + std::to_string(index) + "]");
            ++actualIt;
            ++expectedIt;
            ++index;
        }
    }
    
    /**
     * @brief Count tanks matching a predicate
     */
    template<typename Container, typename Predicate>
    static size_t countTanks(const Container& tanks, Predicate pred) {
        return std::count_if(tanks.begin(), tanks.end(), pred);
    }
    
    /**
     * @brief Count alive (non-destroyed) tanks
     */
    template<typename Container>
    static size_t countAliveTanks(const Container& tanks) {
        return countTanks(tanks, [](const Tank& t) { return !t.isDestroyed(); });
    }
    
    /**
     * @brief Count destroyed tanks
     */
    template<typename Container>
    static size_t countDestroyedTanks(const Container& tanks) {
        return countTanks(tanks, [](const Tank& t) { return t.isDestroyed(); });
    }
    
    /**
     * @brief Count tanks by player ID
     */
    template<typename Container>
    static size_t countTanksByPlayer(const Container& tanks, int playerId) {
        return countTanks(tanks, [playerId](const Tank& t) { return t.getPlayerId() == playerId; });
    }

private:
    GameObjectUtilities() = delete; // Static utility class only
};

} // namespace GameManager_318835816_211314471 