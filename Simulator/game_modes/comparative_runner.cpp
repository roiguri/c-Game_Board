#include "comparative_runner.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <map>
#include <memory>
#include <future>
#include <filesystem>
#include "utils/library_manager.h"
#include "registration/GameManagerRegistrar.h"
#include "registration/AlgorithmRegistrar.h"

ComparativeRunner::ComparativeRunner() : BaseGameMode() {
}

ComparativeRunner::~ComparativeRunner() {
    // Base class destructor will handle cleanup
}

bool ComparativeRunner::loadLibraries(const BaseParameters& params) {
    // Cast to derived parameter type
    const ComparativeParameters* comparativeParams = dynamic_cast<const ComparativeParameters*>(&params);
    if (!comparativeParams) {
        handleError("Invalid parameter type for ComparativeRunner");
        return false;
    }
    
    // Store parameters for later use
    m_currentParams = std::make_unique<ComparativeParameters>(*comparativeParams);
    
    return loadLibrariesImpl(*comparativeParams);
}

bool ComparativeRunner::loadLibrariesImpl(const ComparativeParameters& params) {
    // Enumerate .so files in GameManagers folder
    
    std::vector<std::string> soFiles = enumerateFiles(params.gameManagersFolder, ".so");
    if (soFiles.empty()) {
        handleError("No .so files found in directory: " + params.gameManagersFolder);
        return false;
    }
    
    // Found .so files
    
    // Load and validate each GameManager (just store paths for now)
    for (const auto& soPath : soFiles) {
        GameManagerInfo info;
        info.path = soPath;
        info.loaded = true; // We'll load per run
        info.name = soPath;
        m_discoveredGameManagers.push_back(info);
    }

    // Load and register both algorithms ONCE
    LibraryManager& libManager = LibraryManager::getInstance();
    AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    algoRegistrar.clear();
    
    // Algorithm 1
    algoRegistrar.createAlgorithmFactoryEntry(params.algorithm1Lib);
    if (!libManager.loadLibrary(params.algorithm1Lib)) {
        handleError("Failed to load Algorithm 1: " + libManager.getLastError());
        algoRegistrar.removeLast();
        return false;
    }
    try {
        algoRegistrar.validateLastRegistration();
        // Algorithm 1 registration successful
    } catch (const BadRegistrationException& e) {
        handleError("Algorithm 1 registration failed: " + std::string(e.what()));
        return false;
    }
    // Algorithm 2 (if different)
    if (params.algorithm2Lib != params.algorithm1Lib) {
        algoRegistrar.createAlgorithmFactoryEntry(params.algorithm2Lib);
        if (!libManager.loadLibrary(params.algorithm2Lib)) {
            handleError("Failed to load Algorithm 2: " + libManager.getLastError());
            algoRegistrar.removeLast();
            return false;
        }
        try {
            algoRegistrar.validateLastRegistration();
            // Algorithm 2 registration successful
        } catch (const BadRegistrationException& e) {
            handleError("Algorithm 2 registration failed: " + std::string(e.what()));
            return false;
        }
    } else {
        // Algorithm 2 is the same as Algorithm 1
    }
    return true;
}

bool ComparativeRunner::loadMap(const std::string& mapFile) {
    m_boardInfo = FileLoader::loadBoardWithSatelliteView(mapFile);
    
    if (!m_boardInfo.satelliteView) {
        handleError("Failed to load map file: " + mapFile);
        return false;
    }

    // Check validation status using new validation interface
    if (!m_boardInfo.isValid()) {
        // Early exit on invalid maps - print error and exit immediately without creating error file
        handleError("Board validation failed: " + m_boardInfo.getErrorReason());
        return false;
    }

    // Handle warnings (board is valid but has issues) - collect for later error file creation
    auto warnings = m_boardInfo.getWarnings();
    if (!warnings.empty()) {
        m_errorCollector.addMapWarnings(m_boardInfo.mapName, warnings);
    }

    return true;
}

GameResult ComparativeRunner::executeGameLogic(const BaseParameters& params) {
    const ComparativeParameters* comparativeParams = dynamic_cast<const ComparativeParameters*>(&params);
    if (!comparativeParams) {
        handleError("Invalid parameter type for ComparativeRunner");
        return createErrorResult();
    }
    
    m_results.clear();
    
    // Save collected errors to file before starting game execution
    if (m_errorCollector.hasErrors()) {
        if (!m_errorCollector.saveToFile()) {
            // Log warning but continue - don't crash if error file creation fails
            std::cerr << "Warning: Could not save warnings to input_errors.txt file, continuing without it" << std::endl;
        }
    }
    
    // Create ThreadPool based on parameters
    size_t numThreads = comparativeParams->numThreads;

    ThreadPool threadPool(numThreads);
    std::vector<std::future<ComparativeResult>> futures;
    
    // Pre-load all GameManagers to avoid concurrent loading issues
    std::vector<std::string> validGameManagers;
    for (const auto& info : m_discoveredGameManagers) {
        if (info.loaded) {
            validGameManagers.push_back(info.name);
        }
    }

    // Submit tasks to thread pool for parallel execution
    for (const auto& gameManagerName : validGameManagers) {
        auto future = threadPool.enqueue([this, gameManagerName, comparativeParams]() {
            return executeWithGameManager(gameManagerName, *comparativeParams, m_boardInfo);
        });
        futures.push_back(std::move(future));
    }
    
    // Collect results from all threads
    for (auto& future : futures) {
        try {
            ComparativeResult result = future.get();
            std::lock_guard<std::mutex> lock(m_resultsMutex);
            m_results.push_back(std::move(result));
        } catch (const std::exception& e) {
            // Handle individual task failure
            ComparativeResult errorResult;
            errorResult.success = false;
            errorResult.error = "Thread execution failed: " + std::string(e.what());
            std::lock_guard<std::mutex> lock(m_resultsMutex);
            m_results.push_back(std::move(errorResult));
        }
    }
    
    // Return a summary result
    if (!m_results.empty()) {
        // Use the first successful result as the summary
        for (const auto& result : m_results) {
            if (result.success) {
                // Can't copy GameResult due to unique_ptr, so create a simple success indicator
                GameResult summary;
                summary.winner = result.gameResult.winner;
                summary.rounds = result.gameResult.rounds;
                summary.reason = result.gameResult.reason;
                summary.remaining_tanks = result.gameResult.remaining_tanks;
                // Don't copy gameState (unique_ptr)
                return summary;
            }
        }
    }
    
    return createErrorResult();
}

void ComparativeRunner::displayResults(const GameResult& /* result */) {
    // Generate output file with all results
    if (!m_results.empty() && m_currentParams) {
        std::string outputPath = m_currentParams->gameManagersFolder + "/comparative_results_" + generateTimestamp() + ".txt";
        generateOutput(m_results, outputPath, *m_currentParams);
        
        // Results written to file
    }
}

const std::vector<ComparativeRunner::GameManagerInfo>& ComparativeRunner::getDiscoveredGameManagers() const {
    return m_discoveredGameManagers;
}

// Remove enumerateSoFiles - using base class enumerateFiles method

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
            return info;
        }
        
        // Get GameManager name
        if (gmRegistrar.count() > 0) {
            auto gmEntry = gmRegistrar.begin();
            info.name = gmEntry->name();
            info.loaded = true;
        } else {
            info.error = "No GameManager registered after loading";
            return info;
        }
        
    } catch (const std::exception& e) {
        info.error = "Exception during GameManager loading: " + std::string(e.what());
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

    // Use algorithm file paths as names
    std::string algorithm1Name = params.algorithm1Lib;
    std::string algorithm2Name = params.algorithm2Lib;
    
    // Execute game with timing (GameRunner handles GameManager loading internally)
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Synchronize only the GameManager loading/registration part
        {
            std::lock_guard<std::mutex> gmLock(m_gameManagerMutex);
            
            // Clear and load GameManager registrar for this run
            GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
            gmRegistrar.clear();
            gmRegistrar.createGameManagerEntry(gameManagerName);
            LibraryManager& libManager = LibraryManager::getInstance();
            if (!libManager.loadLibrary(gameManagerName)) {
                result.error = "Failed to load GameManager: " + libManager.getLastError();
                gmRegistrar.removeLast();
                return result;
            }
            try {
                gmRegistrar.validateLastRegistration();
                // GameManager registration successful
            } catch (const GameManagerRegistrar::BadGameManagerRegistrationException& e) {
                result.error = "GameManager registration failed: " + std::string(e.name);
                return result;
            }
        } // Release lock before game execution
        
        result.gameResult = GameRunner::runSingleGame(
            boardInfo,
            gameManagerName,
            algorithm1Name,
            algorithm2Name,
            params.verbose
        );
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        result.success = true;
        // Store map dimensions for game state string conversion
        result.mapRows = boardInfo.rows;
        result.mapCols = boardInfo.cols;
        // Game completed with GameManager
        // Validate dimensions are properly populated
        if (result.mapRows <= 0 || result.mapCols <= 0) {
            result.error = "Invalid map dimensions: rows=" + std::to_string(result.mapRows) + ", cols=" + std::to_string(result.mapCols);
            result.success = false;
            return result;
        }
    } catch (const std::exception& e) {
        result.error = "Exception during game execution: " + std::string(e.what());
        result.success = false;
    }
    
    // Synchronize GameManager cleanup
    {
        std::lock_guard<std::mutex> gmLock(m_gameManagerMutex);
        GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
        gmRegistrar.clear();
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

// Remove custom generateTimestamp - using base class method

void ComparativeRunner::cleanup() {
    GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    gmRegistrar.clear();
    algoRegistrar.clear();
    // Commented out to prevent segmentation fault - LibraryManager singleton handles cleanup at program exit
    // m_libraryManager.unloadAllLibraries();
    
    // Clean up board info
    m_boardInfo.satelliteView.reset();
    
    m_discoveredGameManagers.clear();
    m_results.clear();
    m_currentParams.reset();
    BaseGameMode::cleanup();
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