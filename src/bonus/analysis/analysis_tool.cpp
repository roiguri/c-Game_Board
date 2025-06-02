#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <filesystem> // For std::filesystem operations
#include <ctime>      // For std::time for seeding
#include <sstream>    // For std::ostringstream (key generation)
#include <iomanip>    // For std::fixed and std::setprecision (key generation)
#include <algorithm>

#include "bonus/board_generator.h"
#include "game_manager.h"
#include "algo/basic_tank_algorithm_factory.h"
#include "players/basic_player_factory.h"
#include "bonus/logger/logger_config.h"
#include "bonus/analysis/analysis_tool.h"
#include "bonus/analysis/analysis_utils.h"
#include "bonus/analysis/board_manager.h"
#include "bonus/analysis/result_aggregator.h"
#include "bonus/analysis/analysis_params.h"
#include "bonus/analysis/analysis_config.h"
#include <random>

#ifndef TEST_BUILD
int AnalysisTool::runAnalysis() {
    // Deactivate logging for performance and cleaner output
    Logger::getInstance().initialize(Logger::Level::INFO, false, false);

    std::cout << "Analysis tool started." << std::endl;

    AnalysisConfig config;
    AnalysisParams params = config.getParams();

    ResultAggregator aggregator;
    auto configs = generateAllConfigs(params);

    std::cout << "Generated " << configs.size() << " configurations to test." << std::endl;
    for (const auto& config : configs) {
        runSingleSimulation(config, aggregator);
    }
    std::cout << "\nAnalysis tool finished." << std::endl;
    aggregator.writeCSVs();
    // Optionally: aggregator.printSummaries();
    return 0;
}

int AnalysisTool::runAnalysisWithConfig(const std::string& configFile) {
    // Deactivate logging for performance and cleaner output
    Logger::getInstance().initialize(Logger::Level::INFO, false, false);

    std::cout << "Analysis tool started." << std::endl;

    // Load configuration from file
    AnalysisConfig config;
    if (config.loadFromFile(configFile)) {
        std::cout << "Using configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Failed to load config file, using defaults." << std::endl;
    }
    
    AnalysisParams params = config.getParams();

    ResultAggregator aggregator;
    auto configs = generateAllConfigs(params);
    
    std::cout << "Generated " << configs.size() << " configurations to test." << std::endl;
    
    for (const auto& boardConfig : configs) {
        runSingleSimulation(boardConfig, aggregator);
    }
    std::cout << "\nAnalysis tool finished." << std::endl;
    aggregator.writeCSVs();
    return 0;
}

std::vector<BoardConfig> AnalysisTool::generateAllConfigs(const AnalysisParams& params) {
    std::vector<BoardConfig> configs;
    for (int boardSize : params.boardSizes) {
        for (float wallDensity : params.wallDensities) {
            for (float mineDensity : params.mineDensities) {
                for (int numShell : params.numShells) {
                    for (int numTanks : params.numTanksPerPlayer) {
                        for (int sample = 0; sample < params.numSamples; sample++) {
                            BoardConfig config;
                            config.width = boardSize;
                            config.height = boardSize;
                            config.wallDensity = wallDensity;
                            config.mineDensity = mineDensity;
                            config.symmetry = params.symmetryType;
                            config.seed = -1;
                            config.maxSteps = params.maxSteps;
                            config.numShells = numShell;
                            config.numTanksPerPlayer = numTanks;
                            configs.push_back(config);
                        }
                    }
                }
            }
        }
    }
    return configs;
}

void AnalysisTool::runSingleSimulation(const BoardConfig& config, ResultAggregator& aggregator) {
    std::string configKey = GenerateKey(config);
    BoardManager boardManager;
    BoardConfig actualConfig = config;
    if (config.seed == -1) {
        // Use a combination of current time and a random component to ensure uniqueness
        static std::random_device rd;
        static std::mt19937 gen(rd());
        actualConfig.seed = static_cast<int>(gen());
    }

    if (!boardManager.generateBoard(actualConfig)) {
        std::cerr << "Error: Board generation failed for " << configKey << ". Skipping." << std::endl;
        return;
    }
    const std::string tempBoardFilePath = "temp_analysis_board_" + configKey + ".txt";
    if (!boardManager.saveToFile(actualConfig, tempBoardFilePath)) {
        std::cerr << "Error: Saving board to file '" << tempBoardFilePath << "' failed for " << configKey << ". Skipping." << std::endl;
        return;
    }
    BasicPlayerFactory playerFactory;
    BasicTankAlgorithmFactory algoFactory;
    GameManager gameManager(playerFactory, algoFactory);
    std::streambuf* originalCoutRdbuf = std::cout.rdbuf();
    std::ofstream devNullStream("/dev/null");
    std::cout.rdbuf(devNullStream.rdbuf());
    bool boardReadOk = gameManager.readBoard(tempBoardFilePath);
    std::cout.rdbuf(originalCoutRdbuf);
    if (!boardReadOk) {
        std::cerr << "Error: GameManager failed to read board from '" << tempBoardFilePath << "' for " << configKey << ". Skipping." << std::endl;
        boardManager.cleanupTempFiles(configKey);
        return;
    }
    originalCoutRdbuf = std::cout.rdbuf();
    std::cout.rdbuf(devNullStream.rdbuf());
    gameManager.run();
    std::cout.rdbuf(originalCoutRdbuf);
    devNullStream.close();
    const std::string outputFilePath = "output_temp_analysis_board_" + configKey + ".txt";
    std::ifstream resultFile(outputFilePath);
    std::string lastLine;
    std::string currentLineStr;
    Winner outcome;
    if (resultFile.is_open()) {
        while (std::getline(resultFile, currentLineStr)) {
            if (!currentLineStr.empty()) {
                lastLine = currentLineStr;
            }
        }
        resultFile.close();
        outcome = ParseGameResult(lastLine);
        aggregator.updateResults(actualConfig, configKey, outcome);
    } else {
        std::cerr << "Error: Could not open result file: '" << outputFilePath << "' for " << configKey << "." << std::endl;
        aggregator.updateResults(actualConfig, configKey, Winner::UNKNOWN);
    }
    boardManager.cleanupTempFiles(configKey);
}

int main(int argc, char* argv[]) {
    AnalysisTool tool;
    
    // Check for config file argument
    if (argc > 1) {
        std::string configFile = argv[1];
        return tool.runAnalysisWithConfig(configFile);
    } else {
        return tool.runAnalysis();  // Use defaults
    }
}
#endif // TEST_BUILD
