#pragma once

#include <map>
#include <string>
#include <vector>

#include "UserCommon/bonus/analysis/analysis_params.h"
#include "UserCommon/bonus/board_generator.h"

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
    int runAnalysis(const std::string& configFile);
private:
    static std::vector<BoardConfig> generateAllConfigs(const AnalysisParams& params);
    void runSingleSimulation(const BoardConfig& config, ResultAggregator& aggregator);
};