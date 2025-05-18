#include "bonus/board_generator.h"
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
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

// TODO: update with new command line arguments
// Print usage information
void printUsage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "  tanks_game <game_board_input_file>" << std::endl;
    std::cout << "  tanks_game --only_generate [--config-path=<path>]" << std::endl;
    std::cout << "  tanks_game --run_generated [--config-path=<path>]" << std::endl;
}

int main(int argc, char* argv[]) {
    bool onlyGenerate = false;
    bool runGenerated = false;
    std::string configPath = "";
    
    LoggerConfig::configureFromCommandLine(argc, argv);
    LOG_INFO("Tank Battle Game started");

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--only_generate") {
            onlyGenerate = true;
        } else if (arg == "--run_generated") {
            runGenerated = true;
        } else if (arg.find("--config-path=") == 0) {
            configPath = arg.substr(14); // Extract path after "="
        } else if (!onlyGenerate && !runGenerated) {
            // Regular game mode with specified board file
            return runGameWithFile(arg);
        }
    }
    
    // Handle generation modes
    if (onlyGenerate) {
        return generateBoard(configPath, true);
    } else if (runGenerated) {
        return generateAndRunGame(configPath);
    }
    
    // If we get here, invalid arguments were provided
    printUsage();
    return 1;
}