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
    
    // TODO: consider removing this
    std::cout << "Game execution completed successfully." << std::endl;
    return 0;
}

int executeComparativeMode(const CommandLineParser& /* parser */) {
    throw std::runtime_error("Comparative mode is not yet implemented");
}

int executeCompetitionMode(const CommandLineParser& /* parser */) {
    throw std::runtime_error("Competition mode is not yet implemented");
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    CommandLineParser parser;
    CommandLineParser::ParseResult parseResult = parser.parse(argc, argv);
    
    // TODO: consider moving to cli parser
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
    
    switch (parseResult.mode) {
        case CommandLineParser::Mode::Basic:
            return executeBasicMode(parser);
            
        case CommandLineParser::Mode::Comparative:
            return executeComparativeMode(parser);
            
        case CommandLineParser::Mode::Competition:
            return executeCompetitionMode(parser);
            
        default:
            std::cerr << "Error: Unknown mode detected" << std::endl;
            printUsage(argv[0]);
            return 1;
    }
}