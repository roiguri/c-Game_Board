#pragma once

#include <string>
#include <map>
#include "bonus/board_generator.h" // For BoardConfig
#include "bonus/analysis/analysis_tool.h" // For Winner, GameOutcomeCounts
#include "bonus/analysis/analysis_summarizer.h"

class ResultAggregator {
public:
    void updateResults(const BoardConfig& config, const std::string& configKey, Winner outcome);
    void writeCSVs();
    void printSummaries();
    void generateSummaryReport();

    // Expose maps for testing or further use if needed
    const std::map<std::string, GameOutcomeCounts>& getAggregatedResults() const { return aggregatedResults; }
private:
    std::map<std::string, GameOutcomeCounts> aggregatedResults;
    std::map<int, GameOutcomeCounts> widthAnalysis;
    std::map<int, GameOutcomeCounts> heightAnalysis;
    std::map<float, GameOutcomeCounts> wallDensityAnalysis;
    std::map<float, GameOutcomeCounts> mineDensityAnalysis;
    std::map<std::string, GameOutcomeCounts> symmetryAnalysis;
    std::map<int, GameOutcomeCounts> maxStepsAnalysis;
    std::map<int, GameOutcomeCounts> numShellsAnalysis;
    std::map<int, GameOutcomeCounts> numTanksPerPlayerAnalysis;
    AnalysisSummarizer m_summarizer;
}; 