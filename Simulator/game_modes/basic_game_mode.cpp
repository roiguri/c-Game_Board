#include "basic_game_mode.h"
#include <iostream>
#include "utils/library_manager.h"
#include "registration/GameManagerRegistrar.h"
#include "registration/AlgorithmRegistrar.h"

BasicGameMode::BasicGameMode() = default;

BasicGameMode::~BasicGameMode() {
    cleanup();
}

GameResult BasicGameMode::execute(const GameParameters& params) {
    GameResult result;
    
    try {
        // Load libraries
        if (!loadLibraries(params)) {
            result.winner = -1;
            result.rounds = 0;
            result.reason = GameResult::ALL_TANKS_DEAD;
            return result;
        }
        
        // Load map
        if (!loadMap(params.mapFile)) {
            result.winner = -1;
            result.rounds = 0;
            result.reason = GameResult::ALL_TANKS_DEAD;
            cleanup();
            return result;
        }
        
        // Execute game
        if (params.verbose) {
            std::cout << "Starting game execution..." << std::endl;
        }
        
        result = GameRunner::runSingleGame(
            m_boardInfo,
            m_gameManagerName,
            m_algorithm1Name,
            m_algorithm2Name,
            params.verbose
        );
        
        // Display results
        if (params.verbose) {
            displayResults(result);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during game execution: " << e.what() << std::endl;
        result.winner = -1;
        result.rounds = 0;
        result.reason = GameResult::ALL_TANKS_DEAD;
        cleanup();
        return result;
    } catch (...) {
        std::cerr << "Unknown exception during game execution" << std::endl;
        result.winner = -1;
        result.rounds = 0;
        result.reason = GameResult::ALL_TANKS_DEAD;
        cleanup();
        return result;
    }
    
    cleanup();
    return result;
}

bool BasicGameMode::loadLibraries(const GameParameters& params) {
    LibraryManager& libManager = LibraryManager::getInstance();
    GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    try {
        // Load GameManager library
        std::cout << "Loading GameManager library: " << params.gameManagerLib << std::endl;
        gmRegistrar.createGameManagerEntry(params.gameManagerLib);
        if (!libManager.loadLibrary(params.gameManagerLib)) {
            std::cerr << "Error loading GameManager library: " << libManager.getLastError() << std::endl;
            gmRegistrar.removeLast();
            return false;
        }
        
        try {
            gmRegistrar.validateLastRegistration();
        } catch (const GameManagerRegistrar::BadGameManagerRegistrationException& e) {
            std::cerr << "Error: GameManager registration failed for " << e.name << std::endl;
            cleanup();
            return false;
        }
        
        // Load Algorithm 1 library
        std::cout << "Loading Algorithm 1 library: " << params.algorithm1Lib << std::endl;
        algoRegistrar.createAlgorithmFactoryEntry(params.algorithm1Lib);
        if (!libManager.loadLibrary(params.algorithm1Lib)) {
            std::cerr << "Error loading Algorithm 1 library: " << libManager.getLastError() << std::endl;
            algoRegistrar.removeLast();
            cleanup();
            return false;
        }
        
        try {
            algoRegistrar.validateLastRegistration();
        } catch (const BadRegistrationException& e) {
            std::cerr << "Error: Algorithm registration failed: " << e.what() << std::endl;
            cleanup();
            return false;
        }
        
        // Load Algorithm 2 library (if different)
        if (params.algorithm2Lib != params.algorithm1Lib) {
            std::cout << "Loading Algorithm 2 library: " << params.algorithm2Lib << std::endl;
            algoRegistrar.createAlgorithmFactoryEntry(params.algorithm2Lib);
            if (!libManager.loadLibrary(params.algorithm2Lib)) {
                std::cerr << "Error loading Algorithm 2 library: " << libManager.getLastError() << std::endl;
                algoRegistrar.removeLast();
                cleanup();
                return false;
            }
            
            try {
                algoRegistrar.validateLastRegistration();
            } catch (const BadRegistrationException& e) {
                std::cerr << "Error: Algorithm registration failed: " << e.what() << std::endl;
                cleanup();
                return false;
            }
        } else {
            std::cout << "Algorithm 2 is the same as Algorithm 1: " << params.algorithm1Lib << std::endl;
        }
        
        // Validate registrations
        if (gmRegistrar.count() == 0) {
            std::cerr << "Error: No GameManager registered after loading libraries" << std::endl;
            cleanup();
            return false;
        }
        
        if (algoRegistrar.size() == 0) {
            std::cerr << "Error: No algorithms registered after loading libraries" << std::endl;
            cleanup();
            return false;
        }
        
        // Get registered names
        auto gmEntry = gmRegistrar.begin();
        auto algoIter = algoRegistrar.begin();
        
        m_gameManagerName = gmEntry->name();
        m_algorithm1Name = algoIter->getName();
        
        if (algoRegistrar.size() > 1) {
            ++algoIter;
            m_algorithm2Name = algoIter->getName();
        } else {
            m_algorithm2Name = m_algorithm1Name;
        }
        
        std::cout << "Using GameManager: " << m_gameManagerName << std::endl;
        std::cout << "Using Algorithm 1: " << m_algorithm1Name << std::endl;
        std::cout << "Using Algorithm 2: " << m_algorithm2Name << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during library loading: " << e.what() << std::endl;
        cleanup();
        return false;
    }
}

bool BasicGameMode::loadMap(const std::string& mapFile) {
    try {
        std::cout << "Loading map file: " << mapFile << std::endl;
        m_boardInfo = FileLoader::loadBoardWithSatelliteView(mapFile);
        if (!m_boardInfo.satelliteView) {
            std::cerr << "Error: Failed to load map file: " << mapFile << std::endl;
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception during map loading: " << e.what() << std::endl;
        return false;
    }
}

void BasicGameMode::cleanup() {
    GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    LibraryManager& libManager = LibraryManager::getInstance();
    
    gmRegistrar.clear();
    algoRegistrar.clear();
    libManager.unloadAllLibraries();
}

void BasicGameMode::displayResults(const GameResult& result) {
    std::cout << "\nGame completed!" << std::endl;
    std::cout << "Winner: Player " << result.winner << std::endl;
    std::cout << "Total rounds: " << result.rounds << std::endl;
    std::cout << "Reason: ";
    switch (result.reason) {
        case GameResult::ALL_TANKS_DEAD:
            std::cout << "All tanks destroyed" << std::endl;
            break;
        case GameResult::MAX_STEPS:
            std::cout << "Maximum steps reached" << std::endl;
            break;
        case GameResult::ZERO_SHELLS:
            std::cout << "No shells remaining" << std::endl;
            break;
    }
    std::cout << "Remaining tanks per player: ";
    for (size_t i = 0; i < result.remaining_tanks.size(); ++i) {
        std::cout << "Player " << (i + 1) << ": " << result.remaining_tanks[i] << " ";
    }
    std::cout << std::endl;
}