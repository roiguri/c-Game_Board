#pragma once

#include <string>
#include <vector>
#include <memory>
#include "game_board.h"
#include "objects/tank.h"
#include "objects/shell.h"
#include "algo/algorithm.h"
#include "collision_handler.h"
#include "TankAlgorithmFactory.h"
#include "PlayerFactory.h"
#include "ActionRequest.h"

#ifdef ENABLE_VISUALIZATION
#include "bonus/visualization/visualization.h"
#endif

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
class GameManager {
public:
    /**
     * @brief Construct a new Game Manager object with default state
     */
    GameManager(std::unique_ptr<PlayerFactory> playerFactory,
                std::unique_ptr<TankAlgorithmFactory> tankAlgorithmFactory);
    
    /**
     * @brief Destroy the Game Manager object and free resources
     */
    ~GameManager();

    // Delete copy constructor and assignment operator
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    /**
     * @brief Initialize the game with a board file
     * 
     * Loads the game board, creates tanks, and initializes algorithms
     * 
     * @param filePath Path to the board file
     * @return true if initialization was successful, false otherwise
     */
    bool readBoard(const std::string& filePath,
                   Algorithm* player1Algorithm = nullptr,
                   Algorithm* player2Algorithm = nullptr);
    
    /**
     * @brief Run the game loop until completion
     * 
     * Processes turns, applies actions, and checks for game end conditions
     */
    void run();

    struct TankWithAlgorithm {
        Tank& tank;
        std::unique_ptr<TankAlgorithm> algorithm;
        ActionRequest nextAction = ActionRequest::DoNothing;
        bool actionSuccess = false; // Track if last action was successful
        bool wasKilledInPreviousStep = false; // Track if killed this step
    };

private:
    // Players
    std::unique_ptr<Player> m_player1;
    std::unique_ptr<Player> m_player2;

    // Factories
    std::unique_ptr<PlayerFactory> m_PlayerFactory;
    std::unique_ptr<TankAlgorithmFactory> m_tankAlgorithmFactory;

    // Game state tracking
    int m_currentStep;
    bool m_gameOver;
    int m_remaining_steps;
    int m_maximum_steps;
    std::string m_gameResult;
    std::vector<std::string> m_gameLog;
    CollisionHandler m_collisionHandler;

    // Core game state
    GameBoard m_board;
    std::vector<Tank> m_tanks; // FIXME: remove once migration completes
    std::vector<Shell> m_shells;
    std::vector<TankWithAlgorithm> m_tankControllers;
    
    // Output file path
    std::string m_outputFilePath;

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
    
    // Log an action taken by a player
    void logAction();
    
    // Helper methods

    // Create algorithm instances for all tanks
    void createTankAlgorithms();

    // Create tank objects based on tank positions
    void createTanks(const std::vector<std::pair<int, Point>>& tankPositions);

    // Save recoverable errors to input_errors.txt file
    bool saveErrorsToFile(const std::vector<std::string>& errors) const;

    // Remove any destroyed shells from the game
    void removeDestroyedShells();

    // Save the game results to an output file
    bool saveResults(const std::string& outputFilePath);

    // Helper method to convert ActionRequest to string
    std::string actionToString(ActionRequest action);

    // For testing purposes
    friend class GameManagerTest;

    #ifdef ENABLE_VISUALIZATION
    std::unique_ptr<VisualizationManager> m_visualizationManager;
    #endif

    // TODO: Consider creating a GameState abstraction to encapsulate board, tanks, and shells
    GameBoard m_currentBoard;
    std::vector<Tank> m_currentTanks;
    std::vector<Shell> m_currentShells;
};