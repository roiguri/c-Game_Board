#pragma once

#include <string>
#include <vector>
#include <memory>
#include "game_board.h"
#include "tank.h"
#include "shell.h"
#include "algo/algorithm.h"

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
    GameManager();
    
    /**
     * @brief Destroy the Game Manager object and free resources
     */
    ~GameManager();

    /**
     * @brief Initialize the game with a board file
     * 
     * Loads the game board, creates tanks, and initializes algorithms
     * 
     * @param filePath Path to the board file
     * @return true if initialization was successful, false otherwise
     */
    bool initialize(const std::string& filePath);
    
    /**
     * @brief Run the game loop until completion
     * 
     * Processes turns, applies actions, and checks for game end conditions
     */
    void runGame();
    
    /**
     * @brief Save the game results to an output file
     * 
     * @param outputFilePath Path to the output file
     */
    void saveResults(const std::string& outputFilePath);

private:
    // Core game state
    GameBoard m_board;
    std::vector<Tank> m_tanks;
    std::vector<Shell> m_shells;
    
    // Algorithms
    Algorithm* m_player1Algorithm;
    Algorithm* m_player2Algorithm;
    
    // Game state tracking
    int m_currentStep;
    bool m_gameOver;
    std::string m_gameResult;
    std::vector<std::string> m_gameLog;
    
    // Game step methods
    /**
     * @brief Process a single step of the game
     * 
     * Gets actions from algorithms, applies them, moves shells, and checks for game over
     */
    void processStep();
    
    /**
     * @brief Get the next action for a player from their algorithm
     * 
     * @param playerId The ID of the player (1 or 2)
     * @return Action The chosen action
     */
    Action getPlayerAction(int playerId);
    
    /**
     * @brief Apply an action for a player
     * 
     * Validates and executes the action, updating the game state
     * 
     * @param playerId The ID of the player (1 or 2)
     * @param action The action to apply
     * @return true if the action was valid and applied, false otherwise
     */
    bool applyAction(int playerId, Action action);
    
    /**
     * @brief Move all active shells according to their velocity
     * 
     * Updates shell positions considering board wrapping
     */
    void moveShells();
    
    /**
     * @brief Check if the game has ended
     * 
     * Determines if a tank was destroyed or other end conditions have been met
     * 
     * @return true if the game is over, false otherwise
     */
    bool checkGameOver();
    
    /**
     * @brief Log an action taken by a player
     * 
     * @param playerId The ID of the player (1 or 2)
     * @param action The action taken
     * @param valid Whether the action was valid
     */
    void logAction(int playerId, Action action, bool valid);
    
    // Helper methods
    /**
     * @brief Create algorithm instances for both players
     */
    void createAlgorithms();
    
    /**
     * @brief Clean up resources before destruction
     */
    void cleanup();
};