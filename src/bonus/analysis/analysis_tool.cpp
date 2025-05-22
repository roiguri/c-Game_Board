#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <filesystem> // For std::filesystem operations
#include <ctime>      // For std::time for seeding
#include <sstream>    // For std::ostringstream (key generation)
#include <iomanip>    // For std::fixed and std::setprecision (key generation)

#include "bonus/board_generator.h" // Use the actual BoardConfig from this header
#include "game_manager.h"
#include "algo/basic_tank_algorithm_factory.h"
#include "players/basic_player_factory.h"
#include "bonus/logger/logger_config.h"
#include "bonus/analysis/analysis_tool.h"
#include "bonus/analysis/analysis_utils.h"
#include "bonus/analysis/board_manager.h"
#include "bonus/analysis/result_aggregator.h"

// --- Helper Functions for Result Processing ---

// --- Templated Helper Function for Printing Per-Dimension Analysis ---
// (Moved printDimensionAnalysis to header)

#ifndef TEST_BUILD
int AnalysisTool::runAnalysis() {
    // Deactivate logging for performance and cleaner output
    Logger::getInstance().initialize(Logger::Level::INFO, false, false);

    std::cout << "Analysis tool started." << std::endl;

    AnalysisParams params;
    params.widths = {10, 20};
    params.heights = {10, 15};
    params.wallDensities = {0.1f, 0.25f};
    params.mineDensities = {0.05f};
    params.symmetryTypes = {"none", "horizontal"};
    params.seeds = {-1, 12345};
    params.maxSteps = {500, 1000};
    params.numShells = {10};
    params.numTanksPerPlayer = {1, 2};

    ResultAggregator aggregator;
    Winner outcome;

    for (int width : params.widths) {
        for (int height : params.heights) {
            for (float wallDensity : params.wallDensities) {
                for (float mineDensity : params.mineDensities) {
                    for (const std::string& symmetryTypeLoop : params.symmetryTypes) { 
                        for (int seedValue : params.seeds) { 
                            for (int maxStep : params.maxSteps) {
                                for (int numShell : params.numShells) {
                                    for (int numTanks : params.numTanksPerPlayer) {
                                        BoardConfig currentConfig; 
                                        currentConfig.width = width;
                                        currentConfig.height = height;
                                        currentConfig.wallDensity = wallDensity;
                                        currentConfig.mineDensity = mineDensity;
                                        currentConfig.symmetry = symmetryTypeLoop; 
                                        currentConfig.seed = (seedValue == -1 && currentConfig.seed != 0) ? static_cast<int>(std::time(nullptr)) : seedValue;
                                        if (seedValue == -1) {
                                            #ifdef _WIN32
                                                // Sleep(10);
                                            #else
                                                // usleep(10000);
                                            #endif
                                        }
                                        currentConfig.maxSteps = maxStep;
                                        currentConfig.numShells = numShell;
                                        currentConfig.numTanksPerPlayer = numTanks;
                                        std::string configKey = GenerateKey(currentConfig);

                                        BoardManager boardManager;
                                        if (!boardManager.generateBoard(currentConfig)) {
                                            std::cerr << "Error: Board generation failed for " << configKey << ". Skipping." << std::endl;
                                            continue; 
                                        }
                                        const std::string tempBoardFilePath = "temp_analysis_board_" + configKey + ".txt";
                                        if (!boardManager.saveToFile(currentConfig, tempBoardFilePath)) {
                                            std::cerr << "Error: Saving board to file '" << tempBoardFilePath << "' failed for " << configKey << ". Skipping." << std::endl;
                                            continue;
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
                                            continue;
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
                                        if (resultFile.is_open()) {
                                            while (std::getline(resultFile, currentLineStr)) {
                                                if (!currentLineStr.empty()) {
                                                    lastLine = currentLineStr;
                                                }
                                            }
                                            resultFile.close();
                                            outcome = ParseGameResult(lastLine);
                                            aggregator.updateResults(currentConfig, configKey, outcome);
                                        } else {
                                            std::cerr << "Error: Could not open result file: '" << outputFilePath << "' for " << configKey << "." << std::endl;
                                            aggregator.updateResults(currentConfig, configKey, Winner::UNKNOWN);
                                        }
                                        boardManager.cleanupTempFiles(configKey);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    std::cout << "\nAnalysis tool finished." << std::endl;
    aggregator.writeCSVs();
    // Optionally: aggregator.printSummaries();
    return 0;
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    AnalysisTool tool;
    return tool.runAnalysis();
}
#endif // TEST_BUILD
