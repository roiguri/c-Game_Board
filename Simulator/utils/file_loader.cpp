#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "file_loader.h"
#include "file_satellite_view.h"

std::vector<std::string> FileLoader::loadBoardFile(
    const std::string& filePath,
    int& rows,
    int& cols,
    int& maxSteps,
    int& numShells
) {
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file '" << filePath << "'" << std::endl;
        return {};
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }
    inputFile.close();

    if (lines.size() < 5) {
        std::cerr << "Error: File must have at least 5 header lines" << std::endl;
        return {};
    }

    // 1. Map name/description (can be ignored)
    // 2. MaxSteps = <NUM>
    if (!parseKeyValue(lines[1], "MaxSteps", maxSteps)) {
        std::cerr << "Error: Invalid or missing MaxSteps line: '" << lines[1] << "'" << std::endl;
        return {};
    }
    // 3. NumShells = <NUM>
    if (!parseKeyValue(lines[2], "NumShells", numShells)) {
        std::cerr << "Error: Invalid or missing NumShells line: '" << lines[2] << "'" << std::endl;
        return {};
    }
    // 4. Rows = <NUM>
    if (!parseKeyValue(lines[3], "Rows", rows) || rows <= 0) {
        std::cerr << "Error: Invalid or missing Rows line: '" << lines[3] << "'" << std::endl;
        return {};
    }
    // 5. Cols = <NUM>
    if (!parseKeyValue(lines[4], "Cols", cols) || cols <= 0) {
        std::cerr << "Error: Invalid or missing Cols line: '" << lines[4] << "'" << std::endl;
        return {};
    }

    // Remove the first 5 lines (headers) and return the rest as the board
    return std::vector<std::string>(lines.begin() + 5, lines.end());
}

bool FileLoader::parseKeyValue(const std::string& line, const std::string& key, int& value) {
    // Accepts lines like: Key = Value (spaces around '=' allowed)
    std::string::size_type pos = line.find('=');
    if (pos == std::string::npos) return false;
    std::string left = line.substr(0, pos);
    std::string right = line.substr(pos + 1);
    // Trim spaces
    left.erase(left.find_last_not_of(" \t") + 1);
    left.erase(0, left.find_first_not_of(" \t"));
    right.erase(right.find_last_not_of(" \t") + 1);
    right.erase(0, right.find_first_not_of(" \t"));
    if (left != key) return false;
    std::istringstream iss(right);
    iss >> value;
    return !iss.fail();
}

FileLoader::BoardInfo FileLoader::loadBoardWithSatelliteView(const std::string& filePath) {
    BoardInfo info{};
    
    std::vector<std::string> boardData = loadBoardFile(filePath, info.rows, info.cols, info.maxSteps, info.numShells);
    
    if (!boardData.empty()) {
        info.satelliteView = std::make_unique<FileSatelliteView>(boardData, info.rows, info.cols);
    }
    
    return info;
}