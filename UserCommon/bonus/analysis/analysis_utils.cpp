#include <iomanip>
#include <iostream>
#include <sstream>

#include "UserCommon/bonus/analysis/analysis_utils.h"

Winner ParseGameResult(const std::string& resultLine) {
    if (resultLine.find("Player 1 won") != std::string::npos) {
        return Winner::PLAYER1;
    } else if (resultLine.find("Player 2 won") != std::string::npos) {
        return Winner::PLAYER2;
    } else if (resultLine.find("Tie,") != std::string::npos) {
        return Winner::TIE;
    }
    std::cerr << "Warning: Unknown game result string encountered: \"" << resultLine << "\"" << std::endl;
    return Winner::UNKNOWN;
}

std::string GenerateKey(const BoardConfig& config) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    oss << "w" << config.width
        << "_h" << config.height
        << "_wd" << config.wallDensity
        << "_md" << config.mineDensity;
    oss << std::defaultfloat << std::setprecision(6);
    oss << "_sym" << config.symmetry
        << "_seed" << config.seed
        << "_steps" << config.maxSteps
        << "_shells" << config.numShells
        << "_tanks" << config.numTanksPerPlayer;
    return oss.str();
}