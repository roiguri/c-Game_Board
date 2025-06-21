#include <fstream>
#include <iostream>

#include "UserCommon/bonus/analysis/analysis_reporter.h"

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