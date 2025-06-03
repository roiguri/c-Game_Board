#include "bonus/analysis/analysis_summarizer.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <fstream>
#include <iostream>

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
    
    std::stringstream ss;
    ss << "OVERALL RESULTS\n";
    ss << "===============\n";
    ss << std::fixed << std::setprecision(1);
    ss << "Player 1 Wins: " << totalPlayer1Wins << " (" << calculateWinRate(totalPlayer1Wins, totalGames) << "%)\n";
    ss << "Player 2 Wins: " << totalPlayer2Wins << " (" << calculateWinRate(totalPlayer2Wins, totalGames) << "%)\n";
    ss << "Ties: " << totalTies << " (" << calculateWinRate(totalTies, totalGames) << "%)\n";
    ss << "Total Games: " << totalGames << "\n\n";
    
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
    ss << "--------------------\n";
    for (const auto& pair : boardSizeAnalysis) {
        int size = pair.first;
        const GameOutcomeCounts& counts = pair.second;
        double p1Rate = calculateWinRate(counts.player1Wins, counts.totalGames);
        double p2Rate = calculateWinRate(counts.player2Wins, counts.totalGames);
        double tieRate = calculateWinRate(counts.ties, counts.totalGames);
        
        ss << "Size " << std::setw(2) << size << "x" << size << ": ";
        ss << "P1: " << std::setw(4) << p1Rate << "% | ";
        ss << "P2: " << std::setw(4) << p2Rate << "% | ";
        ss << "Ties: " << std::setw(4) << tieRate << "% | ";
        ss << "Games: " << counts.totalGames << "\n";
    }
    ss << "\n";
    
    // Wall Density Analysis
    ss << "Wall Density Effects:\n";
    ss << "---------------------\n";
    for (const auto& pair : wallDensityAnalysis) {
        float density = pair.first;
        const GameOutcomeCounts& counts = pair.second;
        double p1Rate = calculateWinRate(counts.player1Wins, counts.totalGames);
        double p2Rate = calculateWinRate(counts.player2Wins, counts.totalGames);
        double tieRate = calculateWinRate(counts.ties, counts.totalGames);
        
        ss << "Density " << std::setw(4) << std::setprecision(2) << density << ": ";
        ss << "P1: " << std::setw(4) << std::setprecision(1) << p1Rate << "% | ";
        ss << "P2: " << std::setw(4) << p2Rate << "% | ";
        ss << "Ties: " << std::setw(4) << tieRate << "% | ";
        ss << "Games: " << counts.totalGames << "\n";
    }
    ss << "\n";
    
    // Mine Density Analysis
    ss << "Mine Density Effects:\n";
    ss << "---------------------\n";
    for (const auto& pair : mineDensityAnalysis) {
        float density = pair.first;
        const GameOutcomeCounts& counts = pair.second;
        double p1Rate = calculateWinRate(counts.player1Wins, counts.totalGames);
        double p2Rate = calculateWinRate(counts.player2Wins, counts.totalGames);
        double tieRate = calculateWinRate(counts.ties, counts.totalGames);
        
        ss << "Density " << std::setw(4) << std::setprecision(2) << density << ": ";
        ss << "P1: " << std::setw(4) << std::setprecision(1) << p1Rate << "% | ";
        ss << "P2: " << std::setw(4) << p2Rate << "% | ";
        ss << "Ties: " << std::setw(4) << tieRate << "% | ";
        ss << "Games: " << counts.totalGames << "\n";
    }
    ss << "\n";
    
    // Shell Count Analysis
    ss << "Shell Count Effects:\n";
    ss << "--------------------\n";
    for (const auto& pair : numShellsAnalysis) {
        int shells = pair.first;
        const GameOutcomeCounts& counts = pair.second;
        double p1Rate = calculateWinRate(counts.player1Wins, counts.totalGames);
        double p2Rate = calculateWinRate(counts.player2Wins, counts.totalGames);
        double tieRate = calculateWinRate(counts.ties, counts.totalGames);
        
        ss << "Shells " << std::setw(2) << shells << ": ";
        ss << "P1: " << std::setw(4) << p1Rate << "% | ";
        ss << "P2: " << std::setw(4) << p2Rate << "% | ";
        ss << "Ties: " << std::setw(4) << tieRate << "% | ";
        ss << "Games: " << counts.totalGames << "\n";
    }
    ss << "\n";
    
    // Tank Count Analysis
    ss << "Tank Count Effects:\n";
    ss << "-------------------\n";
    for (const auto& pair : numTanksAnalysis) {
        int tanks = pair.first;
        const GameOutcomeCounts& counts = pair.second;
        double p1Rate = calculateWinRate(counts.player1Wins, counts.totalGames);
        double p2Rate = calculateWinRate(counts.player2Wins, counts.totalGames);
        double tieRate = calculateWinRate(counts.ties, counts.totalGames);
        
        ss << "Tanks " << std::setw(1) << tanks << ": ";
        ss << "P1: " << std::setw(4) << p1Rate << "% | ";
        ss << "P2: " << std::setw(4) << p2Rate << "% | ";
        ss << "Ties: " << std::setw(4) << tieRate << "% | ";
        ss << "Games: " << counts.totalGames << "\n";
    }
    ss << "\n";
    
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