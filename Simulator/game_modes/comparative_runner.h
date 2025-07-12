#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <chrono>
#include <mutex>
#include "base_game_mode.h"
#include "game_runner.h"
#include "utils/file_loader.h"
#include "utils/thread_pool.h"
#include "utils/error_collector.h"
#include "common/GameResult.h"

class ComparativeRunner : public BaseGameMode {
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

    struct ComparativeParameters : public BaseParameters {
        std::string gameManagersFolder;
        std::string algorithm1Lib;
        std::string algorithm2Lib;
        size_t numThreads = 1;
        
        ComparativeParameters() : BaseParameters() {}
    };

    /**
     * Run comparative analysis with multiple GameManagers
     * Uses the base class template method for execution flow
     * @param params Comparative parameters
     * @return Reference to results vector (avoid copying move-only results)
     */
    const std::vector<ComparativeResult>& runComparative(const ComparativeParameters& params) {
        // Execute using base class template method, then return reference to our specific results
        BaseGameMode::execute(params);
        return m_results;
    }

    /**
     * Get list of discovered GameManager libraries
     * @return Vector of GameManager info
     */
    const std::vector<GameManagerInfo>& getDiscoveredGameManagers() const;

protected:
    // Override abstract methods from BaseGameMode
    bool loadLibraries(const BaseParameters& params) override;
    bool loadMap(const std::string& mapFile) override;
    GameResult executeGameLogic(const BaseParameters& params) override;
    void displayResults(const GameResult& result) override;
    
    // Override cleanup for ComparativeRunner-specific cleanup
    void cleanup() override;

private:

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

    // Helper methods specific to ComparativeRunner
    bool loadLibrariesImpl(const ComparativeParameters& params);
    
    // Store parameters for later use in displayResults
    std::unique_ptr<ComparativeParameters> m_currentParams;

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

    // ComparativeRunner-specific data members
    FileLoader::BoardInfo m_boardInfo;
    std::vector<GameManagerInfo> m_discoveredGameManagers;
    std::vector<ComparativeResult> m_results;
    ErrorCollector m_errorCollector;
    
    // Thread-safety for parallel execution
    std::mutex m_resultsMutex;
    std::mutex m_gameManagerMutex;
};