#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common/SatelliteView.h"

class FileSatelliteView;

/**
 * @brief Utility class for loading and parsing game board files
 * 
 * This class handles file operations for the tank battle game, including
 * reading board files, parsing board dimensions, and validating input format.
 * It supports the strict format required by the assignment.
 */
class FileLoader {
public:
    /**
     * @brief Structure containing all board information loaded from file
     */
    struct BoardInfo {
        int rows;
        int cols; 
        int maxSteps;
        int numShells;
        std::string mapName;
        std::unique_ptr<SatelliteView> satelliteView;
    };
    
    /**
     * @brief Load a board file and return BoardInfo with SatelliteView
     * 
     * Reads the file at the given path, parses the header lines for game parameters,
     * and returns a BoardInfo struct containing all parsed data including a SatelliteView.
     * 
     * @param filePath Path to the board file to load
     * @return BoardInfo struct with all parsed data and SatelliteView,
     *         or a BoardInfo with null satelliteView if loading failed
     */
    static BoardInfo loadBoardWithSatelliteView(const std::string& filePath);

    /**
     * @brief Load a board file and parse its contents according to assignment format
     * 
     * Reads the file at the given path, parses the header lines for game parameters,
     * and returns the board data as a vector of strings (each string is a row).
     * 
     * @param filePath Path to the board file to load
     * @param rows Reference to store the parsed number of rows
     * @param cols Reference to store the parsed number of columns
     * @param maxSteps Reference to store the parsed max steps
     * @param numShells Reference to store the parsed number of shells per tank
     * @return Vector of strings containing the board content (each string is a row),
     *         or an empty vector if loading failed due to file errors or invalid format
     */
    static std::vector<std::string> loadBoardFile(
        const std::string& filePath,
        int& rows,
        int& cols,
        int& maxSteps,
        int& numShells,
        std::string& mapName
    );

private:
    // Parse a line of the form "Key = Value" (spaces around '=' allowed)
    static bool parseKeyValue(const std::string& line, const std::string& key, int& value);
};