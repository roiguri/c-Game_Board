#include <filesystem>
#include <iostream>

#include "UserCommon/bonus/analysis/analysis_reporter.h"
#include "UserCommon/bonus/analysis/analysis_tool.h"
#include "UserCommon/bonus/analysis/result_aggregator.h"

void ResultAggregator::updateResults(const BoardConfig& config, const std::string& configKey, Winner outcome) {
    // Overall results
    aggregatedResults[configKey].totalGames++;
    switch (outcome) {
        case Winner::PLAYER1: aggregatedResults[configKey].player1Wins++; break;
        case Winner::PLAYER2: aggregatedResults[configKey].player2Wins++; break;
        case Winner::TIE:     aggregatedResults[configKey].ties++; break;
        case Winner::UNKNOWN: aggregatedResults[configKey].unknownOutcomes++; break;
    }
    // Board Size
    boardSizeAnalysis[config.width].totalGames++;
    if (outcome == Winner::PLAYER1) boardSizeAnalysis[config.width].player1Wins++;
    else if (outcome == Winner::PLAYER2) boardSizeAnalysis[config.width].player2Wins++;
    else if (outcome == Winner::TIE) boardSizeAnalysis[config.width].ties++;
    else boardSizeAnalysis[config.width].unknownOutcomes++;
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
    AnalysisReporter::writeDimensionAnalysisCsv("output/board_size_analysis.csv", "boardSize", boardSizeAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv("output/wall_density_analysis.csv", "wallDensity", wallDensityAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv("output/mine_density_analysis.csv", "mineDensity", mineDensityAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv("output/num_shells_analysis.csv", "numShells", numShellsAnalysis);
    AnalysisReporter::writeDimensionAnalysisCsv("output/num_tanks_per_player_analysis.csv", "numTanksPerPlayer", numTanksPerPlayerAnalysis);

    generateSummaryReport();
}

void ResultAggregator::generateSummaryReport() {
    std::cout << "Generating summary report..." << std::endl;
    
    std::string report = m_summarizer.generateSummaryReport(
        aggregatedResults,
        boardSizeAnalysis,
        wallDensityAnalysis,
        mineDensityAnalysis,
        numShellsAnalysis,
        numTanksPerPlayerAnalysis
    );
    
    // Ensure output directory exists
    std::filesystem::create_directories("output");
    
    // Save the report
    m_summarizer.saveReportToFile(report, "output/analysis_summary.txt");
}