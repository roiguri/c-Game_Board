#pragma once

#include <map>
#include <random>
#include <string>
#include <vector>

#include "game_board.h"

namespace UserCommon_098765432_123456789 {

/**
 * @brief Configuration for board generation
 */
struct BoardConfig {
    int width = 15;
    int height = 10;
    float wallDensity = 0.25f;
    float mineDensity = 0.05f;
    std::string symmetry = "none"; // none, horizontal, vertical, diagonal
    int seed = -1; // -1 for random seed
    int maxSteps = 1000; // Number of max steps for the game
    int numShells = 10;  // Number of shells per tank
    std::string mapName = "Generated Map"; // Map name/description
    int numTanksPerPlayer = 1; // Number of tanks per player
};

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
     * @brief Load configuration from file
     * 
     * @param configPath Path to configuration file
     * @return true if configuration was loaded successfully
     */
    bool loadConfig(const std::string& configPath);
    
    /**
     * @brief Generate a game board
     * 
     * @return true if generation was successful
     */
    bool generateBoard();
    
    /**
     * @brief Save generated board to a file
     *
     * The output file will have the following format:
     *   Line 1: Map name/description
     *   Line 2: MaxSteps = <NUM>
     *   Line 3: NumShells = <NUM>
     *   Line 4: Rows = <NUM>
     *   Line 5: Cols = <NUM>
     *   Then:   <Rows> lines of board content (each line Cols characters)
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
    
    /**
     * @brief Get the current configuration
     */
    const BoardConfig& getConfig() const;
    
    /**
     * @brief Set configuration directly
     * 
     * @param config New configuration to use
     */
    void setConfig(const BoardConfig& config);
    
private:
    BoardConfig m_config;
    std::mt19937 m_rng;
    std::vector<std::vector<char>> m_board;
    std::vector<std::pair<int, int>> m_tankPositions; // Stores all tank positions
    
    // Helper methods
    void initializeEmptyBoard();
    /**
     * @brief Place tanks for both players on the board.
     * Places numTanksPerPlayer tanks for each player, respecting symmetry and avoiding collisions.
     */
    bool placeTanks();
    void placeWalls();
    void placeMines();
    bool validateBoard();
    void applySymmetry(int x, int y, char cellType);
    
    // Get symmetry partner positions for a given position
    std::vector<std::pair<int, int>> getSymmetryPositions(int x, int y);
    
    // Validate if a position is valid for placing a cell (not occupied by a tank)
    bool isValidPosition(int x, int y) const;
    
    // BFS helper for connectivity check
    bool canReach(int startX, int startY, int endX, int endY);
    
    // Parse a key-value pair from config file
    bool parseConfigValue(const std::string& key, const std::string& value);
    
    // --- New helpers for symmetry-aware tank placement ---
    std::pair<int, int> getMirror(int x, int y) const;
    bool isOccupied(int x, int y) const;
    void placeTank(int x, int y, int player);
};

} // namespace UserCommon_098765432_123456789