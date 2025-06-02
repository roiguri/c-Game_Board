#pragma once

#include <vector>
#include <string>
#include <map>
#include "bonus/board_generator.h"
#include "bonus/analysis/analysis_params.h"

enum class Winner { PLAYER1, PLAYER2, TIE, UNKNOWN };

struct GameOutcomeCounts {
    int player1Wins = 0;
    int player2Wins = 0;
    int ties = 0;
    int unknownOutcomes = 0;
    int totalGames = 0;
};

class BoardManager;
class ResultAggregator;
class AnalysisReporter;

class AnalysisTool {
public:
    int runAnalysis();
    int runAnalysisWithConfig(const std::string& configFile);
private:
    static std::vector<BoardConfig> generateAllConfigs(const AnalysisParams& params);
    void runSingleSimulation(const BoardConfig& config, ResultAggregator& aggregator);
};