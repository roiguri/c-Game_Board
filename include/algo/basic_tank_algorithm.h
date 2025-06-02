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
 * @brief A simple tank implementing the TankAlgorithm interface.
 *
 * This algorithm provides basic decision-making for a tank in the game.
 * It prioritizes survival by avoiding immediate threats (such as shells),
 * attempts to shoot at enemy tanks if they are in the line of sight and in the current direction,
 * and otherwise moves to the safest available adjacent position.
 *
 * Decision logic (in order of priority):
 *   1. If battle info is outdated, request an update.
 *   2. If the tank is in immediate danger from shells, move to a safe position.
 *   3. If any enemy tank is in the tank's current direction and line of sight, shoot.
 *   4. Otherwise, move to a safe position or do nothing if no safe moves exist.
 *
 * The algorithm uses helpers to evaluate safe positions, line of sight, and shell danger.
 * It does not coordinate with other tanks or perform advanced pathfinding.
 */
class BasicTankAlgorithm : public TankAlgorithm {
public:
    /**
     * @brief Constructor
     * 
     * Without further information the algorithm assumes a 5x5 board.
     * 
     * @param playerId The player ID this tank belongs to
     * @param tankIndex The index of the tank controlled by this algorithm
     */
    BasicTankAlgorithm(int playerId, int tankIndex);
    /**
     * @brief Destructor
     */
    ~BasicTankAlgorithm() override;

    /**
     * @brief Determines the next action for this tank based on the current state.
     *
     * Decision order:
     *   - Requests new battle info if outdated
     *   - Moves to safety if in danger from shells
     *   - Shoots if an enemy is in line of sight and current direction
     *   - Otherwise, moves to a safe position or does nothing
     *
     * @return ActionRequest for the next move
     */
    ActionRequest getAction() override;

    /**
     * @brief Updates the tank with new battle information from the game engine.
     * @param info The latest battle info (must be a BattleInfoImpl)
     */
    void updateBattleInfo(BattleInfo& info) override;

protected:
    int m_playerId;
    int m_tankIndex;
    int m_turnsSinceLastUpdate = 0;

    // State tracking for position, direction, shells, and cooldown
    Point m_trackedPosition;
    Direction m_trackedDirection;
    int m_trackedShells = Tank::INITIAL_SHELLS;
    int m_trackedCooldown = 0;

    // Known game state
    GameBoard m_gameBoard;
    std::vector<Point> m_enemyTanks;
    std::vector<Point> m_friendlyTanks;
    std::vector<Point> m_shells;

    /**
     * @struct SafeMoveOption
     * @brief Represents a possible move to a safe position, with associated action and cost.
     */
    struct SafeMoveOption {
        Point position;
        ActionRequest action;
        int cost;
        Direction direction;
        bool operator<(const SafeMoveOption& other) const { return cost < other.cost; }
    };

    /**
     * @brief Computes the best move option to reach a given position from the current tank state.
     * @param pos The target position
     * @return SafeMoveOption describing the move and its cost
     */
    SafeMoveOption getSafeMoveOption(const Point& pos) const;

    /**
     * @brief Computes move options for a list of positions.
     * @param positions List of target positions
     * @return Vector of SafeMoveOption for each position
     */
    std::vector<SafeMoveOption> getSafeMoveOptions(const std::vector<Point>& positions) const;

    /**
     * @brief Checks if any enemy tank is in line of sight and in the current direction.
     * @return true if the tank can shoot an enemy, false otherwise
     */
    bool canShootEnemy() const;

    /**
     * @brief Gets the next action for this tank to reach a safe position.
     *        If already safe, returns DoNothing.
     * @return ActionRequest for the next move
     */
    ActionRequest getActionToSafePosition() const;

    /**
     * @brief Checks if the given position is in immediate danger from shells (distance < 3).
     * @param position The position to check
     * @return true if in danger, false otherwise
     */
    bool isInDangerFromShells(const Point& position) const;

    /**
     * @brief Checks if the tank's current position is in immediate danger from shells (distance < 3).
     * @return true if in danger, false otherwise
     */
    bool isInDangerFromShells() const;

    /**
     * @brief Checks if a position is safe (no walls, tanks, mines, or shell danger).
     * @param position The position to check
     * @return true if safe, false otherwise
     */
    bool isPositionSafe(const Point& position) const;

    /**
     * @brief Gets all adjacent safe positions for the tank.
     * @return Vector of safe Point positions
     */
    std::vector<Point> getSafePositions() const;

    /**
     * @brief Returns the direction if there is a line of sight from 'from' to 'to', else std::nullopt.
     *        Considers all 8 directions.
     * @param from The starting point
     * @param to The target point
     * @return Direction if line of sight exists, otherwise std::nullopt
     */
    std::optional<Direction> getLineOfSightDirection(const Point& from, const Point& to) const;

    /**
     * @brief Checks if there is a line of sight from 'from' to 'to' in the given direction.
     *        Returns false if a wall blocks the path.
     * @param from The starting point
     * @param to The target point
     * @param direction The direction to check
     * @return true if line of sight exists, false otherwise
     */
    bool checkLineOfSightInDirection(const Point& from, const Point& to, Direction direction) const;

    /**
     * @brief Checks if there is a tank (enemy or friendly) at the specified position.
     * @param position The position to check
     * @return true if a tank is at the position, false otherwise
     */
    bool isTankAtPosition(const Point& position) const;

    /**
     * @brief Update the tracked position, direction, shells, and cooldown based on the last action.
     * @param lastAction The action that was just taken
     */
    void updateState(ActionRequest lastAction);

    /**
     * @brief Gets optimal rotation action to face target direction.
     * @param current The current direction
     * @param target The target direction
     * @return The ActionRequest needed to rotate toward the target
     */
    static ActionRequest getRotationToDirection(Direction current, Direction target);

    friend class BasicTankAlgorithmTest;
}; 