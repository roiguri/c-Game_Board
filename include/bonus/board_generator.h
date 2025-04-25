#pragma once

#include <string>
#include <random>
#include <vector>
#include <map>
#include "game_board.h"
#include "bonus/config/board_config.h"

/**
 * @brief Generates game boards based on configuration
 */
class BoardGenerator {
public:
    /**
     * @brief Constructor with default configuration
     */
    BoardGenerator();
    
    /**
     * @brief Constructor with specified configuration
     * 
     * @param config Configuration to use
     */
    explicit BoardGenerator(const BoardConfig& config);

    /**
     * @brief Get the current configuration
     * 
     * @return Current configuration
     */
    const BoardConfig& getConfig() const;

    /**
     * @brief Set the configuration
     * 
     * @param config Configuration to set
     */
    void setConfig(const BoardConfig& config);
    
    /**
     * @brief Generate a game board
     * 
     * @return true if generation was successful
     */
    bool generateBoard();
    
    /**
     * @brief Save generated board to a file
     * 
     * @param outputPath Path to save the board file
     * @return true if save was successful
     */
    bool saveToFile(const std::string& outputPath) const;
    
    /**
     * @brief Get the generated board as a vector of strings
     * 
     * @return Vector of strings representing the board
     */
    std::vector<std::string> getBoardLines() const;
    
private:
    BoardConfig m_config;
    std::mt19937 m_rng;
    std::vector<std::vector<char>> m_board;
    
    // Tank positions
    std::pair<int, int> m_tank1Pos;
    std::pair<int, int> m_tank2Pos;
    
    // Helper methods
    void initializeEmptyBoard();
    bool placeTanks();
    void placeWalls();
    void placeMines();
    bool validateBoard();
    void applySymmetry(int x, int y, char cellType);
    
    // Get symmetry partner positions for a given position
    std::vector<std::pair<int, int>> getSymmetryPositions(int x, int y);
    
    // Validate if a position is valid for placing a cell
    bool isValidPosition(int x, int y) const;
    
    // BFS helper for connectivity check
    bool canReach(int startX, int startY, int endX, int endY);
};