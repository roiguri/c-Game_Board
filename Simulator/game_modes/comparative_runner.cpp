#include "comparative_runner.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <map>
#include "utils/library_manager.h"
#include "registration/GameManagerRegistrar.h"
#include "registration/AlgorithmRegistrar.h"

ComparativeRunner::ComparativeRunner() = default;

ComparativeRunner::~ComparativeRunner() {
    cleanup();
}

std::vector<ComparativeRunner::ComparativeResult> ComparativeRunner::runComparative(const ComparativeParameters& params) {
    std::vector<ComparativeResult> results;
    
    try {
        // Load map file
        if (params.verbose) {
            std::cout << "Loading map file: " << params.mapFile << std::endl;
        }
        
        m_boardInfo = FileLoader::loadBoardWithSatelliteView(params.mapFile);
        if (!m_boardInfo.satelliteView) {
            std::cerr << "Error: Failed to load map file: " << params.mapFile << std::endl;
            return results;
        }
        
        // Enumerate .so files in GameManagers folder
        if (params.verbose) {
            std::cout << "Enumerating GameManager libraries in: " << params.gameManagersFolder << std::endl;
        }
        
        std::vector<std::string> soFiles = enumerateSoFiles(params.gameManagersFolder);
        if (soFiles.empty()) {
            std::cerr << "No .so files found in directory: " << params.gameManagersFolder << std::endl;
            return results;
        }
        
        if (params.verbose) {
            std::cout << "Found " << soFiles.size() << " .so files" << std::endl;
        }
        
        // Load and validate each GameManager
        for (const auto& soPath : soFiles) {
            GameManagerInfo info = loadGameManager(soPath);
            m_discoveredGameManagers.push_back(info);
            
            if (params.verbose) {
                if (info.loaded) {
                    std::cout << "Successfully loaded GameManager: " << info.name << std::endl;
                } else {
                    std::cout << "Failed to load GameManager from: " << info.path 
                              << " (Error: " << info.error << ")" << std::endl;
                }
            }
        }
        
        // Execute games with each successfully loaded GameManager
        for (const auto& info : m_discoveredGameManagers) {
            if (info.loaded) {
                if (params.verbose) {
                    std::cout << "Executing game with GameManager: " << info.name << std::endl;
                }
                
                ComparativeResult result = executeWithGameManager(info.name, params, m_boardInfo);
                results.push_back(std::move(result));
                
                if (params.verbose) {
                    std::cout << "Game completed in " << result.executionTime.count() << "ms" << std::endl;
                }
            }
        }
        
        // Generate output file
        if (!results.empty()) {
            std::string outputPath = "comparative_results_" + generateTimestamp() + ".txt";
            generateOutput(results, outputPath, params);
            
            if (params.verbose) {
                std::cout << "Results written to: " << outputPath << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during comparative execution: " << e.what() << std::endl;
        cleanup();
        return results;
    } catch (...) {
        std::cerr << "Unknown exception during comparative execution" << std::endl;
        cleanup();
        return results;
    }
    
    return results;
}

const std::vector<ComparativeRunner::GameManagerInfo>& ComparativeRunner::getDiscoveredGameManagers() const {
    return m_discoveredGameManagers;
}

std::vector<std::string> ComparativeRunner::enumerateSoFiles(const std::string& directory) {
    std::vector<std::string> soFiles;
    
    try {
        // Check if directory exists
        if (!std::filesystem::exists(directory)) {
            std::cerr << "Directory does not exist: " << directory << std::endl;
            return soFiles;
        }
        
        if (!std::filesystem::is_directory(directory)) {
            std::cerr << "Path is not a directory: " << directory << std::endl;
            return soFiles;
        }
        
        // Iterate through directory entries
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string extension = entry.path().extension().string();
                
                // Check for .so extension
                if (extension == ".so") {
                    soFiles.push_back(entry.path().string());
                }
            }
        }
        
        // Sort files for consistent ordering
        std::sort(soFiles.begin(), soFiles.end());
        
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error while enumerating .so files: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception while enumerating .so files: " << e.what() << std::endl;
    }
    
    return soFiles;
}

ComparativeRunner::GameManagerInfo ComparativeRunner::loadGameManager(const std::string& soPath) {
    GameManagerInfo info;
    info.path = soPath;
    info.loaded = false;
    info.error = "";
    
    try {
        LibraryManager& libManager = LibraryManager::getInstance();
        GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
        
        // Create GameManager entry
        gmRegistrar.createGameManagerEntry(soPath);
        
        // Load library
        if (!libManager.loadLibrary(soPath)) {
            info.error = libManager.getLastError();
            gmRegistrar.removeLast();
            return info;
        }
        
        // Validate registration
        try {
            gmRegistrar.validateLastRegistration();
        } catch (const GameManagerRegistrar::BadGameManagerRegistrationException& e) {
            info.error = "GameManager registration failed for " + e.name;
            cleanup();
            return info;
        }
        
        // Get GameManager name
        if (gmRegistrar.count() > 0) {
            auto gmEntry = gmRegistrar.begin();
            info.name = gmEntry->name();
            info.loaded = true;
        } else {
            info.error = "No GameManager registered after loading";
            cleanup();
            return info;
        }
        
    } catch (const std::exception& e) {
        info.error = "Exception during GameManager loading: " + std::string(e.what());
        cleanup();
        return info;
    }
    
    return info;
}

ComparativeRunner::ComparativeResult ComparativeRunner::executeWithGameManager(
    const std::string& gameManagerName,
    const ComparativeParameters& params,
    const FileLoader::BoardInfo& boardInfo) {
    
    ComparativeResult result;
    result.gameManagerName = gameManagerName;
    result.success = false;
    result.executionTime = std::chrono::milliseconds(0);
    
    try {
        // Load algorithm libraries
        LibraryManager& libManager = LibraryManager::getInstance();
        AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        
        // Load Algorithm 1
        algoRegistrar.createAlgorithmFactoryEntry(params.algorithm1Lib);
        if (!libManager.loadLibrary(params.algorithm1Lib)) {
            result.error = "Failed to load Algorithm 1: " + libManager.getLastError();
            algoRegistrar.removeLast();
            return result;
        }
        
        try {
            algoRegistrar.validateLastRegistration();
        } catch (const BadRegistrationException& e) {
            result.error = "Algorithm 1 registration failed: " + std::string(e.what());
            cleanup();
            return result;
        }
        
        // Load Algorithm 2 (if different)
        if (params.algorithm2Lib != params.algorithm1Lib) {
            algoRegistrar.createAlgorithmFactoryEntry(params.algorithm2Lib);
            if (!libManager.loadLibrary(params.algorithm2Lib)) {
                result.error = "Failed to load Algorithm 2: " + libManager.getLastError();
                algoRegistrar.removeLast();
                cleanup();
                return result;
            }
            
            try {
                algoRegistrar.validateLastRegistration();
            } catch (const BadRegistrationException& e) {
                result.error = "Algorithm 2 registration failed: " + std::string(e.what());
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
            result.error = "No algorithms registered";
            cleanup();
            return result;
        }
        
        // Execute game with timing
        auto startTime = std::chrono::high_resolution_clock::now();
        
        result.gameResult = GameRunner::runSingleGame(
            boardInfo,
            gameManagerName,
            algorithm1Name,
            algorithm2Name,
            false  // Not verbose for comparative mode
        );
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        result.success = true;
        
        // Store map dimensions for game state string conversion
        result.mapRows = boardInfo.rows;
        result.mapCols = boardInfo.cols;
        
        // Validate dimensions are properly populated
        if (result.mapRows <= 0 || result.mapCols <= 0) {
            result.error = "Invalid map dimensions: rows=" + std::to_string(result.mapRows) + ", cols=" + std::to_string(result.mapCols);
            result.success = false;
            cleanup();
            return result;
        }
        
        // Clean up for next iteration
        cleanup();
        
    } catch (const std::exception& e) {
        result.error = "Exception during game execution: " + std::string(e.what());
        result.success = false;
        cleanup();
    }
    
    return result;
}

void ComparativeRunner::generateOutput(const std::vector<ComparativeResult>& results, const std::string& outputPath, const ComparativeParameters& params) {
    std::ofstream outFile(outputPath);
    bool useConsole = false;
    
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot create output file: " << outputPath << std::endl;
        std::cerr << "Printing results to console instead:" << std::endl;
        useConsole = true;
    }
    
    std::ostream& output = useConsole ? std::cout : outFile;
    
    // TODO: make sure algorithm names are correct
    output << "game_map=" << params.mapFile << std::endl;
    output << "algorithm1=" << params.algorithm1Lib << std::endl;
    output << "algorithm2=" << params.algorithm2Lib << std::endl;
    output << std::endl;
    
    // Group all GameManagers by identical results
    auto resultGroups = groupResultsByOutcome(results);
    
    // Output each group
    bool firstGroup = true;
    for (const auto& group : resultGroups) {
        if (!firstGroup) {
            output << std::endl; // Empty line between groups
        }
        firstGroup = false;
        
        if (!group.first.empty() && group.second) {
            printResultGroup(output, group.first, group.second);
        }
    }
    
    if (!useConsole) {
        outFile.close();
    }
}

std::string ComparativeRunner::generateTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

void ComparativeRunner::cleanup() {
    GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    LibraryManager& libManager = LibraryManager::getInstance();
    
    gmRegistrar.clear();
    algoRegistrar.clear();
    libManager.unloadAllLibraries();
}

std::vector<std::pair<std::vector<std::string>, const ComparativeRunner::ComparativeResult*>> ComparativeRunner::groupResultsByOutcome(const std::vector<ComparativeResult>& results) {
    std::map<std::string, std::vector<std::string>> resultGroups;
    std::map<std::string, const ComparativeResult*> representativeResults;
    
    // Group results by identical outcomes
    for (const auto& result : results) {
        if (result.success) {
            // Create a key based on winner, reason, rounds, and final state
            std::string key = std::to_string(result.gameResult.winner) + "_" + 
                             std::to_string(static_cast<int>(result.gameResult.reason)) + "_" +
                             std::to_string(result.gameResult.rounds);
            
            // Add final state information if available
            if (result.gameResult.gameState) {
                // Use actual game state string for accurate comparison
                std::string gameStateStr = gameStateToString(*result.gameResult.gameState, result.mapRows, result.mapCols);
                key += "_" + gameStateStr;
            }
            
            resultGroups[key].push_back(result.gameManagerName);
            if (representativeResults.find(key) == representativeResults.end()) {
                representativeResults[key] = &result;
            }
        }
    }
    
    // Convert to vector of pairs for output
    std::vector<std::pair<std::vector<std::string>, const ComparativeResult*>> allGroups;
    for (const auto& group : resultGroups) {
        allGroups.emplace_back(group.second, representativeResults[group.first]);
    }
    
    return allGroups;
}

void ComparativeRunner::printResultGroup(std::ostream& output, const std::vector<std::string>& gameManagerNames, const ComparativeResult* representative) {
    // TODO: make sure algorithm names are correct
    bool first = true;
    for (const auto& name : gameManagerNames) {
        if (!first) output << ",";
        output << name;
        first = false;
    }
    output << std::endl;
    
    // TODO: make sure game result message is correct
    if (representative->gameResult.winner == 0) {
        output << "Tie";
    } else {
        output << "Player " << representative->gameResult.winner << " wins";
    }
    output << " - ";
    switch (representative->gameResult.reason) {
        case GameResult::ALL_TANKS_DEAD:
            output << "all tanks destroyed";
            break;
        case GameResult::MAX_STEPS:
            output << "maximum steps reached";
            break;
        case GameResult::ZERO_SHELLS:
            output << "no shells remaining";
            break;
    }
    output << std::endl;
    
    output << representative->gameResult.rounds << std::endl;
    
    if (representative->gameResult.gameState) {
        std::string gameStateStr = gameStateToString(*representative->gameResult.gameState, representative->mapRows, representative->mapCols);
        output << gameStateStr;
    }
}

std::string ComparativeRunner::gameStateToString(const SatelliteView& gameState, int rows, int cols) {
    std::string result;
    result.reserve(rows * cols + rows); // Pre-allocate for efficiency
    
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            result += gameState.getObjectAt(x, y);
        }
        if (y < rows - 1) {
            result += '\n';
        }
    }
    
    return result;
}