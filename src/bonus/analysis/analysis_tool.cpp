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

void writeOverallResultsCsv(const std::string& filename, const std::map<std::string, GameOutcomeCounts>& results) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }
    // Write header
    file << "configKey,totalGames,player1Wins,player2Wins,ties,unknownOutcomes\n";
    for (const auto& pair : results) {
        const std::string& configKey = pair.first;
        const GameOutcomeCounts& counts = pair.second;
        file << '"' << configKey << '"' << ','
             << counts.totalGames << ','
             << counts.player1Wins << ','
             << counts.player2Wins << ','
             << counts.ties << ','
             << counts.unknownOutcomes << '\n';
    }
    file.close();
}

// Template implementation for per-dimension analysis CSV
// This must be in the header or in the same translation unit as its use
// But for now, we provide a definition here for int, float, and std::string

template<typename KeyType>
void writeDimensionAnalysisCsv(const std::string& filename, const std::string& dimensionName, const std::map<KeyType, GameOutcomeCounts>& analysisMap) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }
    // Write header
    file << dimensionName << ",totalGames,player1Wins,player2Wins,ties,unknownOutcomes\n";
    for (const auto& pair : analysisMap) {
        file << pair.first << ','
             << pair.second.totalGames << ','
             << pair.second.player1Wins << ','
             << pair.second.player2Wins << ','
             << pair.second.ties << ','
             << pair.second.unknownOutcomes << '\n';
    }
    file.close();
}
// Explicit instantiations for common types
// (If you get linker errors, move this template to the header)
template void writeDimensionAnalysisCsv<int>(const std::string&, const std::string&, const std::map<int, GameOutcomeCounts>&);
template void writeDimensionAnalysisCsv<float>(const std::string&, const std::string&, const std::map<float, GameOutcomeCounts>&);
template void writeDimensionAnalysisCsv<std::string>(const std::string&, const std::string&, const std::map<std::string, GameOutcomeCounts>&);

#ifndef TEST_BUILD
int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    // Deactivate logging for performance and cleaner output
    Logger::getInstance().initialize(Logger::Level::INFO, false, false);

    std::cout << "Analysis tool started." << std::endl;

    AnalysisParams params;
    params.widths = {10, 20};
    params.heights = {10, 15};
    params.wallDensities = {0.1f, 0.25f};
    params.mineDensities = {0.05f};
    params.symmetryTypes = {"none", "horizontal"}; // Ensure these are compatible with BoardConfig
    params.seeds = {-1, 12345}; // Added an explicit seed for testing aggregation
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
                                        currentConfig.seed = (seedValue == -1 && currentConfig.seed != 0) ? static_cast<int>(std::time(nullptr)) : seedValue; // ensure time based seed is different if loop is fast
                                        if (seedValue == -1) { // Add small delay to ensure different seeds if loop is too fast
                                            // This is a simple way to potentially get different seeds for time(-1) option on fast loops.
                                            // A more robust method might involve a higher-resolution clock or a dedicated random number generator for seeds.
                                            #ifdef _WIN32
                                                // Sleep(10); // Windows, milliseconds
                                            #else
                                                // usleep(10000); // POSIX, microseconds (10ms)
                                            #endif
                                        }
                                        currentConfig.maxSteps = maxStep;
                                        currentConfig.numShells = numShell;
                                        currentConfig.numTanksPerPlayer = numTanks;
                                        
                                        std::string configKey = GenerateKey(currentConfig);

                                        // Game Simulation
                                        BoardManager boardManager;
                                        if (!boardManager.generateBoard(currentConfig)) {
                                            std::cerr << "Error: Board generation failed for " << configKey << ". Skipping." << std::endl;
                                            continue; 
                                        }

                                        const std::string tempBoardFilePath = "temp_analysis_board_" + configKey + ".txt"; // Unique temp file
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

                                        // Result Extraction
                                        // Output file path is now also made unique to avoid race conditions if parallelized later.
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
                                            
                                            // Parse the result and update aggregatedResults
                                            outcome = ParseGameResult(lastLine);
                                            aggregator.updateResults(currentConfig, configKey, outcome);
                                        } else {
                                            std::cerr << "Error: Could not open result file: '" << outputFilePath << "' for " << configKey << "." << std::endl;
                                            aggregator.updateResults(currentConfig, configKey, Winner::UNKNOWN);
                                        }

                                        // Cleanup
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

    // --- Write CSV Reports ---
    aggregator.writeCSVs();
    // Optionally: aggregator.printSummaries();
    return 0;
}
#endif // TEST_BUILD
