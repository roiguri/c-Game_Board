#pragma once

#include <string>

#include "UserCommon/bonus/board_generator.h" // For BoardConfig

class BoardManager {
public:
    // Generates a board using the given config. Returns true on success.
    bool generateBoard(const BoardConfig& config);
    // Saves the generated board to a file. Returns true on success.
    bool saveToFile(const BoardConfig& config, const std::string& filePath);
    // Cleans up temporary files related to a config key (board, output, visualization).
    void cleanupTempFiles(const std::string& configKey);
private:
    BoardGenerator m_generator;
}; 