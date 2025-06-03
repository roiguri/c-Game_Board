#include "bonus/analysis/analysis_summarizer.h"
#include "bonus/analysis/analysis_stats.h" // For statistical calculations
#include <sstream>
#include <iomanip>
#include <vector>    // For std::vector to hold map keys for pairwise iteration
#include <algorithm> // For std::sort (optional, but good for consistent pair ordering)
#include <ctime>
#include <chrono>
#include <fstream>
#include <iostream>

AnalysisSummarizer::AnalysisSummarizer() {
    // Constructor - nothing to initialize for now
}

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
    const std::map<int, GameOutcomeCounts>& widthAnalysis,
    const std::map<int, GameOutcomeCounts>& heightAnalysis,
    const std::map<float, GameOutcomeCounts>& wallDensityAnalysis,
    const std::map<float, GameOutcomeCounts>& mineDensityAnalysis,
    const std::map<std::string, GameOutcomeCounts>& symmetryAnalysis,
    const std::map<int, GameOutcomeCounts>& maxStepsAnalysis,
    const std::map<int, GameOutcomeCounts>& numShellsAnalysis,
    const std::map<int, GameOutcomeCounts>& numTanksAnalysis
) {
    std::stringstream ss;
    ss << "DIMENSIONAL ANALYSIS\n";
    ss << "===================\n\n";
    ss << std::fixed << std::setprecision(1); // Default precision for win rates

    // Helper lambda for pairwise comparisons
    // Takes the analysis map, dimension name, and the string stream
    // K is the key type (int, float, string)
    // V is GameOutcomeCounts
    auto performPairwiseAnalysis =
        [&](const auto& analysisMap, const std::string& dimensionName, std::ostringstream& stream) {
        stream << dimensionName << " Effects:\n";
        stream << std::string(dimensionName.length() + 8, '-') << "\n"; // Dynamic underline

        // Print individual stats first
        for (const auto& pair : analysisMap) {
            const auto& key = pair.first;
            const GameOutcomeCounts& counts = pair.second;
            double p1Rate = calculateWinRate(counts.player1Wins, counts.totalGames);
            double p2Rate = calculateWinRate(counts.player2Wins, counts.totalGames);
            double tieRate = calculateWinRate(counts.ties, counts.totalGames);

            // Adjust precision for float keys like density
            if constexpr (std::is_same_v<std::decay_t<decltype(key)>, float>) {
                 stream << dimensionName << " " << std::setw(4) << std::setprecision(2) << key << ": ";
            } else {
                 stream << dimensionName << " " << std::setw( (dimensionName == "Symmetry" ? 10 : (dimensionName == "Steps" ? 4 : 2) ) ) << key << ": ";
            }
            stream << std::setprecision(1); // Back to 1 for percentages
            stream << "P1: " << std::setw(4) << p1Rate << "% | ";
            stream << "P2: " << std::setw(4) << p2Rate << "% | ";
            stream << "Ties: " << std::setw(4) << tieRate << "% | ";
            stream << "Games: " << counts.totalGames << "\n";
        }

        // Perform pairwise comparisons if there are enough distinct values
        if (analysisMap.size() > 1) {
            stream << "\n  " << dimensionName << " Pairwise Comparisons (Player 1 Win Rate):\n";
            stream << "  " << std::string(dimensionName.length() + 40, '-') << "\n";

            // Extract keys to allow iteration with indices for pairs
            std::vector<typename std::decay_t<decltype(analysisMap.begin()->first)>> keys;
            for (const auto& pair : analysisMap) {
                keys.push_back(pair.first);
            }
            // Optional: sort keys for consistent comparison order, especially for strings/floats
            // std::sort(keys.begin(), keys.end());

            int numComparisons = keys.size() * (keys.size() - 1) / 2;
            if (numComparisons == 0) numComparisons = 1; // Avoid division by zero if only 2 levels (1 comparison)

            const double ALPHA = 0.05;

            for (size_t i = 0; i < keys.size(); ++i) {
                for (size_t j = i + 1; j < keys.size(); ++j) {
                    const auto& key1 = keys[i];
                    const auto& key2 = keys[j];
                    const GameOutcomeCounts& counts1 = analysisMap.at(key1);
                    const GameOutcomeCounts& counts2 = analysisMap.at(key2);

                    stream << "  Comparing " << dimensionName << " " << key1 << " vs " << dimensionName << " " << key2 << ":\n";

                    std::string recommendation = AnalysisStats::getSampleSizeRecommendation(counts1.totalGames, counts2.totalGames);
                    if (!recommendation.empty()) {
                        stream << "    " << recommendation << "\n";
                    }

                    // Perform ChiSquare only if sample sizes are somewhat reasonable (e.g. recommendation is empty or a softer check)
                    // For now, let's proceed even with recommendation, but flag it. User should be aware.
                    // A stricter rule could be: if (!recommendation.empty() && (counts1.totalGames < 10 || counts2.totalGames < 10)) { skip }

                    AnalysisStats::ChiSquareResult chiResult = AnalysisStats::calculateChiSquareTest(
                        counts1.player1Wins, counts1.totalGames,
                        counts2.player1Wins, counts2.totalGames
                    );

                    bool significant = AnalysisStats::isStatisticallySignificant(chiResult.pValue, ALPHA, numComparisons);

                    stream << std::fixed << std::setprecision(3); // Higher precision for p-value and chi2
                    stream << "    P1 Win Rate: Chi2(" << chiResult.chiSquareValue << "), p-val(" << chiResult.pValue << "). ";
                    stream << (significant ? "Significant" : "Not significant")
                           << " (Bonferroni alpha=" << (ALPHA / numComparisons) << ").\n";

                    if (chiResult.warningNoExpectedFrequencyBelowFive) {
                        stream << "    Warning: Chi-Square assumption (all expected frequencies >= 5) may not be met.\n";
                    }

                    stream << std::fixed << std::setprecision(1); // Back to 1 for CI percentages
                    AnalysisStats::ConfidenceInterval ci1 = AnalysisStats::calculateConfidenceInterval(counts1.player1Wins, counts1.totalGames);
                    stream << "    P1 Win Rate CI for " << dimensionName << " " << key1 << ": ["
                           << ci1.lowerBound * 100.0 << "% - " << ci1.upperBound * 100.0 << "%]\n";

                    AnalysisStats::ConfidenceInterval ci2 = AnalysisStats::calculateConfidenceInterval(counts2.player1Wins, counts2.totalGames);
                    stream << "    P1 Win Rate CI for " << dimensionName << " " << key2 << ": ["
                           << ci2.lowerBound * 100.0 << "% - " << ci2.upperBound * 100.0 << "%]\n";
                    stream << "\n";
                }
            }
        }
        stream << "\n"; // Final newline after each dimension's analysis
    };

    // Apply the helper to each dimension
    performPairwiseAnalysis(widthAnalysis, "Width", ss);
    performPairwiseAnalysis(heightAnalysis, "Height", ss);
    performPairwiseAnalysis(wallDensityAnalysis, "Wall Density", ss);
    performPairwiseAnalysis(mineDensityAnalysis, "Mine Density", ss);
    performPairwiseAnalysis(symmetryAnalysis, "Symmetry", ss);
    performPairwiseAnalysis(maxStepsAnalysis, "Max Steps", ss);
    performPairwiseAnalysis(numShellsAnalysis, "Shells", ss);
    performPairwiseAnalysis(numTanksAnalysis, "Tanks", ss);

    return ss.str();
}
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
    const std::map<int, GameOutcomeCounts>& widthAnalysis,
    const std::map<int, GameOutcomeCounts>& heightAnalysis,
    const std::map<float, GameOutcomeCounts>& wallDensityAnalysis,
    const std::map<float, GameOutcomeCounts>& mineDensityAnalysis,
    const std::map<std::string, GameOutcomeCounts>& symmetryAnalysis,
    const std::map<int, GameOutcomeCounts>& maxStepsAnalysis,
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
        widthAnalysis,
        heightAnalysis,
        wallDensityAnalysis,
        mineDensityAnalysis,
        symmetryAnalysis,
        maxStepsAnalysis,
        numShellsAnalysis,
        numTanksAnalysis
    );

    // Add footer
    report << "===========================================\n";
    report << "Analysis complete. Check CSV files in output/ directory for detailed data.\n";

    return report.str();
}