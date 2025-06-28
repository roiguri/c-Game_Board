#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "UserCommon/bonus/analysis/analysis_summarizer.h"

AnalysisSummarizer::AnalysisSummarizer() {}

double AnalysisSummarizer::calculateWinRate(int wins, int totalGames) {
    if (totalGames == 0) {
        return 0.0;
    }
    return (static_cast<double>(wins) / totalGames) * 100.0;
}

std::string AnalysisSummarizer::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string AnalysisSummarizer::generateHeader(int totalGames) {
    std::stringstream ss;
    ss << "TANK BATTLE ANALYSIS SUMMARY\n";
    ss << "============================\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n";
    ss << "Total Games Played: " << totalGames << "\n\n";
    return ss.str();
}

std::string AnalysisSummarizer::generateOverallResults(const std::map<std::string, GameOutcomeCounts>& overallResults) {
    // Calculate totals across all configurations
    int totalPlayer1Wins = 0;
    int totalPlayer2Wins = 0;
    int totalTies = 0;
    int totalGames = 0;
    
    for (const auto& pair : overallResults) {
        const GameOutcomeCounts& counts = pair.second;
        totalPlayer1Wins += counts.player1Wins;
        totalPlayer2Wins += counts.player2Wins;
        totalTies += counts.ties;
        totalGames += counts.totalGames;
    }
    
    StatResult p1Overall = calculateStats(totalPlayer1Wins, totalGames);
    StatResult p2Overall = calculateStats(totalPlayer2Wins, totalGames);
    
    std::stringstream ss;
    ss << "OVERALL RESULTS\n";
    ss << "===============\n";
    ss << std::fixed << std::setprecision(1);
    
    ss << "Player 1: " << p1Overall.winRate << "% [±" << p1Overall.marginOfError << "%]";
    if (p1Overall.isSignificant) ss << " *";
    ss << "\n";
    
    ss << "Player 2: " << p2Overall.winRate << "% [±" << p2Overall.marginOfError << "%]";
    if (p2Overall.isSignificant) ss << " *";
    ss << "\n";
    
    ss << "Ties: " << calculateWinRate(totalTies, totalGames) << "%\n";
    ss << "Total Games: " << totalGames << "\n";
    
    if (p1Overall.isSignificant || p2Overall.isSignificant) {
        ss << "\n* = Statistically significant difference from 50%\n";
    }
    ss << "\n";
    
    return ss.str();
}

std::string AnalysisSummarizer::generateDimensionalAnalysis(
    const std::map<int, GameOutcomeCounts>& boardSizeAnalysis,
    const std::map<float, GameOutcomeCounts>& wallDensityAnalysis,
    const std::map<float, GameOutcomeCounts>& mineDensityAnalysis,
    const std::map<int, GameOutcomeCounts>& numShellsAnalysis,
    const std::map<int, GameOutcomeCounts>& numTanksAnalysis
) {
    std::stringstream ss;
    ss << "DIMENSIONAL ANALYSIS\n";
    ss << "===================\n\n";
    ss << std::fixed << std::setprecision(1);
    
    // Board Size Analysis
    ss << "Board Size Effects:\n";
    ss << "-------------------\n";
    for (const auto& pair : boardSizeAnalysis) {
        int size = pair.first;
        const GameOutcomeCounts& counts = pair.second;
        
        StatResult p1Stats = calculateStats(counts.player1Wins, counts.totalGames);
        StatResult p2Stats = calculateStats(counts.player2Wins, counts.totalGames);
        
        ss << "Size " << std::setw(2) << size << "x" << size << ": ";
        ss << "P1: " << std::setw(4) << p1Stats.winRate << "% ";
        ss << "[±" << std::setw(4) << p1Stats.marginOfError << "%]";
        if (p1Stats.isSignificant) ss << " *";
        
        ss << " | P2: " << std::setw(4) << p2Stats.winRate << "% ";
        ss << "[±" << std::setw(4) << p2Stats.marginOfError << "%]";
        if (p2Stats.isSignificant) ss << " *";
        
        if (!p1Stats.hasAdequateSample) ss << " [LOW SAMPLE]";
        ss << " | Games: " << counts.totalGames << "\n";
    }
    ss << "\n";
    
    // Wall Density Analysis
    ss << "Wall Density Effects:\n";
    ss << "---------------------\n";
    for (const auto& pair : wallDensityAnalysis) {
        float density = pair.first;
        const GameOutcomeCounts& counts = pair.second;
        
        StatResult p1Stats = calculateStats(counts.player1Wins, counts.totalGames);
        StatResult p2Stats = calculateStats(counts.player2Wins, counts.totalGames);
        
        ss << "Density " << std::setw(4) << std::setprecision(2) << density << ": ";
        ss << "P1: " << std::setw(4) << std::setprecision(1) << p1Stats.winRate << "% ";
        ss << "[±" << std::setw(4) << p1Stats.marginOfError << "%]";
        if (p1Stats.isSignificant) ss << " *";
        
        ss << " | P2: " << std::setw(4) << p2Stats.winRate << "% ";
        ss << "[±" << std::setw(4) << p2Stats.marginOfError << "%]";
        if (p2Stats.isSignificant) ss << " *";
        
        if (!p1Stats.hasAdequateSample) ss << " [LOW SAMPLE]";
        ss << " | Games: " << counts.totalGames << "\n";
    }
    ss << "\n";
    
    // Mine Density Analysis
    ss << "Mine Density Effects:\n";
    ss << "---------------------\n";
    for (const auto& pair : mineDensityAnalysis) {
        float density = pair.first;
        const GameOutcomeCounts& counts = pair.second;
        
        StatResult p1Stats = calculateStats(counts.player1Wins, counts.totalGames);
        StatResult p2Stats = calculateStats(counts.player2Wins, counts.totalGames);
        
        ss << "Density " << std::setw(4) << std::setprecision(2) << density << ": ";
        ss << "P1: " << std::setw(4) << std::setprecision(1) << p1Stats.winRate << "% ";
        ss << "[±" << std::setw(4) << p1Stats.marginOfError << "%]";
        if (p1Stats.isSignificant) ss << " *";
        
        ss << " | P2: " << std::setw(4) << p2Stats.winRate << "% ";
        ss << "[±" << std::setw(4) << p2Stats.marginOfError << "%]";
        if (p2Stats.isSignificant) ss << " *";
        
        if (!p1Stats.hasAdequateSample) ss << " [LOW SAMPLE]";
        ss << " | Games: " << counts.totalGames << "\n";
    }
    ss << "\n";
    
    // Shell Count Analysis
    ss << "Shell Count Effects:\n";
    ss << "--------------------\n";
    for (const auto& pair : numShellsAnalysis) {
        int shells = pair.first;
        const GameOutcomeCounts& counts = pair.second;
        
        StatResult p1Stats = calculateStats(counts.player1Wins, counts.totalGames);
        StatResult p2Stats = calculateStats(counts.player2Wins, counts.totalGames);
        
        ss << "Shells " << std::setw(2) << shells << ": ";
        ss << "P1: " << std::setw(4) << p1Stats.winRate << "% ";
        ss << "[±" << std::setw(4) << p1Stats.marginOfError << "%]";
        if (p1Stats.isSignificant) ss << " *";
        
        ss << " | P2: " << std::setw(4) << p2Stats.winRate << "% ";
        ss << "[±" << std::setw(4) << p2Stats.marginOfError << "%]";
        if (p2Stats.isSignificant) ss << " *";
        
        if (!p1Stats.hasAdequateSample) ss << " [LOW SAMPLE]";
        ss << " | Games: " << counts.totalGames << "\n";
    }
    ss << "\n";
    
    // Tank Count Analysis
    ss << "Tank Count Effects:\n";
    ss << "-------------------\n";
    for (const auto& pair : numTanksAnalysis) {
        int tanks = pair.first;
        const GameOutcomeCounts& counts = pair.second;
        
        StatResult p1Stats = calculateStats(counts.player1Wins, counts.totalGames);
        StatResult p2Stats = calculateStats(counts.player2Wins, counts.totalGames);
        
        ss << "Tanks " << std::setw(1) << tanks << ": ";
        ss << "P1: " << std::setw(4) << p1Stats.winRate << "% ";
        ss << "[±" << std::setw(4) << p1Stats.marginOfError << "%]";
        if (p1Stats.isSignificant) ss << " *";
        
        ss << " | P2: " << std::setw(4) << p2Stats.winRate << "% ";
        ss << "[±" << std::setw(4) << p2Stats.marginOfError << "%]";
        if (p2Stats.isSignificant) ss << " *";
        
        if (!p1Stats.hasAdequateSample) ss << " [LOW SAMPLE]";
        ss << " | Games: " << counts.totalGames << "\n";
    }
    ss << "\n";
    
    // Legend
    ss << "Legend:\n";
    ss << "* = Statistically significant (performance different from 50%)\n";
    ss << "[±X%] = 95% confidence interval margin of error\n";
    ss << "[LOW SAMPLE] = Sample size < 30, results may be unreliable\n\n";
    
    return ss.str();
}

bool AnalysisSummarizer::saveReportToFile(const std::string& report, const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return false;
        }
        
        file << report;
        file.close();
        
        std::cout << "Summary report saved to: " << filename << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error writing summary report: " << e.what() << std::endl;
        return false;
    }
}

std::string AnalysisSummarizer::generateSummaryReport(
    const std::map<std::string, GameOutcomeCounts>& overallResults,
    const std::map<int, GameOutcomeCounts>& boardSizeAnalysis,
    const std::map<float, GameOutcomeCounts>& wallDensityAnalysis,
    const std::map<float, GameOutcomeCounts>& mineDensityAnalysis,  
    const std::map<int, GameOutcomeCounts>& numShellsAnalysis,
    const std::map<int, GameOutcomeCounts>& numTanksAnalysis
) {
    std::stringstream report;
    
    // Calculate total games for header
    int totalGames = 0;
    for (const auto& pair : overallResults) {
        totalGames += pair.second.totalGames;
    }
    
    // Generate each section of the report
    report << generateHeader(totalGames);
    report << generateOverallResults(overallResults);
    report << generateDimensionalAnalysis(
        boardSizeAnalysis,
        wallDensityAnalysis,
        mineDensityAnalysis,
        numShellsAnalysis,
        numTanksAnalysis
    );
    
    // Add footer
    report << "===========================================\n";
    report << "Analysis complete. Check CSV files in output/ directory for detailed data.\n";
    
    return report.str();
}

AnalysisSummarizer::StatResult AnalysisSummarizer::calculateStats(int wins, int totalGames) {
    if (totalGames == 0) {
        return {0.0, 0.0, false, false};
    }
    
    double p = (double)wins / totalGames;
    double standardError = sqrt(p * (1 - p) / totalGames);
    double marginOfError = 1.96 * standardError * 100.0; // Convert to percentage
    
    // Check significance: is 50% outside the confidence interval?
    bool isSignificant = (p + 1.96 * standardError < 0.5) || (p - 1.96 * standardError > 0.5);
    
    // Check sample adequacy
    bool hasAdequateSample = (totalGames >= 30) && (wins >= 5) && ((totalGames - wins) >= 5);
    
    return {p * 100.0, marginOfError, isSignificant, hasAdequateSample};
}