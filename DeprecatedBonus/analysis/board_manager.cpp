#include <filesystem>
#include <iostream>

#include "UserCommon/bonus/analysis/board_manager.h"

bool BoardManager::generateBoard(const BoardConfig& config) {
    m_generator = BoardGenerator(config);
    return m_generator.generateBoard();
}

bool BoardManager::saveToFile(const BoardConfig& /*config*/, const std::string& filePath) {
    // Do NOT recreate m_generator here! Just save the already-generated board.
    return m_generator.saveToFile(filePath);
}

void BoardManager::cleanupTempFiles(const std::string& configKey) {
    const std::string tempBoardFilePath = "temp_analysis_board_" + configKey + ".txt";
    const std::string outputFilePath = "output_temp_analysis_board_" + configKey + ".txt";
    const std::string visualizationFilePath = "output_temp_analysis_board_" + configKey + "_visualization.html";
    try {
        if (std::filesystem::exists(tempBoardFilePath)) {
            if (!std::filesystem::remove(tempBoardFilePath)) {
                std::cerr << "Warning: Failed to remove temporary board file: " << tempBoardFilePath << std::endl;
            }
        }
        if (std::filesystem::exists(outputFilePath)) {
            if (!std::filesystem::remove(outputFilePath)) {
                std::cerr << "Warning: Failed to remove temporary output file: " << outputFilePath << std::endl;
            }
        }
        if (std::filesystem::exists(visualizationFilePath)) {
            if (!std::filesystem::remove(visualizationFilePath)) {
                std::cerr << "Warning: Failed to remove temporary visualization file: " << visualizationFilePath << std::endl;
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error during cleanup for " << configKey << ": " << e.what() << std::endl;
    }
} 