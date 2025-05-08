#include "bonus/board_generator.h"
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include "game_manager.h"
#include "algo/tank_algorithm_factory_impl.h"
#include "player_factory_impl.h"

// Run the game with a specified board file
int runGameWithFile(const std::string& filePath) {
    // FIXME: Update with real factories
    GameManager gameManager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
    if (!gameManager.readBoard(filePath)) {
        return 1;
    }
    
    gameManager.run();
    
    return 0;
}

// Generate a board with the given configuration
int generateBoard(const std::string& configPath, bool saveToFile) {
    BoardGenerator generator;
    if (!configPath.empty()) {
        if (!generator.loadConfig(configPath)) {
            std::cerr << "Failed to load configuration file: " << configPath << std::endl;
            return 1;
        }
        std::cout << "Using configuration from: " << configPath << std::endl;
    } else {
        std::cout << "Using default board configuration" << std::endl;
    }
    
    if (!generator.generateBoard()) {
        std::cerr << "Failed to generate board" << std::endl;
        return 1;
    }
    
    if (saveToFile) {
        std::string outputPath = "generated_board.txt";
        if (generator.saveToFile(outputPath)) {
            std::cout << "Board generated and saved to " << outputPath << std::endl;
            return 0;
        } else {
            std::cerr << "Failed to save generated board" << std::endl;
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
            std::cerr << "Failed to load configuration file: " << configPath << std::endl;
            return 1;
        }
        std::cout << "Using configuration from: " << configPath << std::endl;
    } else {
        std::cout << "Using default board configuration" << std::endl;
    }
    
    if (!generator.generateBoard()) {
        std::cerr << "Failed to generate board" << std::endl;
        return 1;
    }
    
    // Create a temporary file for the generated board
    std::string tempFilePath = "temp_board.txt";
    if (!generator.saveToFile(tempFilePath)) {
        std::cerr << "Failed to create temporary board file" << std::endl;
        return 1;
    }
    
    // Run the game with the temporary file
    // FIXME: Update with real factories
    GameManager gameManager(std::make_unique<PlayerFactoryImpl>(), std::make_unique<TankAlgorithmFactoryImpl>());
    if (!gameManager.readBoard(tempFilePath)) {
        std::filesystem::remove(tempFilePath);
        return 1;
    }
    
    gameManager.run();
    
    // Clean up the temporary file
    std::filesystem::remove(tempFilePath);
    return 0;
}

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