#pragma once

#include <string>
#include <vector>

#include "UserCommon/game_board.h"
#include "UserCommon/objects/shell.h"
#include "UserCommon/objects/tank.h"

/**
 * @brief Lightweight representation of Tank state for visualization
 */
struct TankState {
    int playerId;
    Point position;
    Direction direction;
    int remainingShells;
    bool destroyed;
    
    TankState() = default;
    explicit TankState(const Tank& tank);
};

/**
 * @brief Lightweight representation of Shell state for visualization
 */
struct ShellState {
    int playerId;
    Point position;
    Direction direction;
    bool destroyed;
    
    ShellState() = default;
    explicit ShellState(const Shell& shell);
};

/**
 * @brief Complete snapshot of game state at a point in time
 * 
 * This class captures all the necessary state information for visualizing
 * a single step of the game simulation.
 */
class GameSnapshot {
public:
    GameSnapshot() = default;
    
    /**
     * @brief Create a snapshot from current game state
     * 
     * @param step The step number in the game
     * @param board The current board state
     * @param tanks The tanks in the game
     * @param shells The shells in the game
     * @param countdown The countdown timer value (if active, -1 otherwise)
     * @param message Optional message describing this step
     */
    GameSnapshot(
        int step,
        const GameBoard& board,
        const std::vector<Tank>& tanks,
        const std::vector<Shell>& shells,
        int countdown = -1,
        const std::string& message = ""
    );
    
    // Accessors
    int getStepNumber() const { return m_stepNumber; }
    const std::vector<std::vector<GameBoard::CellType>>& getBoardState() const { return m_boardState; }
    const std::map<Point, int>& getWallHealth() const { return m_wallHealth; }
    const std::vector<TankState>& getTanks() const { return m_tanks; }
    const std::vector<ShellState>& getShells() const { return m_shells; }
    int getCountdown() const { return m_countdown; }
    const std::string& getMessage() const { return m_message; }
    
    /**
     * @brief Convert the snapshot to a JSON string
     * 
     * @return A JSON representation of the snapshot
     */
    std::string toJson() const;
    
private:
    int m_stepNumber = 0;
    std::vector<std::vector<GameBoard::CellType>> m_boardState;
    std::map<Point, int> m_wallHealth;
    std::vector<TankState> m_tanks;
    std::vector<ShellState> m_shells;
    int m_countdown = -1;
    std::string m_message;
};