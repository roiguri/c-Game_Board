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

namespace {
    constexpr int DEFAULT_NO_SHELLS_STEPS = 40;
}

ComparativeRunner::ComparativeRunner() : BaseGameMode() {
}

ComparativeRunner::~ComparativeRunner() {
    // Base class destructor will handle cleanup
}

bool ComparativeRunner::loadLibraries(const BaseParameters& params) {
    // Cast to derived parameter type
    const ComparativeParameters* comparativeParams = dynamic_cast<const ComparativeParameters*>(&params);
    if (!comparativeParams) {
        // Should not happen
        std::cout << "Error: Unexpected exception during library loading" << std::endl;
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
      // Print usage and exit - insufficient GameManagers for comparative mode
      std::cout << "Error: Comparative mode requires at least 2 working GameManager libraries." << std::endl;
      std::cout << "Found 0 .so files in given game_managers_folder" << std::endl;
      std::cout << "Usage:" << std::endl;
      std::cout << "  Place at least 2 valid GameManager .so files in the game_managers_folder" << std::endl;
      std::cout << "  Check input_errors.txt for detailed loading errors" << std::endl;
      
      return false;
    }
    
    // Load and validate each GameManager, collecting errors for failed ones
    m_discoveredGameManagers.clear();
    for (const auto& soPath : soFiles) {
        GameManagerInfo info = loadGameManager(soPath);
        if (info.loaded) {
            m_discoveredGameManagers.push_back(info);
        } else {
            // Collect GameManager loading error instead of failing immediately
            m_errorCollector.addGameManagerError(soPath, info.error);
        }
    }
    
    // Check minimum requirements for comparative mode: need at least 2 GameManagers
    if (m_discoveredGameManagers.size() < 2) {
        // Print usage and exit - insufficient GameManagers for comparative mode
        std::cout << "Error: Comparative mode requires at least 2 working GameManager libraries." << std::endl;
        std::cout << "Found " << m_discoveredGameManagers.size() << " working GameManager(s) out of " << soFiles.size() << " total." << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  Place at least 2 valid GameManager .so files in the game_managers_folder" << std::endl;
        std::cout << "  Check input_errors.txt for detailed loading errors" << std::endl;
        
        return false;
    }

    // Load and register both algorithms ONCE
    LibraryManager& libManager = LibraryManager::getInstance();
    AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    algoRegistrar.clear();
    
    // Algorithm 1
    algoRegistrar.createAlgorithmFactoryEntry(params.algorithm1Lib);
    if (!libManager.loadLibrary(params.algorithm1Lib)) {
        // For comparative mode, both algorithms are required - exit with usage if either fails
        std::cout << "Error: Comparative mode requires both algorithms to load successfully." << std::endl;
        std::cout << "Failed to load Algorithm 1: " + libManager.getLastError() << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  Ensure both algorithm1= and algorithm2= point to valid Algorithm .so files" << std::endl;
        
        algoRegistrar.removeLast();
        return false;
    }
    try {
        algoRegistrar.validateLastRegistration();
    } catch (const BadRegistrationException& e) {
        std::cout << "Error: Comparative mode requires both algorithms to register successfully." << std::endl;
        std::cout << "Algorithm 1 registration failed: " + std::string(e.what()) << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  Ensure both algorithm1= and algorithm2= point to valid Algorithm .so files" << std::endl;
        
        return false;
    }
    
    // Algorithm 2 (if different)
    if (params.algorithm2Lib != params.algorithm1Lib) {
        algoRegistrar.createAlgorithmFactoryEntry(params.algorithm2Lib);
        if (!libManager.loadLibrary(params.algorithm2Lib)) {
            std::cout << "Error: Comparative mode requires both algorithms to load successfully." << std::endl;
            std::cout << "Failed to load Algorithm 2: " + libManager.getLastError() << std::endl;
            std::cout << "Usage:" << std::endl;
            std::cout << "  Ensure both algorithm1= and algorithm2= point to valid Algorithm .so files" << std::endl;
            algoRegistrar.removeLast();
            return false;
        }
        try {
            algoRegistrar.validateLastRegistration();
        } catch (const BadRegistrationException& e) {
            std::cout << "Error: Comparative mode requires both algorithms to register successfully." << std::endl;
            std::cout << "Algorithm 2 registration failed: " + std::string(e.what()) << std::endl;
            std::cout << "Usage:" << std::endl;
            std::cout << "  Ensure both algorithm1= and algorithm2= point to valid Algorithm .so files" << std::endl;
            
            return false;
        }
    } else {
        // Algorithm 2 is the same as Algorithm 1
    }
    return true;
}

bool ComparativeRunner::loadMap(const std::string& mapFile) {
    m_boardInfo = FileLoader::loadBoardWithSatelliteView(mapFile, m_errorCollector);
    
    if (!m_boardInfo.satelliteView) {
        std::cout << "Error: Failed to load map file: " + mapFile << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  Ensure map file contains valid .txt map file" << std::endl;
        return false;
    }

    // Check validation status using new validation interface
    if (!m_boardInfo.isValid()) {
        // Early exit on invalid maps - print error and exit immediately without creating error file
        std::cout << "Error: Board validation failed: " + m_boardInfo.getErrorReason() << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  Ensure map file contains valid .txt map file" << std::endl;
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
        // Should not happen
        std::cout << "Error: Unexpected exception during game execution" << std::endl;
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
    
    // Submit tasks to thread pool for parallel execution using loaded GameManager info
    for (const auto& info : m_discoveredGameManagers) {
        if (info.loaded) {
            auto future = threadPool.enqueue([this, info, comparativeParams]() {
                return executeWithGameManager(info, *comparativeParams, m_boardInfo);
            });
            futures.push_back(std::move(future));
        }
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
            errorResult.error = "Error: Unexpected exception during thread execution: " + std::string(e.what());
            std::lock_guard<std::mutex> lock(m_resultsMutex);
            m_results.push_back(std::move(errorResult));
        }
    }
    
    // Return a summary result
    if (!m_results.empty()) {
        // Use the first successful result as the summary
        for (const auto& result : m_results) {
            if (result.success) {
                GameResult summary;
                summary.winner = result.gameResult.winner;
                summary.rounds = result.gameResult.rounds;
                summary.reason = result.gameResult.reason;
                summary.remaining_tanks = result.gameResult.remaining_tanks;
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
    }
}

const std::vector<ComparativeRunner::GameManagerInfo>& ComparativeRunner::getDiscoveredGameManagers() const {
    return m_discoveredGameManagers;
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
            return info;
        }
        
        // Get GameManager name
        if (gmRegistrar.count() > 0) {
            auto lastEntry = gmRegistrar.begin();
            std::advance(lastEntry, gmRegistrar.count() - 1);
            info.name = lastEntry->name();
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
    const GameManagerInfo& gameManagerInfo,
    const ComparativeParameters& params,
    const FileLoader::BoardInfo& boardInfo) {
    ComparativeResult result;
    result.gameManagerName = gameManagerInfo.name;
    result.success = false;
    result.executionTime = std::chrono::milliseconds(0);

    // Use algorithm file paths as names
    std::string algorithm1Name = params.algorithm1Lib;
    std::string algorithm2Name = params.algorithm2Lib;
    
    // Execute game with timing (GameRunner handles GameManager loading internally)
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        result.gameResult = GameRunner::runSingleGame(
            boardInfo,
            gameManagerInfo.name,
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
    
    output << "game_map=" << extractFileName(params.mapFile) << std::endl;
    output << "algorithm1=" << extractFileName(params.algorithm1Lib) << std::endl;
    output << "algorithm2=" << extractFileName(params.algorithm2Lib) << std::endl;
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
    
    // Sort groups by size (most common results first)
    std::sort(allGroups.begin(), allGroups.end(), 
        [](const auto& a, const auto& b) {
            return a.first.size() > b.first.size();
        });
    
    return allGroups;
}

void ComparativeRunner::printResultGroup(std::ostream& output, const std::vector<std::string>& gameManagerNames, const ComparativeResult* representative) {
    bool first = true;
    for (const auto& name : gameManagerNames) {
        if (!first) output << ",";
        output << extractFileName(name);
        first = false;
    }
    output << std::endl;
    
    std::string gameResult;
    const auto& result = representative->gameResult;
    
    if (result.reason == GameResult::ALL_TANKS_DEAD) {
        if (result.winner == 0) {
            gameResult = "Tie, both players have zero tanks";
        } else {
            size_t winnerTanks = result.remaining_tanks[result.winner - 1];
            gameResult = "Player " + std::to_string(result.winner) + " won with " + std::to_string(winnerTanks) + " tanks still alive";
        }
    } else if (result.reason == GameResult::ZERO_SHELLS) {
        gameResult = "Tie, both players have zero shells for " + std::to_string(DEFAULT_NO_SHELLS_STEPS) + " steps";
    } else if (result.reason == GameResult::MAX_STEPS) {
        // Get tank counts for both players
        size_t player1Tanks = result.remaining_tanks.size() >= 1 ? result.remaining_tanks[0] : 0;
        size_t player2Tanks = result.remaining_tanks.size() >= 2 ? result.remaining_tanks[1] : 0;
        gameResult = "Tie, reached max steps = " + std::to_string(result.rounds) + 
                    ", player 1 has " + std::to_string(player1Tanks) + 
                    " tanks, player 2 has " + std::to_string(player2Tanks) + " tanks";
    }
    
    output << gameResult << std::endl;
    
    output << representative->gameResult.rounds << std::endl;
    
    if (representative->gameResult.gameState) {
        std::string gameStateStr = gameStateToString(*representative->gameResult.gameState, representative->mapRows, representative->mapCols);
        output << gameStateStr << std::endl;
    }
}

std::string ComparativeRunner::gameStateToString(const SatelliteView& gameState, int rows, int cols) {
    std::string result;
    result.reserve(rows * cols + rows);
    
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