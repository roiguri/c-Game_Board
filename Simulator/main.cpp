#include <iostream>
#include <string>
#include <vector>

#include "game_runner.h"
#include "utils/file_loader.h"
#include "utils/library_manager.h"
#include "registration/GameManagerRegistrar.h"
#include "registration/AlgorithmRegistrar.h"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <map_file> <gamemanager.so> <algo1.so> <algo2.so>\n";
    std::cout << "  map_file        Path to the board/map file\n";
    std::cout << "  gamemanager.so  Path to the GameManager shared library\n";
    std::cout << "  algo1.so        Path to the first algorithm shared library\n";
    std::cout << "  algo2.so        Path to the second algorithm shared library\n";
}

bool validateArguments(int argc, char* /* argv */[]) {
    if (argc != 5) {
        std::cerr << "Error: Expected exactly 4 arguments, got " << (argc - 1) << std::endl;
        return false;
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    // Validate command line arguments
    if (!validateArguments(argc, argv)) {
        printUsage(argv[0]);
        return 1;
    }
    
    const std::string mapFile = argv[1];
    const std::string gameManagerLib = argv[2];
    const std::string algo1Lib = argv[3];
    const std::string algo2Lib = argv[4];
    
    // Get LibraryManager instance
    LibraryManager& libManager = LibraryManager::getInstance();
    
    // Get registrars 
    GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    try {
        
        // Load GameManager library and trigger registration
        std::cout << "Loading GameManager library: " << gameManagerLib << std::endl;
        gmRegistrar.createGameManagerEntry(gameManagerLib);
        if (!libManager.loadLibrary(gameManagerLib)) {
            std::cerr << "Error loading GameManager library: " << libManager.getLastError() << std::endl;
            gmRegistrar.removeLast();
            return 1;
        }
        try {
            gmRegistrar.validateLastRegistration();
        } catch (const GameManagerRegistrar::BadGameManagerRegistrationException& e) {
            std::cerr << "Error: GameManager registration failed for " << e.name << std::endl;
            gmRegistrar.clear();
            algoRegistrar.clear();
            libManager.unloadAllLibraries();
            return 1;
        }
        
        // Load Algorithm 1 library and trigger registration
        std::cout << "Loading Algorithm 1 library: " << algo1Lib << std::endl;
        algoRegistrar.createAlgorithmFactoryEntry(algo1Lib);
        if (!libManager.loadLibrary(algo1Lib)) {
            std::cerr << "Error loading Algorithm 1 library: " << libManager.getLastError() << std::endl;
            algoRegistrar.removeLast();
            gmRegistrar.clear();
            algoRegistrar.clear();
            libManager.unloadAllLibraries();
            return 1;
        }
        try {
            algoRegistrar.validateLastRegistration();
        } catch (const BadRegistrationException& e) {
            std::cerr << "Error: Algorithm registration failed: " << e.what() << std::endl;
            gmRegistrar.clear();
            algoRegistrar.clear();
            libManager.unloadAllLibraries();
            return 1;
        }
        
        // Load Algorithm 2 library and trigger registration (if different from Algorithm 1)
        if (algo2Lib != algo1Lib) {
            std::cout << "Loading Algorithm 2 library: " << algo2Lib << std::endl;
            algoRegistrar.createAlgorithmFactoryEntry(algo2Lib);
            if (!libManager.loadLibrary(algo2Lib)) {
                std::cerr << "Error loading Algorithm 2 library: " << libManager.getLastError() << std::endl;
                algoRegistrar.removeLast();
                gmRegistrar.clear();
                algoRegistrar.clear();
                libManager.unloadAllLibraries();
                return 1;
            }
            try {
                algoRegistrar.validateLastRegistration();
            } catch (const BadRegistrationException& e) {
                std::cerr << "Error: Algorithm registration failed: " << e.what() << std::endl;
                gmRegistrar.clear();
                algoRegistrar.clear();
                libManager.unloadAllLibraries();
                return 1;
            }
        } else {
            std::cout << "Algorithm 2 is the same as Algorithm 1: " << algo1Lib << std::endl;
        }
        
        // Load map file
        std::cout << "Loading map file: " << mapFile << std::endl;
        FileLoader::BoardInfo boardInfo = FileLoader::loadBoardWithSatelliteView(mapFile);
        if (!boardInfo.satelliteView) {
            std::cerr << "Error: Failed to load map file: " << mapFile << std::endl;
            gmRegistrar.clear();
            algoRegistrar.clear();
            libManager.unloadAllLibraries();
            return 1;
        }
        
        // Validate that we have the required registrations
        if (gmRegistrar.count() == 0) {
            std::cerr << "Error: No GameManager registered after loading libraries" << std::endl;
            gmRegistrar.clear();
            algoRegistrar.clear();
            libManager.unloadAllLibraries();
            return 1;
        }
        
        if (algoRegistrar.size() == 0) {
            std::cerr << "Error: No algorithms registered after loading libraries" << std::endl;
            gmRegistrar.clear();
            algoRegistrar.clear();
            libManager.unloadAllLibraries();
            return 1;
        }
        
        // Use the first registered GameManager and algorithm(s)
        auto gmEntry = gmRegistrar.begin();
        auto algoIter = algoRegistrar.begin();
        
        const std::string gameManagerName = gmEntry->name();
        const std::string algorithm1Name = algoIter->getName();
        
        // For the second algorithm, use the next one if available, otherwise reuse the first
        std::string algorithm2Name;
        if (algoRegistrar.size() > 1) {
            ++algoIter;
            algorithm2Name = algoIter->getName();
        } else {
            algorithm2Name = algorithm1Name;
        }
        
        std::cout << "Using GameManager: " << gameManagerName << std::endl;
        std::cout << "Using Algorithm 1: " << algorithm1Name << std::endl;
        std::cout << "Using Algorithm 2: " << algorithm2Name << std::endl;
        
        std::cout << "Starting game execution..." << std::endl;
        GameResult result = GameRunner::runSingleGame(
            boardInfo,
            gameManagerName,
            algorithm1Name,
            algorithm2Name,
            true // verbose output
        );
        
        // Display game results
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
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during game execution: " << e.what() << std::endl;
        gmRegistrar.clear();
        algoRegistrar.clear();
        libManager.unloadAllLibraries();
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception during game execution" << std::endl;
        gmRegistrar.clear();
        algoRegistrar.clear();
        libManager.unloadAllLibraries();
        return 1;
    }
    
    // Cleanup - clear registrars first, then unload libraries
    std::cout << "Cleaning up libraries..." << std::endl;
    gmRegistrar.clear();
    algoRegistrar.clear();
    libManager.unloadAllLibraries();
    
    std::cout << "Game execution completed successfully." << std::endl;
    return 0;
}