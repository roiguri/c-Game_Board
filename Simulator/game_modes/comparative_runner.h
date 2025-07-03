#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <chrono>
#include "game_runner.h"
#include "utils/file_loader.h"

class ComparativeRunner {
public:
    ComparativeRunner();
    ~ComparativeRunner();

    struct GameManagerInfo {
        std::string path;
        std::string name;
        bool loaded;
        std::string error;
    };

    struct ComparativeResult {
        std::string gameManagerName;
        GameResult gameResult;
        std::chrono::milliseconds executionTime;
        bool success;
        std::string error;
        int mapRows;
        int mapCols;
        
        // Make it move-only to handle unique_ptr in GameResult
        ComparativeResult() = default;
        ComparativeResult(const ComparativeResult&) = delete;
        ComparativeResult& operator=(const ComparativeResult&) = delete;
        ComparativeResult(ComparativeResult&&) = default;
        ComparativeResult& operator=(ComparativeResult&&) = default;
    };

    struct ComparativeParameters {
        std::string gameManagersFolder;
        std::string mapFile;
        std::string algorithm1Lib;
        std::string algorithm2Lib;
        bool verbose = true;
    };

    /**
     * Run comparative analysis with multiple GameManagers
     * @param params Comparative parameters
     * @return Vector of results from all GameManagers
     */
    std::vector<ComparativeResult> runComparative(const ComparativeParameters& params);

    /**
     * Get list of discovered GameManager libraries
     * @return Vector of GameManager info
     */
    const std::vector<GameManagerInfo>& getDiscoveredGameManagers() const;

private:
    /**
     * Enumerate all .so files in the specified directory
     * @param directory Directory to scan
     * @return Vector of .so file paths
     */
    std::vector<std::string> enumerateSoFiles(const std::string& directory);

    /**
     * Load and validate GameManager from .so file
     * @param soPath Path to .so file
     * @return GameManagerInfo with load result
     */
    GameManagerInfo loadGameManager(const std::string& soPath);

    /**
     * Execute game with specific GameManager
     * @param gameManagerName Name of GameManager to use
     * @param params Game parameters
     * @return ComparativeResult with execution outcome
     */
    ComparativeResult executeWithGameManager(
        const std::string& gameManagerName,
        const ComparativeParameters& params,
        const FileLoader::BoardInfo& boardInfo
    );

    /**
     * Generate output file with comparative results
     * @param results Vector of comparative results
     * @param outputPath Output file path
     * @param params Comparative parameters for header info
     */
    void generateOutput(const std::vector<ComparativeResult>& results, const std::string& outputPath, const ComparativeParameters& params);

    /**
     * Generate timestamp for output filename
     * @return Timestamp string
     */
    std::string generateTimestamp() const;

    /**
     * Clean up loaded libraries and registrars
     */
    void cleanup();

    /**
     * Group GameManagers by identical game outcomes
     * @param results Vector of comparative results
     * @return Vector of pairs (GameManager names, representative result) for each group
     */
    std::vector<std::pair<std::vector<std::string>, const ComparativeResult*>> groupResultsByOutcome(const std::vector<ComparativeResult>& results);

    /**
     * Print result group to output stream
     * @param output Output stream (file or console)
     * @param gameManagerNames List of GameManager names in the group
     * @param representative Representative result for the group
     */
    void printResultGroup(std::ostream& output, const std::vector<std::string>& gameManagerNames, const ComparativeResult* representative);

    /**
     * Convert final game state to string representation
     * @param gameState SatelliteView of the final game state
     * @param rows Map rows
     * @param cols Map columns
     * @return String representation of the game state
     */
    std::string gameStateToString(const SatelliteView& gameState, int rows, int cols);

    std::vector<GameManagerInfo> m_discoveredGameManagers;
    FileLoader::BoardInfo m_boardInfo;
};