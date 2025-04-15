#pragma once

#include <string>
#include <vector>

/**
 * @brief Utility class for loading and parsing game files
 * 
 * This class handles file operations for the game, including
 * reading board files and parsing board dimensions.
 */
class FileLoader {
public:
    /**
     * @brief Load a board file and parse its contents
     * 
     * @param filePath Path to the board file to load
     * @param width Reference to store the board width
     * @param height Reference to store the board height
     * @return Vector of strings containing the board content (without the dimensions line)
     *         or an empty vector if loading failed
     */
    static std::vector<std::string> loadBoardFile(
        const std::string& filePath,
        int& width,
        int& height
    );

private:
    /**
     * @brief Parse board dimensions from a string
     * 
     * @param line The string containing dimensions
     * @param width Reference to store the parsed width
     * @param height Reference to store the parsed height
     * @return true if dimensions were successfully parsed, false otherwise
     */
    static bool parseDimensions(
        const std::string& line,
        int& width,
        int& height
    );
};