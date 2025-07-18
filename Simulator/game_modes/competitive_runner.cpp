#include "competitive_runner.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <set>
#include <memory>
#include <future>
#include <dlfcn.h>
#include "utils/library_manager.h"
#include "registration/GameManagerRegistrar.h"
#include "registration/AlgorithmRegistrar.h"
#include "game_runner.h"

CompetitiveRunner::CompetitiveRunner() : BaseGameMode() {
}

CompetitiveRunner::~CompetitiveRunner() {
    // Base class destructor will handle cleanup
}

bool CompetitiveRunner::loadLibraries(const BaseParameters& params) {
    // Cast to derived parameter type
    const CompetitiveParameters* competitiveParams = dynamic_cast<const CompetitiveParameters*>(&params);
    if (!competitiveParams) {
        handleError("Invalid parameter type for CompetitiveRunner");
        return false;
    }
    
    // Store parameters for later use
    m_currentParams = std::make_unique<CompetitiveParameters>(*competitiveParams);
    
    return loadLibrariesImpl(*competitiveParams);
}

bool CompetitiveRunner::loadLibrariesImpl(const CompetitiveParameters& params) {
    // Load GameManager library first
    LibraryManager& libManager = LibraryManager::getInstance();
    GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    
    try {
        gmRegistrar.createGameManagerEntry(params.gameManagerLib);
        if (!libManager.loadLibrary(params.gameManagerLib)) {
            handleError("Failed to load GameManager: " + libManager.getLastError());
            gmRegistrar.removeLast();
            return false;
        }
        
        try {
            gmRegistrar.validateLastRegistration();
        } catch (const GameManagerRegistrar::BadGameManagerRegistrationException& e) {
            handleError("GameManager registration failed: " + std::string(e.name));
            return false;
        }
        
        // Get GameManager name
        if (gmRegistrar.count() > 0) {
            auto gmEntry = gmRegistrar.begin();
            m_gameManagerName = gmEntry->name();
        } else {
            handleError("No GameManager registered after loading");
            return false;
        }
        
        // Load algorithms from folder
        std::vector<std::string> algorithmFiles = enumerateFiles(params.algorithmsFolder, ".so");
        if (algorithmFiles.size() < 2) {
            handleError("Need at least 2 algorithms for competition. Found " + std::to_string(algorithmFiles.size()));
            return false;
        }
        
        AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        algoRegistrar.clear();
        
        m_discoveredAlgorithms.clear();
        for (const auto& file : algorithmFiles) {
            // Create algorithm entry and load library
            algoRegistrar.createAlgorithmFactoryEntry(file);
            if (!libManager.loadLibrary(file)) {
                handleError("Failed to load algorithm: " + libManager.getLastError());
                algoRegistrar.removeLast();
                continue;
            }
            
            // Validate registration
            try {
                algoRegistrar.validateLastRegistration();                
                // Create algorithm info for tracking
                AlgorithmInfo info;
                info.path = file;
                info.name = std::filesystem::path(file).stem().string();
                info.loaded = true;
                m_discoveredAlgorithms.push_back(std::move(info));
                
            } catch (const BadRegistrationException& e) {
                if (params.verbose) {
                    std::cout << "Warning: Algorithm registration failed for " << file << ": " << e.what() << std::endl;
                }
                algoRegistrar.removeLast();
            }
        }
        
        if (m_discoveredAlgorithms.size() < 2) {
            handleError("Need at least 2 valid algorithms for competition");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        handleError("Exception during library loading: " + std::string(e.what()));
        return false;
    }
}

bool CompetitiveRunner::loadMap(const std::string& /* mapFile */) {
    // For competitive mode, mapFile parameter is not used - we load all maps from folder
    // Validate that we have the required parameters
    if (!m_currentParams) {
        handleError("CompetitiveParameters not initialized");
        return false;
    }
    
    if (m_currentParams->gameMapsFolder.empty()) {
        handleError("Game maps folder parameter is empty");
        return false;
    }
    
    return loadMapsImpl(*m_currentParams);
}

bool CompetitiveRunner::loadMapsImpl(const CompetitiveParameters& params) {
    try {
        // Load maps from folder
        std::vector<std::string> mapFiles = enumerateFiles(params.gameMapsFolder, ".txt");
        if (mapFiles.empty()) {
            handleError("No maps found in " + params.gameMapsFolder);
            return false;
        }
        
        m_discoveredMaps.clear();
        m_loadedMaps.clear();
        for (const auto& file : mapFiles) {
            auto info = loadMapFile(file); // Use renamed method to avoid recursion
            if (info.loaded) {
                m_discoveredMaps.push_back(std::move(info));
                // Also load the actual map data for game execution
                auto boardInfo = FileLoader::loadBoardWithSatelliteView(file);
                if (boardInfo.satelliteView) {
                    m_loadedMaps.push_back(std::move(boardInfo));
                }
            } else if (params.verbose) {
                std::cout << "Warning: Failed to load map " << file << ": " << info.error << std::endl;
            }
        }
        
        if (m_discoveredMaps.empty()) {
            handleError("No valid maps found");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        handleError("Exception during map loading: " + std::string(e.what()));
        return false;
    }
}

GameResult CompetitiveRunner::executeGameLogic(const BaseParameters& params) {
    const CompetitiveParameters* competitiveParams = dynamic_cast<const CompetitiveParameters*>(&params);
    if (!competitiveParams) {
        handleError("Invalid parameter type for CompetitiveRunner");
        return createErrorResult();
    }
    
    m_finalScores.clear();
    
    // Create ThreadPool based on parameters
    size_t numThreads = competitiveParams->numThreads;

    ThreadPool threadPool(numThreads);
    std::vector<std::future<void>> futures;
    std::unordered_map<int, AlgorithmScore> scores;
    int numAlgorithms = static_cast<int>(m_discoveredAlgorithms.size());
    
    // For each map, generate pairings and submit parallel tasks
    for (int mapIndex = 0; mapIndex < static_cast<int>(m_discoveredMaps.size()); ++mapIndex) {
        auto pairings = generatePairings(numAlgorithms, mapIndex);
        
        for (const auto& pairing : pairings) {
            // Submit each match as a parallel task
            auto future = threadPool.enqueue([this, pairing, mapIndex, competitiveParams, &scores]() {
                try {
                    auto result = executeMatch(pairing.first, pairing.second, mapIndex, *competitiveParams);
                    std::lock_guard<std::mutex> lock(m_scoresMutex);
                    updateScores(pairing.first, pairing.second, result, scores);
                } catch (const std::exception& e) {
                    std::string error = "Error executing match between algorithms " + 
                                      std::to_string(pairing.first) + " and " + std::to_string(pairing.second) + 
                                      " on map " + std::to_string(mapIndex) + ": " + e.what();
                    handleError(error);
                }
            });
            futures.push_back(std::move(future));
        }
    }
    
    // Wait for all matches to complete
    for (auto& future : futures) {
        try {
            future.get();
        } catch (const std::exception& e) {
            handleError("Thread execution failed: " + std::string(e.what()));
        }
    }
    
    // Sort scores and store them
    m_finalScores = sortByScore(scores);
    
    // Return a summary result - just indicating success
    GameResult summary;
    summary.winner = 0; // Tournament completed
    summary.rounds = static_cast<int>(m_finalScores.size());
    summary.reason = GameResult::ALL_TANKS_DEAD; // Use as completion indicator
    
    return summary;
}

void CompetitiveRunner::displayResults(const GameResult& /* result */) {
    // Generate output file with tournament results
    if (!m_finalScores.empty() && m_currentParams) {
        std::string outputPath = m_currentParams->algorithmsFolder + "/competition_" + 
                               generateTimestamp(true) + ".txt";
        generateOutput(m_finalScores, outputPath, *m_currentParams);        
    }
}

CompetitiveRunner::MapInfo CompetitiveRunner::loadMapFile(const std::string& mapPath) {
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

// Remove custom generateTimestamp - using base class method

void CompetitiveRunner::cleanup() {
    GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    gmRegistrar.clear();
    algoRegistrar.clear();
    m_libraryManager.unloadAllLibraries();
    
    // Clear our specific data
    m_discoveredAlgorithms.clear();
    m_discoveredMaps.clear();
    m_loadedMaps.clear();
    m_finalScores.clear();
    m_currentParams.reset();
    
    // Call base class cleanup
    BaseGameMode::cleanup();
}

// Remove the runCompetition method - using base class template method approach

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
    
    try {
        // Get registrar for algorithm lookups
        AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        
        // Use the pre-loaded GameManager (loaded during initialization)
        std::string gameManagerName = m_gameManagerName;
        
        // Get algorithm names from the pre-registered algorithms
        std::string algorithm1Name, algorithm2Name;
        if (algoRegistrar.size() < 2) {
            handleError("Not enough algorithms registered for competition");
            return result;
        }
        
        // Find the algorithm names by their paths
        std::string algorithm1Path = m_discoveredAlgorithms[algorithm1Index].path;
        std::string algorithm2Path = m_discoveredAlgorithms[algorithm2Index].path;
        
        // Search for matching algorithms in the registrar
        bool found1 = false, found2 = false;
        for (auto iter = algoRegistrar.begin(); iter != algoRegistrar.end(); ++iter) {
            std::string entryPath = iter->getName(); // Entry name is the path
            if (entryPath == algorithm1Path && !found1) {
                algorithm1Name = entryPath;
                found1 = true;
            }
            if (entryPath == algorithm2Path && !found2) {
                algorithm2Name = entryPath;
                found2 = true;
            }
        }
        
        if (!found1 || !found2) {
            handleError("Could not find registered algorithms for match");
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
        
        // Clean up for next iteration - but don't call full cleanup during execution
        
    } catch (const std::exception& e) {
        handleError("Exception during match execution: " + std::string(e.what()));
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