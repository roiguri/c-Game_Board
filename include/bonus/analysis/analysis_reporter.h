#pragma once

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <iomanip>
#include <vector>   // Added
#include <numeric>  // Added
#include <cmath>    // Added
#include "bonus/analysis/analysis_tool.h" // For GameOutcomeCounts

class AnalysisReporter {
private:
    static double calculateMean(const std::vector<double>& values) {
        if (values.empty()) {
            return 0.0;
        }
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / values.size();
    }

    static double calculateStdDev(const std::vector<double>& values, double mean) {
        if (values.size() < 2) { // Standard deviation is not well-defined for less than 2 values
            return 0.0;
        }
        double sq_sum = 0.0;
        for (double val : values) {
            sq_sum += (val - mean) * (val - mean);
        }
        return std::sqrt(sq_sum / (values.size() - 1)); // Sample standard deviation
    }

public:
    static void writeOverallResultsCsv(const std::string& filename, const std::map<std::string, GameOutcomeCounts>& results);
    static void writeGrandTotalCsv(const std::string& filename, const GameOutcomeCounts& grandTotalCounts);

    // Writes to an output file stream
    template<typename KeyType>
    static void writeDimensionAnalysisCsv(std::ofstream& outFile, const std::string& dimensionName, const std::map<KeyType, GameOutcomeCounts>& analysisMap) {
        if (!outFile.is_open()) {
            std::cerr << "Error: Output file stream is not open for dimension: " << dimensionName << std::endl;
            return;
        }
        outFile << "Dimension: " << dimensionName << "\n"; // Header for the dimension
        outFile << dimensionName << ",totalGames,player1Wins,player2Wins,ties,unknownOutcomes\n";
        for (const auto& pair : analysisMap) {
            outFile << pair.first << ','
                    << pair.second.totalGames << ','
                    << pair.second.player1Wins << ','
                    << pair.second.player2Wins << ','
                    << pair.second.ties << ','
                    << pair.second.unknownOutcomes << '\n';
        }
        outFile << "\n"; // Add a blank line for separation
    }

    // Overload that takes a filename for backward compatibility or direct use
    template<typename KeyType>
    static void writeDimensionAnalysisCsv(const std::string& filename, const std::string& dimensionName, const std::map<KeyType, GameOutcomeCounts>& analysisMap) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return;
        }
        // Call the stream version
        writeDimensionAnalysisCsv(file, dimensionName, analysisMap);
        file.close();
    }

    template<typename KeyType>
    static void writeOddsStatisticsCsv(std::ofstream& outFile, const std::string& dimensionName, const std::map<KeyType, GameOutcomeCounts>& analysisMap) {
        if (!outFile.is_open()) {
            std::cerr << "Error: Output file stream is not open for odds statistics: " << dimensionName << std::endl;
            return;
        }
        outFile << "Dimension: " << dimensionName << "\n";
        outFile << "DimensionValue,P1WinOdds,P2WinOdds\n";
        
        std::vector<double> p1OddsList;
        std::vector<double> p2OddsList;
        p1OddsList.reserve(analysisMap.size());
        p2OddsList.reserve(analysisMap.size());

        for (const auto& pair : analysisMap) {
            outFile << pair.first << ','
                    << std::fixed << std::setprecision(4) << pair.second.p1WinOdds << ','
                    << std::fixed << std::setprecision(4) << pair.second.p2WinOdds << '\n';
            p1OddsList.push_back(pair.second.p1WinOdds);
            p2OddsList.push_back(pair.second.p2WinOdds);
        }

        double meanP1 = calculateMean(p1OddsList);
        double stdDevP1 = calculateStdDev(p1OddsList, meanP1);
        double meanP2 = calculateMean(p2OddsList);
        double stdDevP2 = calculateStdDev(p2OddsList, meanP2);

        outFile << "MeanP1WinOdds," << std::fixed << std::setprecision(4) << meanP1 << "\n";
        outFile << "StdDevP1WinOdds," << std::fixed << std::setprecision(4) << stdDevP1 << "\n";
        outFile << "MeanP2WinOdds," << std::fixed << std::setprecision(4) << meanP2 << "\n";
        outFile << "StdDevP2WinOdds," << std::fixed << std::setprecision(4) << stdDevP2 << "\n";
        outFile << "\n"; // Add a blank line for separation
    }

    static void writeGrandTotalOdds(std::ofstream& outFile, const GameOutcomeCounts& grandTotalCounts);

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