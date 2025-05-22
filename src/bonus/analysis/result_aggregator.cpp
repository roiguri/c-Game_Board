#include "bonus/analysis/result_aggregator.h"
#include <filesystem>
#include <iostream>
#include "bonus/analysis/analysis_tool.h"
#include "bonus/analysis/analysis_reporter.h"

void ResultAggregator::updateResults(const BoardConfig& config, const std::string& configKey, Winner outcome) {
    // Overall results
    aggregatedResults[configKey].totalGames++;
    switch (outcome) {
        case Winner::PLAYER1: aggregatedResults[configKey].player1Wins++; break;
        case Winner::PLAYER2: aggregatedResults[configKey].player2Wins++; break;
        case Winner::TIE:     aggregatedResults[configKey].ties++; break;
        case Winner::UNKNOWN: aggregatedResults[configKey].unknownOutcomes++; break;
    }
    // Width
    widthAnalysis[config.width].totalGames++;
    if (outcome == Winner::PLAYER1) widthAnalysis[config.width].player1Wins++;
    else if (outcome == Winner::PLAYER2) widthAnalysis[config.width].player2Wins++;
    else if (outcome == Winner::TIE) widthAnalysis[config.width].ties++;
    else widthAnalysis[config.width].unknownOutcomes++;
    // Height
    heightAnalysis[config.height].totalGames++;
    if (outcome == Winner::PLAYER1) heightAnalysis[config.height].player1Wins++;
    else if (outcome == Winner::PLAYER2) heightAnalysis[config.height].player2Wins++;
    else if (outcome == Winner::TIE) heightAnalysis[config.height].ties++;
    else heightAnalysis[config.height].unknownOutcomes++;
    // Wall Density
    wallDensityAnalysis[config.wallDensity].totalGames++;
    if (outcome == Winner::PLAYER1) wallDensityAnalysis[config.wallDensity].player1Wins++;
    else if (outcome == Winner::PLAYER2) wallDensityAnalysis[config.wallDensity].player2Wins++;
    else if (outcome == Winner::TIE) wallDensityAnalysis[config.wallDensity].ties++;
    else wallDensityAnalysis[config.wallDensity].unknownOutcomes++;
    // Mine Density
    mineDensityAnalysis[config.mineDensity].totalGames++;
    if (outcome == Winner::PLAYER1) mineDensityAnalysis[config.mineDensity].player1Wins++;
    else if (outcome == Winner::PLAYER2) mineDensityAnalysis[config.mineDensity].player2Wins++;
    else if (outcome == Winner::TIE) mineDensityAnalysis[config.mineDensity].ties++;
    else mineDensityAnalysis[config.mineDensity].unknownOutcomes++;
    // Symmetry
    symmetryAnalysis[config.symmetry].totalGames++;
    if (outcome == Winner::PLAYER1) symmetryAnalysis[config.symmetry].player1Wins++;
    else if (outcome == Winner::PLAYER2) symmetryAnalysis[config.symmetry].player2Wins++;
    else if (outcome == Winner::TIE) symmetryAnalysis[config.symmetry].ties++;
    else symmetryAnalysis[config.symmetry].unknownOutcomes++;
    // Max Steps
    maxStepsAnalysis[config.maxSteps].totalGames++;
    if (outcome == Winner::PLAYER1) maxStepsAnalysis[config.maxSteps].player1Wins++;
    else if (outcome == Winner::PLAYER2) maxStepsAnalysis[config.maxSteps].player2Wins++;
    else if (outcome == Winner::TIE) maxStepsAnalysis[config.maxSteps].ties++;
    else maxStepsAnalysis[config.maxSteps].unknownOutcomes++;
    // Num Shells
    numShellsAnalysis[config.numShells].totalGames++;
    if (outcome == Winner::PLAYER1) numShellsAnalysis[config.numShells].player1Wins++;
    else if (outcome == Winner::PLAYER2) numShellsAnalysis[config.numShells].player2Wins++;
    else if (outcome == Winner::TIE) numShellsAnalysis[config.numShells].ties++;
    else numShellsAnalysis[config.numShells].unknownOutcomes++;
    // Num Tanks Per Player
    numTanksPerPlayerAnalysis[config.numTanksPerPlayer].totalGames++;
    if (outcome == Winner::PLAYER1) numTanksPerPlayerAnalysis[config.numTanksPerPlayer].player1Wins++;
    else if (outcome == Winner::PLAYER2) numTanksPerPlayerAnalysis[config.numTanksPerPlayer].player2Wins++;
    else if (outcome == Winner::TIE) numTanksPerPlayerAnalysis[config.numTanksPerPlayer].ties++;
    else numTanksPerPlayerAnalysis[config.numTanksPerPlayer].unknownOutcomes++;
}

void ResultAggregator::writeCSVs() {
    std::filesystem::create_directories("output");
    AnalysisReporter::writeOverallResultsCsv("output/overall_results.csv", aggregatedResults);
    AnalysisReporter::writeDimensionAnalysisCsv("output/width_analysis.csv", "width", widthAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv("output/height_analysis.csv", "height", heightAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv("output/wall_density_analysis.csv", "wallDensity", wallDensityAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv("output/mine_density_analysis.csv", "mineDensity", mineDensityAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv("output/symmetry_analysis.csv", "symmetry", symmetryAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv("output/max_steps_analysis.csv", "maxSteps", maxStepsAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv("output/num_shells_analysis.csv", "numShells", numShellsAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv("output/num_tanks_per_player_analysis.csv", "numTanksPerPlayer", numTanksPerPlayerAnalysis);
}

void ResultAggregator::printSummaries() {
    AnalysisReporter::printDimensionAnalysis("width", widthAnalysis);
    AnalysisReporter::printDimensionAnalysis("height", heightAnalysis);
    AnalysisReporter::printDimensionAnalysis("wallDensity", wallDensityAnalysis);
    AnalysisReporter::printDimensionAnalysis("mineDensity", mineDensityAnalysis);
    AnalysisReporter::printDimensionAnalysis("symmetry", symmetryAnalysis);
    AnalysisReporter::printDimensionAnalysis("maxSteps", maxStepsAnalysis);
    AnalysisReporter::printDimensionAnalysis("numShells", numShellsAnalysis);
    AnalysisReporter::printDimensionAnalysis("numTanksPerPlayer", numTanksPerPlayerAnalysis);
} 