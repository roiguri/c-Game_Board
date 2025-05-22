#pragma once

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <iomanip>
#include "bonus/analysis/analysis_tool.h" // For GameOutcomeCounts

class AnalysisReporter {
public:
    static void writeOverallResultsCsv(const std::string& filename, const std::map<std::string, GameOutcomeCounts>& results);
    template<typename KeyType>
    static void writeDimensionAnalysisCsv(const std::string& filename, const std::string& dimensionName, const std::map<KeyType, GameOutcomeCounts>& analysisMap) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return;
        }
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
    template<typename KeyType>
    static void printDimensionAnalysis(const std::string& dimensionName, const std::map<KeyType, GameOutcomeCounts>& analysisMap) {
        std::cout << "\n--- Analysis by " << dimensionName << " ---" << std::endl;
        if (analysisMap.empty()) {
            std::cout << "No data available for this dimension." << std::endl;
            return;
        }
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
                std::cout << std::fixed << std::setprecision(1);
                std::cout << "  P1 Win %: " << p1WinRate << "%" << std::endl;
                std::cout << "  P2 Win %: " << p2WinRate << "%" << std::endl;
                std::cout << "  Tie %: " << tieRate << "%" << std::endl;
                if (counts.unknownOutcomes > 0) {
                     double unknownRate = (static_cast<double>(counts.unknownOutcomes) / counts.totalGames) * 100.0;
                     std::cout << "  Unknown Outcomes: " << counts.unknownOutcomes << " (" << unknownRate << "%)" << std::endl;
                }
                std::cout << std::defaultfloat << std::setprecision(6);
            }
            std::cout << "  ---------------------------" << std::endl;
        }
    }
}; 