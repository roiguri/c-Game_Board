#pragma once

#include <string>
#include <vector>

/**
 * @brief Utility class for loading and parsing game board files
 * 
 * This class handles file operations for the tank battle game, including
 * reading board files, parsing board dimensions, and validating input format.
 */
class FileLoader {
public:
    /**
     * @brief Load a board file and parse its contents
     * 
     * Reads the file at the given path, parses the first line for dimensions,
     * and returns the remaining lines that contain the actual board data.
     * 
     * @param filePath Path to the board file to load
     * @param width Reference to store the parsed board width
     * @param height Reference to store the parsed board height
     * @return Vector of strings containing the board content (without the
     *  dimensions line) or an empty vector if loading failed due to file
     *  errors or invalid dimensions
     */
    static std::vector<std::string> loadBoardFile(
        const std::string& filePath,
        int& width,
        int& height
    );

private:
    // Parse board dimensions from the first line of the file
    // The expected format is "width height", e.g., "10 20"
    // where width and height are integers
    static bool parseDimensions(
        const std::string& line,
        int& width,
        int& height
    );
};