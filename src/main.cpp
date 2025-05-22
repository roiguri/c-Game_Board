#include "bonus/board_generator.h"
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include "bonus/cli/cli_parser.h" // Added include
#include "game_manager.h"
#include "algo/basic_tank_algorithm_factory.h"
#include "players/basic_player_factory.h"
#include "bonus/logger/logger_config.h"

// Run the game with a specified board file
int runGameWithFile(const std::string& filePath) {
    BasicPlayerFactory playerFactory;
    BasicTankAlgorithmFactory algoFactory;
    GameManager gameManager(playerFactory, algoFactory);
    if (!gameManager.readBoard(filePath)) {
        return 1;
    }
    
    gameManager.run();
    
    return 0;
}

// Generate a board with the given configuration
int generateBoard(const std::string& configPath, bool saveToFile) {
    LOG_INFO("Generating board with configuration file: " + configPath);

    BoardGenerator generator;
    if (!configPath.empty()) {
        if (!generator.loadConfig(configPath)) {
            LOG_ERROR("Failed to load configuration file: " + configPath);
            return 1;
        }
        LOG_INFO("Using configuration from: " + configPath);
    } else {
        LOG_INFO("Using default board configuration");
    }
    
    if (!generator.generateBoard()) {
        LOG_ERROR("Failed to generate board");
        return 1;
    }
    
    if (saveToFile) {
        std::string outputPath = "generated_board.txt";
        if (generator.saveToFile(outputPath)) {
            LOG_INFO("Board generated and saved to " + outputPath);
            return 0;
        } else {
            LOG_ERROR("Failed to save generated board");
            return 1;
        }
    }
    
    return 0;
}

// Generate a board and run the game with it
int generateAndRunGame(const std::string& configPath) {
    BoardGenerator generator;
    if (!configPath.empty()) {
        if (!generator.loadConfig(configPath)) {
            LOG_ERROR("Failed to load configuration file: " + configPath);
            return 1;
        }
        LOG_INFO("Using configuration from: " + configPath);
    } else {
        LOG_INFO("Using default board configuration");
    }
    
    if (!generator.generateBoard()) {
        LOG_ERROR("Failed to generate board");
        return 1;
    }
    
    // Create a temporary file for the generated board
    std::string tempFilePath = "temp_board.txt";
    if (!generator.saveToFile(tempFilePath)) {
        LOG_ERROR("Failed to create temporary board file");
        return 1;
    }
    
    BasicPlayerFactory playerFactory;
    BasicTankAlgorithmFactory algoFactory;
    GameManager gameManager(playerFactory, algoFactory);
    if (!gameManager.readBoard(tempFilePath)) {
        std::filesystem::remove(tempFilePath);
        return 1;
    }
    
    gameManager.run();
    
    // Clean up the temporary file
    std::filesystem::remove(tempFilePath);
    return 0;
}

int main(int argc, char* argv[]) {
    CliParser parser(argc, argv);
    if (!parser.parse()) {
        // Error message already printed by parser
        std::cerr << parser.getHelpMessage() << std::endl; // Print help on error
        return 1;
    }

    if (parser.isHelp()) {
        std::cout << parser.getHelpMessage() << std::endl;
        return 0;
    }
    
    // Configure logger using the parsed options - Game continues even if logger configuration fails
    LoggerConfig::configure(parser);

    LOG_INFO("Tank Battle Game started");

    if (parser.isOnlyGenerate()) {
        if (!parser.getGameBoardFile().empty()) {
            std::cerr << "Warning: Game board file specified with --only_generate. Board file will be ignored." << std::endl;
        }
        return generateBoard(parser.getConfigPath(), true);
    } else if (parser.isRunGenerated()) {
        if (!parser.getGameBoardFile().empty()) {
             std::cerr << "Warning: Game board file specified with --run_generated. Board file will be ignored." << std::endl;
        }
        return generateAndRunGame(parser.getConfigPath());
    } else if (!parser.getGameBoardFile().empty()) {
        if (!parser.getConfigPath().empty()) {
             std::cerr << "Warning: --config-path is specified with a direct game board file. Config path will be ignored for direct board run." << std::endl;
        }
        return runGameWithFile(parser.getGameBoardFile());
    } else {
        // No specific mode and no game board file.
        std::cout << parser.getHelpMessage() << std::endl;
        return 1; // Indicate incorrect usage
    }
}