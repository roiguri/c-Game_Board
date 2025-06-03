#pragma once

#include <string>
#include <map>
#include <vector>
#include "bonus/analysis/analysis_tool.h"  // For Winner enum and GameOutcomeCounts

/**
 * @brief Generates structured summary reports from analysis results
 * 
 * Takes aggregated analysis data and produces human-readable summary
 * reports with statistics, dimensional breakdowns, and insights.
 */
class AnalysisSummarizer {
public:
    /**
     * @brief Constructor
     */
    AnalysisSummarizer();
    
    /**
     * @brief Generate a comprehensive summary report
     * 
     * @param overallResults Results grouped by configuration key
     * @param boardSizeAnalysis Results grouped by board size
     * @param wallDensityAnalysis Results grouped by wall density
     * @param mineDensityAnalysis Results grouped by mine density
     * @param numShellsAnalysis Results grouped by shell count
     * @param numTanksAnalysis Results grouped by tank count
     * @return Formatted summary report as string
     */
    std::string generateSummaryReport(
        const std::map<std::string, GameOutcomeCounts>& overallResults,
        const std::map<int, GameOutcomeCounts>& boardSizeAnalysis,
        const std::map<float, GameOutcomeCounts>& wallDensityAnalysis,
        const std::map<float, GameOutcomeCounts>& mineDensityAnalysis,
        const std::map<int, GameOutcomeCounts>& numShellsAnalysis,
        const std::map<int, GameOutcomeCounts>& numTanksAnalysis
    );
    
    /**
     * @brief Save summary report to file
     * 
     * @param report The generated report content
     * @param filename Path to save the report
     * @return true if saved successfully, false otherwise
     */
    bool saveReportToFile(const std::string& report, const std::string& filename);

private:
    struct StatResult {
        double winRate;
        double marginOfError;
        bool isSignificant;
        bool hasAdequateSample;
    };
    
    StatResult calculateStats(int wins, int totalGames);

    /**
     * @brief Generate header section of the report
     * 
     * @param totalGames Total number of games played
     * @return Formatted header string
     */
    std::string generateHeader(int totalGames);
    
    /**
     * @brief Generate overall results section
     * 
     * @param overallResults Complete results data
     * @return Formatted overall results string
     */
    std::string generateOverallResults(const std::map<std::string, GameOutcomeCounts>& overallResults);
    
    /**
     * @brief Generate dimensional analysis section
     * 
     * @param boardSizeAnalysis Results by board size
     * @param wallDensityAnalysis Results by wall density
     * @param mineDensityAnalysis Results by mine density
     * @param numShellsAnalysis Results by shell count
     * @param numTanksAnalysis Results by tank count
     * @return Formatted dimensional analysis string
     */
    std::string generateDimensionalAnalysis(
        const std::map<int, GameOutcomeCounts>& boardSizeAnalysis,
        const std::map<float, GameOutcomeCounts>& wallDensityAnalysis,
        const std::map<float, GameOutcomeCounts>& mineDensityAnalysis,
        const std::map<int, GameOutcomeCounts>& numShellsAnalysis,
        const std::map<int, GameOutcomeCounts>& numTanksAnalysis
    );
    
    /**
     * @brief Calculate win rate percentage
     * 
     * @param wins Number of wins
     * @param totalGames Total number of games
     * @return Win rate as percentage (0.0 to 100.0)
     */
    double calculateWinRate(int wins, int totalGames);
    
    /**
     * @brief Get current timestamp as string
     * 
     * @return Formatted timestamp
     */
    std::string getCurrentTimestamp();
};