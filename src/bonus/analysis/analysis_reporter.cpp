#include "bonus/analysis/analysis_reporter.h"
#include <fstream>
#include <iostream>

void AnalysisReporter::writeOverallResultsCsv(const std::string& filename, const std::map<std::string, GameOutcomeCounts>& results) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }
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

void AnalysisReporter::writeGrandTotalCsv(const std::string& filename, const GameOutcomeCounts& grandTotalCounts) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }
    file << "totalGames,player1Wins,player2Wins,ties,unknownOutcomes\n";
    file << grandTotalCounts.totalGames << ','
         << grandTotalCounts.player1Wins << ','
         << grandTotalCounts.player2Wins << ','
         << grandTotalCounts.ties << ','
         << grandTotalCounts.unknownOutcomes << '\n';
    file.close();
}

void AnalysisReporter::writeGrandTotalOdds(std::ofstream& outFile, const GameOutcomeCounts& grandTotalCounts) {
    if (!outFile.is_open()) {
        std::cerr << "Error: Output file stream is not open for grand total odds." << std::endl;
        return;
    }
    outFile << "Grand Total Odds\n";
    outFile << "Statistic,P1WinOdds,P2WinOdds\n";
    outFile << "Overall," 
            << std::fixed << std::setprecision(4) << grandTotalCounts.p1WinOdds << ',' 
            << std::fixed << std::setprecision(4) << grandTotalCounts.p2WinOdds << '\n';
    outFile << "\n"; // Add a blank line for separation
}