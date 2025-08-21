#pragma once

#include <memory>
#include <string>
#include <vector>

#include "collision_handler.h"
#include "game_board.h"
#include "objects/shell.h"
#include "objects/tank.h"
#include "common/AbstractGameManager.h"
#include "common/GameResult.h"
#include "common/SatelliteView.h"
#include "common/ActionRequest.h"
#include "common/Player.h"
#include "common/TankAlgorithm.h"

#ifdef ENABLE_VISUALIZATION
#include "bonus/visualization/visualization.h"
#endif

namespace GameManager_318835816_211314471 {

using namespace UserCommon_098765432_123456789;

/**
 * @brief Manages the overall game flow and state for the tank battle simulation
 * 
 * The GameManager is responsible for:
 * - Loading the game board from an input file
 * - Managing the game loop and state
 * - Coordinating tank actions through algorithms
 * - Handling movement, collisions, and game rules
 * - Tracking game history and saving results
 */
class MyGameManager_318835816_211314471 : public AbstractGameManager {
public:
    /** Default number of steps allowed when tanks run out of shells */
    static constexpr int DEFAULT_NO_SHELLS_STEPS = 40;

    /**
     * @brief Construct a new MyGameManager_318835816_211314471 object with default state
     * @param verbose Whether to generate output files (default: false)
     */
    MyGameManager_318835816_211314471(bool verbose = false);
    
    /**
     * @brief Destroy the MyGameManager_318835816_211314471 object and free resources
     */
    ~MyGameManager_318835816_211314471();

    MyGameManager_318835816_211314471(const MyGameManager_318835816_211314471&) = delete;
    MyGameManager_318835816_211314471& operator=(const MyGameManager_318835816_211314471&) = delete;
    MyGameManager_318835816_211314471(MyGameManager_318835816_211314471&&) = default;
    MyGameManager_318835816_211314471& operator=(MyGameManager_318835816_211314471&&) = default;

    /**
     * @brief Initialize the game with SatelliteView input and provided players
     * 
     * Loads the game board from SatelliteView, sets up tanks, and initializes algorithms
     * 
     * @param satellite_view The SatelliteView to read from
     * @param map_width Width of the map
     * @param map_height Height of the map
     * @param max_steps Maximum number of steps allowed
     * @param num_shells Number of shells per tank
     * @param player1_factory Factory for creating player 1 tank algorithms
     * @param player2_factory Factory for creating player 2 tank algorithms
     * @return true if initialization was successful, false otherwise
     */
    bool readBoard(const SatelliteView& satellite_view, size_t map_width, size_t map_height, 
                   size_t max_steps, size_t num_shells,
                   TankAlgorithmFactory player1_factory, TankAlgorithmFactory player2_factory);
    
    /**
     * @brief Run the game loop until completion
     * 
     * Processes turns, applies actions, and checks for game end conditions
     */
    GameResult run(
        size_t map_width, size_t map_height,
        const SatelliteView& map, // <= assume it is a snapshot, NOT updated
        string map_name,
        size_t max_steps, size_t num_shells,
        Player& player1, string name1,
        Player& player2, string name2,
        TankAlgorithmFactory player1_tank_algo_factory,
        TankAlgorithmFactory player2_tank_algo_factory) override;

    struct TankWithAlgorithm {
        Tank& tank;
        std::unique_ptr<TankAlgorithm> algorithm;
        ActionRequest nextAction = ActionRequest::DoNothing;
        bool actionSuccess = false; // Track if last action was successful
        bool wasKilledInPreviousStep = false; // Track if killed this step
    };

private:
    // Player structure with ID
    struct PlayerWithId {
        int playerId;
        Player& player;
        
        PlayerWithId(int id, Player& p) : playerId(id), player(p) {}
    };
    
    // Players
    std::vector<PlayerWithId> m_players;

    // Game state tracking
    int m_currentStep;
    bool m_gameOver;
    int m_remaining_steps;
    int m_maximum_steps;
    std::string m_gameResult;
    GameResult m_finalGameResult;
    std::vector<std::string> m_gameLog;
    CollisionHandler m_collisionHandler;

    // Core game state
    GameBoard m_board;
    std::vector<Tank> m_tanks;
    std::vector<Shell> m_shells;
    std::vector<TankWithAlgorithm> m_tankControllers;
    
    // Verbose mode flag
    bool m_verbose;

    // Names for output file generation
    std::string m_mapName;
    std::string m_player1Name;
    std::string m_player2Name;

    // Current game state
    GameBoard m_currentBoard;
    std::vector<Tank> m_currentTanks;
    std::vector<Shell> m_currentShells;

    // Game step methods
    // Process a single step of the game
    void processStep();
    
    // Apply an action for a player
    // Validates and executes the action, updating the game state
    void applyAction(TankWithAlgorithm& controller);
    
    // Move all active shells one cell in their direction
    void moveShellsOnce();
    
    // Check if the game has ended
    // Determines if a tank was destroyed or other end conditions have been met
    bool checkGameOver();
    
    // Helper method to populate GameResult with common fields
    void populateGameResult(int winner, GameResult::Reason reason, const std::vector<size_t>& remainingTanks);
    
    // Log an action taken by a player
    std::string logAction();
    
    // Create algorithm instances for all tanks using provided factories
    void createTankAlgorithms(TankAlgorithmFactory player1_factory, TankAlgorithmFactory player2_factory);

    // Create tank objects based on tank positions
    void createTanks(const std::vector<std::pair<int, Point>>& tankPositions);

    // Get initial direction for a player based on player ID
    Direction getInitialDirection(int playerId);

    // Remove any destroyed shells from the game
    void removeDestroyedShells();

    // Save the game results to an output file (only when verbose=true)
    bool saveResults();

    // Convert ActionRequest to string
    std::string actionToString(ActionRequest action);

    // Generate unique output file path with timestamp and player names
    std::string generateOutputFilePath(const std::string& player1Name, const std::string& player2Name);
    
    // Clean filename to remove invalid characters for file system
    std::string cleanFilename(const std::string& name);

    // Extract base name from full file path (remove directory path only)
    std::string extractBaseName(const std::string& fullPath);

    // Read character grid from SatelliteView and convert to vector<string> format
    std::vector<std::string> readSatelliteView(const SatelliteView& satellite_view, 
                                               size_t map_width, 
                                               size_t map_height) const;

    // For testing purposes
    friend class GameManagerTest;

    #ifdef ENABLE_VISUALIZATION
    std::unique_ptr<VisualizationManager> m_visualizationManager;
    #endif
};

} // namespace GameManager_318835816_211314471
