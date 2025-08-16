#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "game_runner.h"
#include "utils/file_loader.h"
#include "utils/library_manager.h"
#include "registration/GameManagerRegistrar.h"
#include "registration/AlgorithmRegistrar.h"
#include "utils/command_line_parser.h"
#include "game_modes/basic_game_mode.h"
#include "game_modes/comparative_runner.h"
#include "game_modes/competitive_runner.h"

void printUsage(const char* programName) {
    CommandLineParser parser;
    std::cout << parser.generateUsage(programName) << std::endl;
}

int executeBasicMode(const CommandLineParser& parser) {
    BasicGameMode gameMode;
    BasicGameMode::GameParameters params;
    
    params.mapFile = parser.getMapFile();
    params.gameManagerLib = parser.getGameManagerLib();
    params.algorithm1Lib = parser.getAlgorithm1Lib();
    params.algorithm2Lib = parser.getAlgorithm2Lib();
    params.verbose = parser.isVerbose();
    
    GameResult result = gameMode.execute(params);
    
    if (result.winner == -1) {
        return 1; // Game execution failed
    }
    
    // Game execution completed successfully
    return 0;
}

int executeComparativeMode(const CommandLineParser& parser) {
    ComparativeRunner runner;
    
    ComparativeRunner::ComparativeParameters params;
    
    params.mapFile = parser.getGameMap();
    params.gameManagersFolder = parser.getGameManagersFolder();
    params.algorithm1Lib = parser.getAlgorithm1();
    params.algorithm2Lib = parser.getAlgorithm2();
    params.verbose = parser.isVerbose();
    
    // Optional: num_threads parameter
    auto numThreads = parser.getNumThreads();
    if (numThreads.has_value()) {
        params.numThreads = numThreads.value();
    }
    
    const auto& results = runner.runComparative(params);
    
    if (results.empty()) {
        return 1; // Comparative execution failed
    }    
    return 0;
}

int executeCompetitionMode(const CommandLineParser& parser) {
    CompetitiveRunner runner;
    CompetitiveRunner::CompetitiveParameters params;
    
    params.gameMapsFolder = parser.getGameMapsFolder();
    params.gameManagerLib = parser.getGameManager();
    params.algorithmsFolder = parser.getAlgorithmsFolder();
    params.verbose = parser.isVerbose();
    
    // Optional: num_threads parameter
    auto numThreads = parser.getNumThreads();
    if (numThreads.has_value()) {
        params.numThreads = numThreads.value();
    }
    
    const auto& scores = runner.runCompetition(params);
    
    if (scores.empty()) {
        return 1; // Competition execution failed
    }    
    return 0;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    CommandLineParser parser;
    CommandLineParser::ParseResult parseResult = parser.parse(argc, argv);
    
    if (!parseResult.success) {
        std::cerr << "Error: " << parseResult.errorMessage << std::endl;
        
        if (!parseResult.missingParams.empty()) {
            std::cerr << "Missing required parameters: ";
            for (const auto& param : parseResult.missingParams) {
                std::cerr << param << " ";
            }
            std::cerr << std::endl;
        }
        
        if (!parseResult.unsupportedParams.empty()) {
            std::cerr << "Unsupported parameters: ";
            for (const auto& param : parseResult.unsupportedParams) {
                std::cerr << param << " ";
            }
            std::cerr << std::endl;
        }
        
        printUsage(argv[0]);
        return 1;
    }
    
    int result = 0;
    switch (parseResult.mode) {
        case CommandLineParser::Mode::Basic:
            result = executeBasicMode(parser);
            break;
            
        case CommandLineParser::Mode::Comparative:
            result = executeComparativeMode(parser);
            break;
            
        case CommandLineParser::Mode::Competition:
            result = executeCompetitionMode(parser);
            break;
            
        default:
            std::cerr << "Error: Unknown mode detected" << std::endl;
            printUsage(argv[0]);
            return 1;
    }
    // Execution completed
    return result;
}