#include "competitive_runner.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <set>
#include <dlfcn.h>
#include "utils/library_manager.h"
#include "registration/GameManagerRegistrar.h"
#include "registration/AlgorithmRegistrar.h"
#include "game_runner.h"

CompetitiveRunner::CompetitiveRunner() {
}

CompetitiveRunner::~CompetitiveRunner() {
    cleanup();
}

std::vector<std::string> CompetitiveRunner::enumerateAlgorithmFiles(const std::string& directory) {
    std::vector<std::string> soFiles;
    
    try {
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
            return soFiles;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".so") {
                soFiles.push_back(entry.path().string());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error scanning algorithm directory " << directory << ": " << e.what() << std::endl;
    }
    
    return soFiles;
}

std::vector<std::string> CompetitiveRunner::enumerateMapFiles(const std::string& directory) {
    std::vector<std::string> mapFiles;
    
    try {
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
            return mapFiles;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                // Accept any file as potential map file
                mapFiles.push_back(entry.path().string());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error scanning maps directory " << directory << ": " << e.what() << std::endl;
    }
    
    return mapFiles;
}

CompetitiveRunner::AlgorithmInfo CompetitiveRunner::loadAlgorithm(const std::string& soPath) {
    AlgorithmInfo info;
    info.path = soPath;
    info.name = std::filesystem::path(soPath).stem().string();
    info.loaded = false;
    
    // Try to load the .so file to validate it
    void* handle = dlopen(soPath.c_str(), RTLD_LAZY);
    if (!handle) {
        info.error = std::string("Failed to load: ") + dlerror();
        return info;
    }
    // TODO: check what needs changing here
    // Check if required symbols exist (basic validation)
    // For now, just check if it loads successfully
    info.loaded = true;
    dlclose(handle);
    
    return info;
}

CompetitiveRunner::MapInfo CompetitiveRunner::loadMap(const std::string& mapPath) {
    MapInfo info;
    info.path = mapPath;
    info.name = std::filesystem::path(mapPath).stem().string();
    info.loaded = false;
    
    try {
        // Try to load the map using FileLoader to validate it
        auto boardInfo = FileLoader::loadBoardWithSatelliteView(mapPath);
        
        // Basic validation - check if map has valid dimensions and content
        if (!boardInfo.satelliteView || boardInfo.rows == 0 || boardInfo.cols == 0) {
            info.error = "Invalid map format: empty or zero dimensions";
            return info;
        }
        
        info.loaded = true;
    } catch (const std::exception& e) {
        info.error = std::string("Failed to load map: ") + e.what();
    }
    
    return info;
}

const std::vector<CompetitiveRunner::AlgorithmInfo>& CompetitiveRunner::getDiscoveredAlgorithms() const {
    return m_discoveredAlgorithms;
}

const std::vector<CompetitiveRunner::MapInfo>& CompetitiveRunner::getDiscoveredMaps() const {
    return m_discoveredMaps;
}

std::string CompetitiveRunner::generateTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    ss << "_" << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void CompetitiveRunner::cleanup() {
    GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    LibraryManager& libManager = LibraryManager::getInstance();
    
    gmRegistrar.clear();
    algoRegistrar.clear();
    libManager.unloadAllLibraries();
}

// Placeholder implementations for remaining methods that will be implemented in subsequent subtasks
std::vector<CompetitiveRunner::AlgorithmScore> CompetitiveRunner::runCompetition(const CompetitiveParameters& params) {
    // Step 1: Load algorithms from folder
    auto algorithmFiles = enumerateAlgorithmFiles(params.algorithmsFolder);
    if (algorithmFiles.size() < 2) {
        std::cerr << "Error: Need at least 2 algorithms for competition. Found " << algorithmFiles.size() << std::endl;
        return {};
    }
    
    m_discoveredAlgorithms.clear();
    for (const auto& file : algorithmFiles) {
        auto info = loadAlgorithm(file);
        if (info.loaded) {
            m_discoveredAlgorithms.push_back(std::move(info));
        } else {
            std::cerr << "Warning: Failed to load algorithm " << file << ": " << info.error << std::endl;
        }
    }
    
    if (m_discoveredAlgorithms.size() < 2) {
        std::cerr << "Error: Need at least 2 valid algorithms for competition." << std::endl;
        return {};
    }
    
    // Step 2: Load maps from folder
    auto mapFiles = enumerateMapFiles(params.gameMapsFolder);
    if (mapFiles.empty()) {
        std::cerr << "Error: No maps found in " << params.gameMapsFolder << std::endl;
        return {};
    }
    
    m_discoveredMaps.clear();
    m_loadedMaps.clear();
    for (const auto& file : mapFiles) {
        auto info = loadMap(file);
        if (info.loaded) {
            m_discoveredMaps.push_back(std::move(info));
            // Also load the actual map data for game execution
            auto boardInfo = FileLoader::loadBoardWithSatelliteView(file);
            if (boardInfo.satelliteView) {
                m_loadedMaps.push_back(std::move(boardInfo));
            }
        } else {
            std::cerr << "Warning: Failed to load map " << file << ": " << info.error << std::endl;
        }
    }
    
    if (m_discoveredMaps.empty()) {
        std::cerr << "Error: No valid maps found." << std::endl;
        return {};
    }
    
    // Step 3: Execute tournament
    std::unordered_map<int, AlgorithmScore> scores;
    int numAlgorithms = static_cast<int>(m_discoveredAlgorithms.size());
    
    // For each map, generate pairings and execute matches
    for (int mapIndex = 0; mapIndex < static_cast<int>(m_discoveredMaps.size()); ++mapIndex) {
        auto pairings = generatePairings(numAlgorithms, mapIndex);
        
        for (const auto& pairing : pairings) {
            try {
                auto result = executeMatch(pairing.first, pairing.second, mapIndex, params);
                updateScores(pairing.first, pairing.second, result, scores);
            } catch (const std::exception& e) {
                std::cerr << "Error executing match between algorithms " 
                          << pairing.first << " and " << pairing.second 
                          << " on map " << mapIndex << ": " << e.what() << std::endl;
            }
        }
    }
    
    // Step 4: Sort and generate output
    auto sortedScores = sortByScore(scores);
    
    // Generate output file
    std::string outputPath = params.algorithmsFolder + "/competition_" + generateTimestamp() + ".txt";
    generateOutput(sortedScores, outputPath, params);
    
    return sortedScores;
}

std::vector<std::pair<int, int>> CompetitiveRunner::generatePairings(int numAlgorithms, int mapIndex) {
    std::vector<std::pair<int, int>> pairings;
    
    // Handle edge cases
    if (numAlgorithms < 2) {
        return pairings; // No pairings possible with less than 2 algorithms
    }
    
    // Competition pairing formula from instructions:
    // For kth map, algorithms i against (i + 1 + k % (N-1)) % N
    // This creates a total of N games per map (each algorithm plays once)
    
    int N = numAlgorithms;
    int k = mapIndex;
    
    // Use set to avoid duplicate pairings
    std::set<std::pair<int, int>> uniquePairings;
    
    for (int i = 0; i < N; i++) {
        int opponent = (i + 1 + k % (N - 1)) % N;
        
        // Ensure we don't pair an algorithm with itself
        if (i != opponent) {
            // Always store pairs in sorted order to avoid duplicates (i,j) and (j,i)
            int first = std::min(i, opponent);
            int second = std::max(i, opponent);
            uniquePairings.insert({first, second});
        }
    }
    
    // Convert set to vector
    for (const auto& pair : uniquePairings) {
        pairings.push_back(pair);
    }
    
    return pairings;
}

GameResult CompetitiveRunner::executeMatch(int algorithm1Index, int algorithm2Index, int mapIndex, const CompetitiveParameters& params) {
    GameResult result;
    result.winner = 0;
    result.reason = GameResult::MAX_STEPS;
    result.rounds = 0;
    result.remaining_tanks = {0, 0};
    result.gameState = nullptr;
    
    if (params.verbose) {
        std::cout << "Executing match: Algorithm " << algorithm1Index 
                  << " vs Algorithm " << algorithm2Index 
                  << " on map " << mapIndex << std::endl;
    }
    
    try {
        // Load the GameManager library
        LibraryManager& libManager = LibraryManager::getInstance();
        GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
        AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        
        // Load GameManager
        gmRegistrar.createGameManagerEntry(params.gameManagerLib);
        if (!libManager.loadLibrary(params.gameManagerLib)) {
            std::cerr << "Failed to load GameManager: " << libManager.getLastError() << std::endl;
            gmRegistrar.removeLast();
            return result;
        }
        
        // Validate GameManager registration
        try {
            gmRegistrar.validateLastRegistration();
        } catch (const GameManagerRegistrar::BadGameManagerRegistrationException& e) {
            std::cerr << "GameManager registration failed: " << e.name << std::endl;
            cleanup();
            return result;
        }
        
        // Get GameManager name
        std::string gameManagerName;
        if (gmRegistrar.count() > 0) {
            auto gmEntry = gmRegistrar.begin();
            gameManagerName = gmEntry->name();
        } else {
            std::cerr << "No GameManager registered after loading" << std::endl;
            cleanup();
            return result;
        }
        
        // Load Algorithm 1
        const std::string& algorithm1Path = m_discoveredAlgorithms[algorithm1Index].path;
        algoRegistrar.createAlgorithmFactoryEntry(algorithm1Path);
        if (!libManager.loadLibrary(algorithm1Path)) {
            std::cerr << "Failed to load Algorithm 1: " << libManager.getLastError() << std::endl;
            algoRegistrar.removeLast();
            cleanup();
            return result;
        }
        
        try {
            algoRegistrar.validateLastRegistration();
        } catch (const BadRegistrationException& e) {
            std::cerr << "Algorithm 1 registration failed: " << e.what() << std::endl;
            cleanup();
            return result;
        }
        
        // Load Algorithm 2 (if different)
        const std::string& algorithm2Path = m_discoveredAlgorithms[algorithm2Index].path;
        if (algorithm2Path != algorithm1Path) {
            algoRegistrar.createAlgorithmFactoryEntry(algorithm2Path);
            if (!libManager.loadLibrary(algorithm2Path)) {
                std::cerr << "Failed to load Algorithm 2: " << libManager.getLastError() << std::endl;
                algoRegistrar.removeLast();
                cleanup();
                return result;
            }
            
            try {
                algoRegistrar.validateLastRegistration();
            } catch (const BadRegistrationException& e) {
                std::cerr << "Algorithm 2 registration failed: " << e.what() << std::endl;
                cleanup();
                return result;
            }
        }
        
        // Get algorithm names
        std::string algorithm1Name, algorithm2Name;
        if (algoRegistrar.size() > 0) {
            auto algoIter = algoRegistrar.begin();
            algorithm1Name = algoIter->getName();
            
            if (algoRegistrar.size() > 1) {
                ++algoIter;
                algorithm2Name = algoIter->getName();
            } else {
                algorithm2Name = algorithm1Name;
            }
        } else {
            std::cerr << "No algorithms registered" << std::endl;
            cleanup();
            return result;
        }
        
        // Execute game using GameRunner
        result = GameRunner::runSingleGame(
            m_loadedMaps[mapIndex],
            gameManagerName,
            algorithm1Name,
            algorithm2Name,
            params.verbose
        );
        
        // Clean up for next iteration
        cleanup();
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during match execution: " << e.what() << std::endl;
        cleanup();
        return result;
    }
    
    return result;
}

void CompetitiveRunner::updateScores(int algorithm1Index, int algorithm2Index, const GameResult& result, std::unordered_map<int, AlgorithmScore>& scores) {
    // Initialize scores for algorithms if not already present
    if (scores.find(algorithm1Index) == scores.end()) {
        scores[algorithm1Index] = AlgorithmScore{};
        scores[algorithm1Index].algorithmName = m_discoveredAlgorithms[algorithm1Index].name;
    }
    if (scores.find(algorithm2Index) == scores.end()) {
        scores[algorithm2Index] = AlgorithmScore{};
        scores[algorithm2Index].algorithmName = m_discoveredAlgorithms[algorithm2Index].name;
    }
    
    // Apply 3-1-0 scoring system based on game result
    if (result.winner == 0) {
        // Tie - both algorithms get 1 point
        scores[algorithm1Index].totalScore += 1;
        scores[algorithm1Index].ties++;
        scores[algorithm2Index].totalScore += 1;
        scores[algorithm2Index].ties++;
    } else if (result.winner == 1) {
        // Algorithm 1 wins - gets 3 points, algorithm 2 gets 0
        scores[algorithm1Index].totalScore += 3;
        scores[algorithm1Index].wins++;
        scores[algorithm2Index].losses++;
    } else if (result.winner == 2) {
        // Algorithm 2 wins - gets 3 points, algorithm 1 gets 0
        scores[algorithm2Index].totalScore += 3;
        scores[algorithm2Index].wins++;
        scores[algorithm1Index].losses++;
    }
}

void CompetitiveRunner::generateOutput(const std::vector<AlgorithmScore>& scores, const std::string& outputPath, const CompetitiveParameters& params) {
    std::ofstream outFile(outputPath);
    std::ostream* output = &outFile;
    
    // If file cannot be opened, fall back to console output
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot create output file " << outputPath << ". Writing to console instead." << std::endl;
        output = &std::cout;
    }
    
    // Write header according to instructions format
    *output << "game_maps_folder=" << params.gameMapsFolder << std::endl;
    *output << "game_manager=" << params.gameManagerLib << std::endl;
    *output << std::endl; // Empty line after header
    
    // TODO: make sure algorithm names are correct
    // Write sorted algorithm scores
    // Format: <algorithm name> <total score>
    for (const auto& score : scores) {
        *output << score.algorithmName << " " << score.totalScore << std::endl;
    }
    
    if (outFile.is_open()) {
        outFile.close();
    }
}

std::vector<CompetitiveRunner::AlgorithmScore> CompetitiveRunner::sortByScore(const std::unordered_map<int, AlgorithmScore>& scores) {
    std::vector<AlgorithmScore> sortedScores;
    
    // Extract all algorithm scores from the map
    for (const auto& pair : scores) {
        sortedScores.push_back(pair.second);
    }
    
    // Sort by total score in descending order (highest scores first)
    // If scores are equal, algorithms can appear in any order per instructions
    std::sort(sortedScores.begin(), sortedScores.end(), 
        [](const AlgorithmScore& a, const AlgorithmScore& b) {
            return a.totalScore > b.totalScore;
        });
    
    return sortedScores;
}