#pragma once

#include <map>
#include <string>

#include "UserCommon/bonus/analysis/analysis_summarizer.h"
#include "UserCommon/bonus/analysis/analysis_tool.h" // For Winner, GameOutcomeCounts
#include "UserCommon/bonus/board_generator.h" // For BoardConfig

class ResultAggregator {
public:
    void updateResults(const BoardConfig& config, const std::string& configKey, Winner outcome);
    void writeCSVs();
    void generateSummaryReport();

    // Expose maps for testing or further use if needed
    const std::map<std::string, GameOutcomeCounts>& getAggregatedResults() const { return aggregatedResults; }
private:
    std::map<std::string, GameOutcomeCounts> aggregatedResults;
    std::map<int, GameOutcomeCounts> boardSizeAnalysis;
    std::map<float, GameOutcomeCounts> wallDensityAnalysis;
    std::map<float, GameOutcomeCounts> mineDensityAnalysis;
    std::map<int, GameOutcomeCounts> numShellsAnalysis;
    std::map<int, GameOutcomeCounts> numTanksPerPlayerAnalysis;
    AnalysisSummarizer m_summarizer;
}; 