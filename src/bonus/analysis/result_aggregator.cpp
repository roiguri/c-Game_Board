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

    // Update grand total counts
    grandTotalCounts.totalGames++;
    switch (outcome) {
        case Winner::PLAYER1: grandTotalCounts.player1Wins++; break;
        case Winner::PLAYER2: grandTotalCounts.player2Wins++; break;
        case Winner::TIE:     grandTotalCounts.ties++; break;
        case Winner::UNKNOWN: grandTotalCounts.unknownOutcomes++; break;
    }
}

void ResultAggregator::calculateAllOdds() {
    // Calculate for aggregatedResults
    for (auto& pair : aggregatedResults) {
        GameOutcomeCounts& counts = pair.second;
        counts.p1WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player1Wins) / counts.totalGames : 0.0;
        counts.p2WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player2Wins) / counts.totalGames : 0.0;
    }
    // Calculate for widthAnalysis
    for (auto& pair : widthAnalysis) {
        GameOutcomeCounts& counts = pair.second;
        counts.p1WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player1Wins) / counts.totalGames : 0.0;
        counts.p2WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player2Wins) / counts.totalGames : 0.0;
    }
    // Calculate for heightAnalysis
    for (auto& pair : heightAnalysis) {
        GameOutcomeCounts& counts = pair.second;
        counts.p1WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player1Wins) / counts.totalGames : 0.0;
        counts.p2WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player2Wins) / counts.totalGames : 0.0;
    }
    // Calculate for wallDensityAnalysis
    for (auto& pair : wallDensityAnalysis) {
        GameOutcomeCounts& counts = pair.second;
        counts.p1WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player1Wins) / counts.totalGames : 0.0;
        counts.p2WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player2Wins) / counts.totalGames : 0.0;
    }
    // Calculate for mineDensityAnalysis
    for (auto& pair : mineDensityAnalysis) {
        GameOutcomeCounts& counts = pair.second;
        counts.p1WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player1Wins) / counts.totalGames : 0.0;
        counts.p2WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player2Wins) / counts.totalGames : 0.0;
    }
    // Calculate for symmetryAnalysis
    for (auto& pair : symmetryAnalysis) {
        GameOutcomeCounts& counts = pair.second;
        counts.p1WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player1Wins) / counts.totalGames : 0.0;
        counts.p2WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player2Wins) / counts.totalGames : 0.0;
    }
    // Calculate for maxStepsAnalysis
    for (auto& pair : maxStepsAnalysis) {
        GameOutcomeCounts& counts = pair.second;
        counts.p1WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player1Wins) / counts.totalGames : 0.0;
        counts.p2WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player2Wins) / counts.totalGames : 0.0;
    }
    // Calculate for numShellsAnalysis
    for (auto& pair : numShellsAnalysis) {
        GameOutcomeCounts& counts = pair.second;
        counts.p1WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player1Wins) / counts.totalGames : 0.0;
        counts.p2WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player2Wins) / counts.totalGames : 0.0;
    }
    // Calculate for numTanksPerPlayerAnalysis
    for (auto& pair : numTanksPerPlayerAnalysis) {
        GameOutcomeCounts& counts = pair.second;
        counts.p1WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player1Wins) / counts.totalGames : 0.0;
        counts.p2WinOdds = (counts.totalGames > 0) ? static_cast<double>(counts.player2Wins) / counts.totalGames : 0.0;
    }

    // Calculate for grandTotalCounts
    grandTotalCounts.p1WinOdds = (grandTotalCounts.totalGames > 0) ? static_cast<double>(grandTotalCounts.player1Wins) / grandTotalCounts.totalGames : 0.0;
    grandTotalCounts.p2WinOdds = (grandTotalCounts.totalGames > 0) ? static_cast<double>(grandTotalCounts.player2Wins) / grandTotalCounts.totalGames : 0.0;
}

void ResultAggregator::writeCSVs() {
    calculateAllOdds(); // Calculate odds before writing
    std::filesystem::create_directories("output");
    // Write overall results to its own file as before
    AnalysisReporter::writeOverallResultsCsv("output/overall_results.csv", aggregatedResults);

    // Create and open the single file for all dimension analyses
    std::ofstream dimensionSummaryFile("output/dimension_summary.csv");
    if (!dimensionSummaryFile.is_open()) {
        std::cerr << "Error: Could not open file for writing: output/dimension_summary.csv" << std::endl;
        return;
    }

    // Write each dimension's analysis to the single file
    AnalysisReporter::writeDimensionAnalysisCsv(dimensionSummaryFile, "width", widthAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv(dimensionSummaryFile, "height", heightAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv(dimensionSummaryFile, "wallDensity", wallDensityAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv(dimensionSummaryFile, "mineDensity", mineDensityAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv(dimensionSummaryFile, "symmetry", symmetryAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv(dimensionSummaryFile, "maxSteps", maxStepsAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv(dimensionSummaryFile, "numShells", numShellsAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv(dimensionSummaryFile, "numTanksPerPlayer", numTanksPerPlayerAnalysis);

    dimensionSummaryFile.close();

    // Write the grand total summary
    AnalysisReporter::writeGrandTotalCsv("output/grand_total_summary.csv", grandTotalCounts);

    // Open a new file for odds statistics
    std::ofstream oddsStatsFile("output/odds_statistics_summary.csv");
    if (!oddsStatsFile.is_open()) {
        std::cerr << "Error: Could not open file for writing: output/odds_statistics_summary.csv" << std::endl;
        return; // Or skip, but returning since other critical files might also fail
    }

    // Write odds statistics for each dimension
    AnalysisReporter::writeOddsStatisticsCsv(oddsStatsFile, "Width", widthAnalysis);
    AnalysisReporter::writeOddsStatisticsCsv(oddsStatsFile, "Height", heightAnalysis);
    AnalysisReporter::writeOddsStatisticsCsv(oddsStatsFile, "WallDensity", wallDensityAnalysis);
    AnalysisReporter::writeOddsStatisticsCsv(oddsStatsFile, "MineDensity", mineDensityAnalysis);
    AnalysisReporter::writeOddsStatisticsCsv(oddsStatsFile, "Symmetry", symmetryAnalysis);
    AnalysisReporter::writeOddsStatisticsCsv(oddsStatsFile, "MaxSteps", maxStepsAnalysis);
    AnalysisReporter::writeOddsStatisticsCsv(oddsStatsFile, "NumShells", numShellsAnalysis);
    AnalysisReporter::writeOddsStatisticsCsv(oddsStatsFile, "NumTanksPerPlayer", numTanksPerPlayerAnalysis);
    
    // Write grand total odds
    AnalysisReporter::writeGrandTotalOdds(oddsStatsFile, grandTotalCounts);

    oddsStatsFile.close();
}

void ResultAggregator::printSummaries() {
    calculateAllOdds(); // Calculate odds before printing
    AnalysisReporter::printDimensionAnalysis("width", widthAnalysis);
    AnalysisReporter::printDimensionAnalysis("height", heightAnalysis);
    AnalysisReporter::printDimensionAnalysis("wallDensity", wallDensityAnalysis);
    AnalysisReporter::printDimensionAnalysis("mineDensity", mineDensityAnalysis);
    AnalysisReporter::printDimensionAnalysis("symmetry", symmetryAnalysis);
    AnalysisReporter::printDimensionAnalysis("maxSteps", maxStepsAnalysis);
    AnalysisReporter::printDimensionAnalysis("numShells", numShellsAnalysis);
    AnalysisReporter::printDimensionAnalysis("numTanksPerPlayer", numTanksPerPlayerAnalysis);
} 