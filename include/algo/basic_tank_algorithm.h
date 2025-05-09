#pragma once

#include "TankAlgorithm.h"
#include "players/battle_info_impl.h"
#include "game_board.h"
#include "utils/point.h"
#include <vector>
#include <objects/tank.h>
#include <optional>
#include <functional>

/**
 * @class BasicTankAlgorithm
 * @brief Basic implementation of TankAlgorithm for simple tank behavior
 *
 * Decides actions based on simple rules using provided battle info.
 */
class BasicTankAlgorithm : public TankAlgorithm {
public:
    /**
     * @brief Constructor
     * @param playerId The player ID this tank belongs to
     * @param tankIndex The index of the tank
     */
    BasicTankAlgorithm(int playerId, int tankIndex);
    /**
     * @brief Destructor
     */
    ~BasicTankAlgorithm() override;

    /**
     * @brief Gets the next action for this tank
     * @return ActionRequest for the next move
     */
    ActionRequest getAction() override;

    /**
     * @brief Updates the tank with new battle information
     * @param info The latest battle info
     */
    void updateBattleInfo(BattleInfo& info) override;

    /**
     * @brief Sets the tank reference for this algorithm
     * @param tank The tank to associate
     */
    void setTank(Tank& tank);

private:
    int m_playerId;
    int m_tankIndex;
    int m_turnsSinceLastUpdate = 0;

    // Known game state
    // TODO: consider changing to a regular reference
    std::optional<std::reference_wrapper<Tank>> m_tank;
    GameBoard m_gameBoard;
    std::vector<Point> m_enemyTanks;
    std::vector<Point> m_friendlyTanks;
    std::vector<Point> m_shells;

    bool canShootEnemy() const;

    // === Helper Methods ===
    /**
     * @brief Checks if the tank is in immediate danger from shells (distance < 3)
     * @return true if in danger, false otherwise
     */
    bool isInDangerFromShells() const;

    /**
     * @brief Checks if a position is safe (no walls, tanks, mines, or shell danger)
     * @param position The position to check
     * @return true if safe, false otherwise
     */
    bool isPositionSafe(const Point& position) const;

    /**
     * @brief Gets all adjacent safe positions for the tank
     * @return Vector of safe Point positions
     */
    std::vector<Point> getSafePositions() const;

    /**
     * @brief Finds the best action to move to a safe position
     * @param safePositions List of safe positions
     * @return ActionRequest to reach a safe position
     */
    ActionRequest getActionToSafePosition(const std::vector<Point>& safePositions) const;

    /**
     * @brief Returns the direction if there is a line of sight from 'from' to 'to', else std::nullopt
     */
    std::optional<Direction> getLineOfSightDirection(const Point& from, const Point& to) const;

    /**
     * @brief Checks if there is a line of sight from 'from' to 'to' in the given direction
     */
    bool checkLineOfSightInDirection(const Point& from, const Point& to, Direction direction) const;

    friend class BasicTankAlgorithmTest;
}; 