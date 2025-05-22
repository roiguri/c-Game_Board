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
#include "analysis_tool.h"

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
    oss << "_sym" << config.symmetry
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
                                            outcome = ParseGameResult(lastLine);
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
                                        symmetryAnalysis[currentConfig.symmetry].totalGames++; // Assuming symmetryType is the key
                                        if (outcome == Winner::PLAYER1) symmetryAnalysis[currentConfig.symmetry].player1Wins++;
                                        else if (outcome == Winner::PLAYER2) symmetryAnalysis[currentConfig.symmetry].player2Wins++;
                                        else if (outcome == Winner::TIE) symmetryAnalysis[currentConfig.symmetry].ties++;
                                        else symmetryAnalysis[currentConfig.symmetry].unknownOutcomes++;
                                        
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
                                            // Remove visualization output file if it exists
                                            std::string visualizationFilePath = "output_temp_analysis_board_" + configKey + "_visualization.html";
                                            if (std::filesystem::exists(visualizationFilePath)) {
                                                if (!std::filesystem::remove(visualizationFilePath)) {
                                                    std::cerr << "Warning: Failed to remove temporary visualization file: " << visualizationFilePath << std::endl;
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
    // (Removed console output, only CSV output remains)

    // --- Per-Dimension Analysis Summaries ---
    // (Removed console output, only CSV output remains)

    // --- Write CSV Reports ---
    std::filesystem::create_directories("output"); // Ensure output directory exists
    writeOverallResultsCsv("output/overall_results.csv", aggregatedResults);
    writeDimensionAnalysisCsv("output/width_analysis.csv", "width", widthAnalysis);
    writeDimensionAnalysisCsv("output/height_analysis.csv", "height", heightAnalysis);
    writeDimensionAnalysisCsv("output/wall_density_analysis.csv", "wallDensity", wallDensityAnalysis);
    writeDimensionAnalysisCsv("output/mine_density_analysis.csv", "mineDensity", mineDensityAnalysis);
    writeDimensionAnalysisCsv("output/symmetry_analysis.csv", "symmetry", symmetryAnalysis);
    writeDimensionAnalysisCsv("output/max_steps_analysis.csv", "maxSteps", maxStepsAnalysis);
    writeDimensionAnalysisCsv("output/num_shells_analysis.csv", "numShells", numShellsAnalysis);
    writeDimensionAnalysisCsv("output/num_tanks_per_player_analysis.csv", "numTanksPerPlayer", numTanksPerPlayerAnalysis);
    
    return 0;
}
#endif // TEST_BUILD
