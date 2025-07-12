#include "basic_game_mode.h"
#include <iostream>
#include "utils/library_manager.h"
#include "registration/GameManagerRegistrar.h"
#include "registration/AlgorithmRegistrar.h"

BasicGameMode::BasicGameMode() : BaseGameMode() {
}

BasicGameMode::~BasicGameMode() {
    // Base class destructor will handle cleanup
}

bool BasicGameMode::loadLibraries(const BaseParameters& params) {
    // Cast to derived parameter type
    const GameParameters* gameParams = dynamic_cast<const GameParameters*>(&params);
    if (!gameParams) {
        handleError("Invalid parameter type for BasicGameMode");
        return false;
    }
    
    return loadLibrariesImpl(*gameParams);
}

bool BasicGameMode::loadLibrariesImpl(const GameParameters& params) {
    LibraryManager& libManager = LibraryManager::getInstance();
    GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    try {
        // Load GameManager library
        gmRegistrar.createGameManagerEntry(params.gameManagerLib);
        if (!libManager.loadLibrary(params.gameManagerLib)) {
            handleError("Error loading GameManager library: " + libManager.getLastError());
            gmRegistrar.removeLast();
            return false;
        }
        
        try {
            gmRegistrar.validateLastRegistration();
        } catch (const GameManagerRegistrar::BadGameManagerRegistrationException& e) {
            handleError("GameManager registration failed for " + std::string(e.name));
            return false;
        }
        
        // Load Algorithm 1 library
        algoRegistrar.createAlgorithmFactoryEntry(params.algorithm1Lib);
        if (!libManager.loadLibrary(params.algorithm1Lib)) {
            handleError("Error loading Algorithm 1 library: " + libManager.getLastError());
            algoRegistrar.removeLast();
            return false;
        }
        
        try {
            algoRegistrar.validateLastRegistration();
        } catch (const BadRegistrationException& e) {
            handleError("Algorithm registration failed: " + std::string(e.what()));
            return false;
        }
        
        // Load Algorithm 2 library (if different)
        if (params.algorithm2Lib != params.algorithm1Lib) {
            algoRegistrar.createAlgorithmFactoryEntry(params.algorithm2Lib);
            if (!libManager.loadLibrary(params.algorithm2Lib)) {
                handleError("Error loading Algorithm 2 library: " + libManager.getLastError());
                algoRegistrar.removeLast();
                return false;
            }
            
            try {
                algoRegistrar.validateLastRegistration();
            } catch (const BadRegistrationException& e) {
                handleError("Algorithm registration failed: " + std::string(e.what()));
                return false;
            }
        } else {
            // Algorithm 2 is the same as Algorithm 1
        }
        
        // Validate registrations
        if (gmRegistrar.count() == 0) {
            handleError("No GameManager registered after loading libraries");
            return false;
        }
        
        if (algoRegistrar.size() == 0) {
            handleError("No algorithms registered after loading libraries");
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
        
        return true;
        
    } catch (const std::exception& e) {
        handleError("Exception during library loading: " + std::string(e.what()));
        return false;
    }
}

bool BasicGameMode::loadMap(const std::string& mapFile) {
    m_boardInfo = FileLoader::loadBoardWithSatelliteView(mapFile);
    
    if (!m_boardInfo.satelliteView) {
        handleError("Failed to load map file: " + mapFile);
        return false;
    }

    if (!m_boardInfo.isValid()) {
        handleError("Board validation failed: " + m_boardInfo.getErrorReason());
        return false;
    }

    auto warnings = m_boardInfo.getWarnings();
    if (!warnings.empty()) {
        if (!saveErrorsToFile(warnings)) {
            std::cerr << "Warning: Could not save warnings to input_errors.txt file, continuing without it" << std::endl;
        }
    }

    return true;
}

GameResult BasicGameMode::executeGameLogic(const BaseParameters& params) {
    const GameParameters* gameParams = dynamic_cast<const GameParameters*>(&params);
    if (!gameParams) {
        handleError("Invalid parameter type for BasicGameMode");
        return createErrorResult();
    }
    
    return GameRunner::runSingleGame(
        m_boardInfo,
        m_gameManagerName,
        m_algorithm1Name,
        m_algorithm2Name,
        params.verbose
    );
}

void BasicGameMode::cleanup() {
    GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    gmRegistrar.clear();
    algoRegistrar.clear();
    m_libraryManager.unloadAllLibraries();
    
    // Clean up board info
    m_boardInfo.satelliteView.reset();
    
    // Call base class cleanup
    BaseGameMode::cleanup();
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