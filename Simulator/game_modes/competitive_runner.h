#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <chrono>
#include <unordered_map>
#include "game_runner.h"
#include "utils/file_loader.h"

class CompetitiveRunner {
public:
    CompetitiveRunner();
    ~CompetitiveRunner();

    struct AlgorithmInfo {
        std::string path;
        std::string name;
        bool loaded;
        std::string error;
    };

    struct MapInfo {
        std::string path;
        std::string name;
        bool loaded;
        std::string error;
    };

    struct AlgorithmScore {
        std::string algorithmName;
        int totalScore = 0;
        int wins = 0;
        int ties = 0;
        int losses = 0;
    };

    struct CompetitiveParameters {
        std::string gameMapsFolder;
        std::string gameManagerLib;
        std::string algorithmsFolder;
        bool verbose = true;
    };

    /**
     * Run competitive tournament with multiple algorithms and maps
     * @param params Competitive parameters
     * @return Vector of final algorithm scores sorted by performance
     */
    std::vector<AlgorithmScore> runCompetition(const CompetitiveParameters& params);

    /**
     * Get list of discovered algorithm libraries
     * @return Vector of algorithm info
     */
    const std::vector<AlgorithmInfo>& getDiscoveredAlgorithms() const;

    /**
     * Get list of discovered map files
     * @return Vector of map info
     */
    const std::vector<MapInfo>& getDiscoveredMaps() const;

private:
    /**
     * Enumerate all .so files in the specified directory
     * @param directory Directory to scan
     * @return Vector of .so file paths
     */
    std::vector<std::string> enumerateAlgorithmFiles(const std::string& directory);

    /**
     * Enumerate all map files in the specified directory
     * @param directory Directory to scan
     * @return Vector of map file paths
     */
    std::vector<std::string> enumerateMapFiles(const std::string& directory);

    /**
     * Load and validate algorithm from .so file
     * @param soPath Path to .so file
     * @return AlgorithmInfo with load result
     */
    AlgorithmInfo loadAlgorithm(const std::string& soPath);

    /**
     * Load and validate map from file
     * @param mapPath Path to map file
     * @return MapInfo with load result
     */
    MapInfo loadMap(const std::string& mapPath);

    /**
     * Generate algorithm pairings for given map index using competition formula
     * @param numAlgorithms Total number of algorithms
     * @param mapIndex Index of current map (k)
     * @return Vector of pairs (algorithm1_index, algorithm2_index)
     */
    std::vector<std::pair<int, int>> generatePairings(int numAlgorithms, int mapIndex);

    /**
     * Execute single game between two algorithms on specified map
     * @param algorithm1Index Index of first algorithm
     * @param algorithm2Index Index of second algorithm
     * @param mapIndex Index of map to use
     * @param params Competitive parameters
     * @return GameResult with match outcome
     */
    GameResult executeMatch(
        int algorithm1Index,
        int algorithm2Index,
        int mapIndex,
        const CompetitiveParameters& params
    );

    /**
     * Update algorithm scores based on game result
     * @param algorithm1Index Index of first algorithm
     * @param algorithm2Index Index of second algorithm
     * @param result Game result
     * @param scores Reference to scores map to update
     */
    void updateScores(
        int algorithm1Index,
        int algorithm2Index,
        const GameResult& result,
        std::unordered_map<int, AlgorithmScore>& scores
    );

    /**
     * Generate output file with competition results
     * @param scores Final algorithm scores
     * @param outputPath Output file path
     * @param params Competitive parameters for header info
     */
    void generateOutput(
        const std::vector<AlgorithmScore>& scores,
        const std::string& outputPath,
        const CompetitiveParameters& params
    );

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
     * Sort algorithms by score (descending order)
     * @param scores Unsorted scores map
     * @return Sorted vector of algorithm scores
     */
    std::vector<AlgorithmScore> sortByScore(const std::unordered_map<int, AlgorithmScore>& scores);

    std::vector<AlgorithmInfo> m_discoveredAlgorithms;
    std::vector<MapInfo> m_discoveredMaps;
    std::vector<FileLoader::BoardInfo> m_loadedMaps;
    std::string m_gameManagerName;
};