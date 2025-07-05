#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include "game_runner.h"
#include "common/SatelliteView.h"

/**
 * @brief Utility class for output generation and formatting operations across game modes
 * 
 * This class consolidates output generation logic that was previously duplicated
 * across multiple game mode implementations (ComparativeRunner, CompetitiveRunner).
 * It provides methods for timestamp generation, file writing, game result formatting,
 * and path creation using standard C++ libraries for cross-platform compatibility.
 */
class OutputGenerator {
public:
    /**
     * @brief Generate ISO 8601 formatted timestamp for file naming
     * 
     * Creates a timestamp string suitable for use in output filenames.
     * Uses format: YYYYMMDD_HHMMSS for basic timestamp or 
     * YYYYMMDD_HHMMSS_mmm for high-precision timestamp with milliseconds.
     * 
     * @param includeMilliseconds Whether to include milliseconds in timestamp
     * @return String containing formatted timestamp
     */
    static std::string generateTimestamp(bool includeMilliseconds = false);

    /**
     * @brief Write content to file with proper error handling
     * 
     * Attempts to write the given content to the specified file path.
     * If file creation fails, optionally falls back to console output.
     * 
     * @param filePath Path where the file should be written
     * @param content Content to write to the file
     * @param fallbackToConsole If true, write to console when file creation fails
     * @return True if file was written successfully, false otherwise
     */
    static bool writeToFile(const std::string& filePath, const std::string& content, bool fallbackToConsole = true);

    /**
     * @brief Format game result for output display
     * 
     * Converts a GameResult object into a formatted string representation
     * suitable for output files or console display.
     * 
     * @param result GameResult to format
     * @return Formatted string representation of the game result
     */
    static std::string formatGameResult(const GameResult& result);

    /**
     * @brief Convert game state to string representation
     * 
     * Converts a SatelliteView game state into a multi-line string
     * representation showing the current board state.
     * 
     * @param gameState SatelliteView containing the game state
     * @param rows Number of rows in the game board
     * @param cols Number of columns in the game board
     * @return String representation of the game state
     */
    static std::string formatGameState(const SatelliteView& gameState, int rows, int cols);

    /**
     * @brief Create output file path with timestamp
     * 
     * Generates an output file path by combining a base name with a timestamp.
     * Supports optional directory prefix and custom file extension.
     * 
     * @param baseName Base name for the output file (e.g., "competitive_results")
     * @param directory Optional directory path where file should be created
     * @param extension File extension (default: ".txt")
     * @param includeMilliseconds Whether timestamp should include milliseconds
     * @return Complete file path with timestamp
     */
    static std::string createOutputPath(
        const std::string& baseName,
        const std::string& directory = "",
        const std::string& extension = ".txt",
        bool includeMilliseconds = false
    );

    /**
     * @brief Write content to stream with error handling
     * 
     * Helper method to write content to either a file stream or console,
     * with proper error checking and fallback mechanisms.
     * 
     * @param content Content to write
     * @param filePath Target file path
     * @param fallbackToConsole Whether to use console if file fails
     * @return Reference to the output stream used (file or console)
     */
    static std::ostream& writeToStream(
        const std::string& content,
        const std::string& filePath,
        bool fallbackToConsole = true
    );

    /**
     * @brief Get the last error message from output operations
     * 
     * Returns human-readable error message from the most recent operation
     * that failed. Useful for debugging and user feedback.
     * 
     * @return String containing the last error message, empty if no error
     */
    static const std::string& getLastError();

private:
    /**
     * @brief Set the last error message for error reporting
     * 
     * @param error Error message to store
     */
    static void setLastError(const std::string& error);

    /**
     * @brief Ensure directory exists for output file
     * 
     * Creates the directory structure if it doesn't exist.
     * 
     * @param filePath Full path to the output file
     * @return True if directory exists or was created successfully
     */
    static bool ensureDirectoryExists(const std::string& filePath);

    static thread_local std::string s_lastError;
};