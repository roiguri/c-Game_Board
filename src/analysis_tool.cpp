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

// Placeholder BoardConfig struct is removed; bonus/board_generator.h should provide it.

struct AnalysisParams {
    std::vector<int> widths;
    std::vector<int> heights;
    std::vector<float> wallDensities;
    std::vector<float> mineDensities;
    std::vector<std::string> symmetryTypes;
    std::vector<int> seeds;
    std::vector<int> maxSteps;
    std::vector<int> numShells;
    std::vector<int> numTanksPerPlayer;
};

// --- Structs and Enums for Result Aggregation ---
enum class Winner { PLAYER1, PLAYER2, TIE, UNKNOWN };

struct GameOutcomeCounts {
    int player1Wins = 0;
    int player2Wins = 0;
    int ties = 0;
    int unknownOutcomes = 0; // To count games where the result string was not recognized
    int totalGames = 0;      // New member for total games for this configuration
};

// --- Helper Functions for Result Processing ---
Winner ParseGameResult(const std::string& resultLine) {
    if (resultLine.find("Player 1 won") != std::string::npos) {
        return Winner::PLAYER1;
    } else if (resultLine.find("Player 2 won") != std::string::npos) {
        return Winner::PLAYER2;
    } else if (resultLine.find("Tie,") != std::string::npos) { // Assuming "Tie," is part of the tie message
        return Winner::TIE;
    }
    // Log unknown results for debugging, but don't let it stop the analysis.
    std::cerr << "Warning: Unknown game result string encountered: \"" << resultLine << "\"" << std::endl;
    return Winner::UNKNOWN;
}

std::string GenerateKey(const BoardConfig& config) {
    std::ostringstream oss;
    // Use fixed precision for floating point numbers to ensure consistent keys.
    // Using 3 decimal places for densities as an example.
    oss << std::fixed << std::setprecision(3); 

    oss << "w" << config.width
        << "_h" << config.height
        << "_wd" << config.wallDensity  // wallDensity will be formatted to 3 decimal places
        << "_md" << config.mineDensity; // mineDensity will be formatted to 3 decimal places
    
    // Reset precision to default for non-float types if needed, though here it's not strictly necessary
    // as subsequent items are integers or strings that don't use stream precision.
    oss << std::defaultfloat << std::setprecision(6); // Reset to default

    // Assuming symmetryType is a string. If it's an enum, it should be converted to string/int first.
    oss << "_sym" << config.symmetryType 
        // Note: if seed is -1 (time-based), this part of the key will vary for each run with that setting.
        // This is generally desired if you want to treat time-based seeds as distinct runs.
        // If seeds are explicit, they will group correctly.
        << "_seed" << config.seed 
        << "_steps" << config.maxSteps
        << "_shells" << config.numShells
        << "_tanks" << config.numTanksPerPlayer;
    return oss.str();
}

// --- Templated Helper Function for Printing Per-Dimension Analysis ---
template<typename KeyType>
void printDimensionAnalysis(const std::string& dimensionName, const std::map<KeyType, GameOutcomeCounts>& analysisMap) {
    std::cout << "\n--- Analysis by " << dimensionName << " ---" << std::endl;
    if (analysisMap.empty()) {
        std::cout << "No data available for this dimension." << std::endl;
        return;
    }

    // Determine if KeyType is float to apply std::fixed and std::setprecision for the key itself
    bool isKeyFloat = std::is_same<KeyType, float>::value;

    for (const auto& pair : analysisMap) {
        const KeyType& paramValue = pair.first;
        const GameOutcomeCounts& counts = pair.second;

        std::cout << dimensionName << ": ";
        if (isKeyFloat) {
            std::cout << std::fixed << std::setprecision(3) << paramValue << std::defaultfloat << std::setprecision(6);
        } else {
            std::cout << paramValue;
        }
        std::cout << std::endl;

        if (counts.totalGames == 0) {
            std::cout << "  Total Games: 0" << std::endl;
            std::cout << "  P1 Win %: N/A" << std::endl;
            std::cout << "  P2 Win %: N/A" << std::endl;
            std::cout << "  Tie %: N/A" << std::endl;
            if (counts.unknownOutcomes > 0) {
                 std::cout << "  Unknown Outcomes: " << counts.unknownOutcomes << std::endl;
            }
        } else {
            double p1WinRate = (static_cast<double>(counts.player1Wins) / counts.totalGames) * 100.0;
            double p2WinRate = (static_cast<double>(counts.player2Wins) / counts.totalGames) * 100.0;
            double tieRate = (static_cast<double>(counts.ties) / counts.totalGames) * 100.0;
            
            std::cout << "  Total Games: " << counts.totalGames << std::endl;
            std::cout << std::fixed << std::setprecision(1); // For printing percentages
            std::cout << "  P1 Win %: " << p1WinRate << "%" << std::endl;
            std::cout << "  P2 Win %: " << p2WinRate << "%" << std::endl;
            std::cout << "  Tie %: " << tieRate << "%" << std::endl;
            if (counts.unknownOutcomes > 0) {
                 double unknownRate = (static_cast<double>(counts.unknownOutcomes) / counts.totalGames) * 100.0;
                 std::cout << "  Unknown Outcomes: " << counts.unknownOutcomes << " (" << unknownRate << "%)" << std::endl;
            }
            std::cout << std::defaultfloat << std::setprecision(6); // Reset to default for subsequent prints
        }
        std::cout << "  ---------------------------" << std::endl;
    }
}


#ifndef TEST_BUILD
int main(int argc, char* argv[]) {
    // Deactivate logging for performance and cleaner output
    LoggerConfig::getInstance().setLogLevel(LogLevel::NONE);
    LoggerConfig::getInstance().setLogToConsole(false);
    LoggerConfig::getInstance().setLogToFile(false);

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

    std::map<std::string, GameOutcomeCounts> aggregatedResults; // Overall results per unique config string

    // --- New Maps for Per-Dimension Analysis ---
    std::map<int, GameOutcomeCounts> widthAnalysis;
    std::map<int, GameOutcomeCounts> heightAnalysis;
    std::map<float, GameOutcomeCounts> wallDensityAnalysis;
    std::map<float, GameOutcomeCounts> mineDensityAnalysis;
    std::map<std::string, GameOutcomeCounts> symmetryAnalysis;
    // std::map<int, GameOutcomeCounts> seedAnalysis; // Decided against per-seed analysis for now
    std::map<int, GameOutcomeCounts> maxStepsAnalysis;
    std::map<int, GameOutcomeCounts> numShellsAnalysis;
    std::map<int, GameOutcomeCounts> numTanksPerPlayerAnalysis;

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
                                        currentConfig.symmetryType = symmetryTypeLoop; 
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

                                        std::cout << "\nProcessing Config Key: " << configKey << std::endl;
                                        // std::cout << "BoardConfig: " // Old detailed print, can be enabled for debugging
                                        //           << "width=" << currentConfig.width << ", "
                                        //           << "height=" << currentConfig.height << ", "
                                        //           // ... (rest of params)
                                        //           << std::endl;


                                        // Game Simulation
                                        BoardGenerator generator(currentConfig);
                                        if (!generator.generateBoard()) {
                                            std::cerr << "Error: Board generation failed for " << configKey << ". Skipping." << std::endl;
                                            continue; 
                                        }

                                        const std::string tempBoardFilePath = "temp_analysis_board_" + configKey + ".txt"; // Unique temp file
                                        if (!generator.saveToFile(tempBoardFilePath)) {
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
                                            try { if(std::filesystem::exists(tempBoardFilePath)) std::filesystem::remove(tempBoardFilePath); } catch (const std::exception& e) { /* ignore */ }
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
                                            Winner outcome = ParseGameResult(lastLine);
                                            aggregatedResults[configKey].totalGames++; // Update total games for the main aggregated map
                                            
                                            switch (outcome) {
                                                case Winner::PLAYER1:
                                                    aggregatedResults[configKey].player1Wins++;
                                                    break;
                                                case Winner::PLAYER2:
                                                    aggregatedResults[configKey].player2Wins++;
                                                    break;
                                                case Winner::TIE:
                                                    aggregatedResults[configKey].ties++;
                                                    break;
                                                case Winner::UNKNOWN:
                                                    aggregatedResults[configKey].unknownOutcomes++;
                                                    break;
                                            }
                                            // Removed: std::cout << "Game Result: " << lastLine << std::endl; 
                                        } else {
                                            std::cerr << "Error: Could not open result file: '" << outputFilePath << "' for " << configKey << "." << std::endl;
                                            aggregatedResults[configKey].totalGames++; // Still count this attempt in totalGames for the main map
                                            aggregatedResults[configKey].unknownOutcomes++; 
                                        }

                                        // --- Update Per-Dimension Analysis Maps ---
                                        // Note: 'outcome' is the Winner enum value from ParseGameResult
                                        // 'currentConfig' is the BoardConfig for the current iteration

                                        // Width Analysis
                                        widthAnalysis[currentConfig.width].totalGames++;
                                        if (outcome == Winner::PLAYER1) widthAnalysis[currentConfig.width].player1Wins++;
                                        else if (outcome == Winner::PLAYER2) widthAnalysis[currentConfig.width].player2Wins++;
                                        else if (outcome == Winner::TIE) widthAnalysis[currentConfig.width].ties++;
                                        else widthAnalysis[currentConfig.width].unknownOutcomes++;

                                        // Height Analysis
                                        heightAnalysis[currentConfig.height].totalGames++;
                                        if (outcome == Winner::PLAYER1) heightAnalysis[currentConfig.height].player1Wins++;
                                        else if (outcome == Winner::PLAYER2) heightAnalysis[currentConfig.height].player2Wins++;
                                        else if (outcome == Winner::TIE) heightAnalysis[currentConfig.height].ties++;
                                        else heightAnalysis[currentConfig.height].unknownOutcomes++;

                                        // Wall Density Analysis
                                        wallDensityAnalysis[currentConfig.wallDensity].totalGames++;
                                        if (outcome == Winner::PLAYER1) wallDensityAnalysis[currentConfig.wallDensity].player1Wins++;
                                        else if (outcome == Winner::PLAYER2) wallDensityAnalysis[currentConfig.wallDensity].player2Wins++;
                                        else if (outcome == Winner::TIE) wallDensityAnalysis[currentConfig.wallDensity].ties++;
                                        else wallDensityAnalysis[currentConfig.wallDensity].unknownOutcomes++;
                                        
                                        // Mine Density Analysis
                                        mineDensityAnalysis[currentConfig.mineDensity].totalGames++;
                                        if (outcome == Winner::PLAYER1) mineDensityAnalysis[currentConfig.mineDensity].player1Wins++;
                                        else if (outcome == Winner::PLAYER2) mineDensityAnalysis[currentConfig.mineDensity].player2Wins++;
                                        else if (outcome == Winner::TIE) mineDensityAnalysis[currentConfig.mineDensity].ties++;
                                        else mineDensityAnalysis[currentConfig.mineDensity].unknownOutcomes++;

                                        // Symmetry Analysis
                                        symmetryAnalysis[currentConfig.symmetryType].totalGames++; // Assuming symmetryType is the key
                                        if (outcome == Winner::PLAYER1) symmetryAnalysis[currentConfig.symmetryType].player1Wins++;
                                        else if (outcome == Winner::PLAYER2) symmetryAnalysis[currentConfig.symmetryType].player2Wins++;
                                        else if (outcome == Winner::TIE) symmetryAnalysis[currentConfig.symmetryType].ties++;
                                        else symmetryAnalysis[currentConfig.symmetryType].unknownOutcomes++;
                                        
                                        // Max Steps Analysis
                                        maxStepsAnalysis[currentConfig.maxSteps].totalGames++;
                                        if (outcome == Winner::PLAYER1) maxStepsAnalysis[currentConfig.maxSteps].player1Wins++;
                                        else if (outcome == Winner::PLAYER2) maxStepsAnalysis[currentConfig.maxSteps].player2Wins++;
                                        else if (outcome == Winner::TIE) maxStepsAnalysis[currentConfig.maxSteps].ties++;
                                        else maxStepsAnalysis[currentConfig.maxSteps].unknownOutcomes++;

                                        // Num Shells Analysis
                                        numShellsAnalysis[currentConfig.numShells].totalGames++;
                                        if (outcome == Winner::PLAYER1) numShellsAnalysis[currentConfig.numShells].player1Wins++;
                                        else if (outcome == Winner::PLAYER2) numShellsAnalysis[currentConfig.numShells].player2Wins++;
                                        else if (outcome == Winner::TIE) numShellsAnalysis[currentConfig.numShells].ties++;
                                        else numShellsAnalysis[currentConfig.numShells].unknownOutcomes++;

                                        // Num Tanks Per Player Analysis
                                        numTanksPerPlayerAnalysis[currentConfig.numTanksPerPlayer].totalGames++;
                                        if (outcome == Winner::PLAYER1) numTanksPerPlayerAnalysis[currentConfig.numTanksPerPlayer].player1Wins++;
                                        else if (outcome == Winner::PLAYER2) numTanksPerPlayerAnalysis[currentConfig.numTanksPerPlayer].player2Wins++;
                                        else if (outcome == Winner::TIE) numTanksPerPlayerAnalysis[currentConfig.numTanksPerPlayer].ties++;
                                        else numTanksPerPlayerAnalysis[currentConfig.numTanksPerPlayer].unknownOutcomes++;

                                        // Cleanup
                                        try {
                                            if (std::filesystem::exists(tempBoardFilePath)) {
                                                if (!std::filesystem::remove(tempBoardFilePath)) {
                                                     std::cerr << "Warning: Failed to remove temporary board file: " << tempBoardFilePath << std::endl;
                                                }
                                            }
                                            if (std::filesystem::exists(outputFilePath)) {
                                                 if (!std::filesystem::remove(outputFilePath)) {
                                                     std::cerr << "Warning: Failed to remove temporary output file: " << outputFilePath << std::endl;
                                                 }
                                            }
                                        } catch (const std::filesystem::filesystem_error& e) {
                                            std::cerr << "Filesystem error during cleanup for " << configKey << ": " << e.what() << std::endl;
                                        }
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

    // --- Overall Aggregated Results Summary (kept from previous step) ---
    std::cout << "\n--- Overall Aggregated Results ---" << std::endl;
    if (aggregatedResults.empty()) {
        std::cout << "No results were aggregated during the analysis." << std::endl;
    } else {
        for (const auto& pair : aggregatedResults) {
            const std::string& configKey = pair.first;
            const GameOutcomeCounts& counts = pair.second;

            std::cout << "Parameters: " << configKey << std::endl;
            std::cout << "  Total Games: " << counts.totalGames << std::endl; // Added total games here too
            std::cout << std::fixed << std::setprecision(1);
            if (counts.totalGames > 0) {
                 std::cout << "  P1 Win %: " << (static_cast<double>(counts.player1Wins) / counts.totalGames) * 100.0 << "%" << std::endl;
                 std::cout << "  P2 Win %: " << (static_cast<double>(counts.player2Wins) / counts.totalGames) * 100.0 << "%" << std::endl;
                 std::cout << "  Tie %: " << (static_cast<double>(counts.ties) / counts.totalGames) * 100.0 << "%" << std::endl;
                 if (counts.unknownOutcomes > 0) {
                     std::cout << "  Unknown %: " << (static_cast<double>(counts.unknownOutcomes) / counts.totalGames) * 100.0 << "% (" << counts.unknownOutcomes << " games)" << std::endl;
                 }
            } else {
                 std::cout << "  P1 Win %: N/A" << std::endl;
                 std::cout << "  P2 Win %: N/A" << std::endl;
                 std::cout << "  Tie %: N/A" << std::endl;
                 if (counts.unknownOutcomes > 0) {
                     std::cout << "  Unknown Outcomes: " << counts.unknownOutcomes << std::endl;
                 }
            }
            std::cout << std::defaultfloat << std::setprecision(6); // Reset to default
            std::cout << "--------------------------------------------------" << std::endl;
        }
    }

    // --- Per-Dimension Analysis Summaries ---
    printDimensionAnalysis("Width", widthAnalysis);
    printDimensionAnalysis("Height", heightAnalysis);
    printDimensionAnalysis("Wall Density", wallDensityAnalysis);
    printDimensionAnalysis("Mine Density", mineDensityAnalysis);
    printDimensionAnalysis("Symmetry", symmetryAnalysis);
    printDimensionAnalysis("Max Steps", maxStepsAnalysis);
    printDimensionAnalysis("Number of Shells", numShellsAnalysis);
    printDimensionAnalysis("Tanks Per Player", numTanksPerPlayerAnalysis);
    
    return 0;
}
#endif // TEST_BUILD
