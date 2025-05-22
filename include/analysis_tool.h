#pragma once

#include <vector>
#include <string>
#include <map>

// Forward declaration for BoardConfig
struct BoardConfig;

struct AnalysisParams {
    std::vector<int> widths;
    std::vector<int> heights;
    std::vector<float> wallDensities;
    std::vector<float> mineDensities;
    std::vector<std::string> symmetryTypes;
    std::vector<int> seeds;
    std::vector<int> maxSteps;
    std::vector<int> numShells;
    std::vector<int> numTanksPerPlayer;
};

enum class Winner { PLAYER1, PLAYER2, TIE, UNKNOWN };

struct GameOutcomeCounts {
    int player1Wins = 0;
    int player2Wins = 0;
    int ties = 0;
    int unknownOutcomes = 0;
    int totalGames = 0;
};

Winner ParseGameResult(const std::string& resultLine);
std::string GenerateKey(const BoardConfig& config);

void writeOverallResultsCsv(const std::string& filename, const std::map<std::string, GameOutcomeCounts>& results);

template<typename KeyType>
void writeDimensionAnalysisCsv(const std::string& filename, const std::string& dimensionName, const std::map<KeyType, GameOutcomeCounts>& analysisMap);

template<typename KeyType>
void printDimensionAnalysis(const std::string& dimensionName, const std::map<KeyType, GameOutcomeCounts>& analysisMap);