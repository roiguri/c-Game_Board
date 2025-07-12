#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "file_loader.h"
#include "file_satellite_view.h"

// TODO: move errors to error collector
std::vector<std::string> FileLoader::loadBoardFile(
    const std::string& filePath,
    size_t& rows,
    size_t& cols,
    size_t& maxSteps,
    size_t& numShells,
    std::string& mapName
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

    // 1. Extract map name from first line and clean it
    mapName = lines[0];
    // Replace spaces with underscores
    std::replace(mapName.begin(), mapName.end(), ' ', '_');
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
    if (!parseKeyValue(lines[3], "Rows", rows) || rows == 0) {
        std::cerr << "Error: Invalid or missing Rows line: '" << lines[3] << "'" << std::endl;
        return {};
    }
    // 5. Cols = <NUM>
    if (!parseKeyValue(lines[4], "Cols", cols) || cols == 0) {
        std::cerr << "Error: Invalid or missing Cols line: '" << lines[4] << "'" << std::endl;
        return {};
    }

    // Remove the first 5 lines (headers) and return the rest as the board
    return std::vector<std::string>(lines.begin() + 5, lines.end());
}

bool FileLoader::parseKeyValue(const std::string& line, const std::string& key, size_t& value) {
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
    
    // Parse as long long first to check for negative values
    std::istringstream iss(right);
    long long temp;
    iss >> temp;
    if (iss.fail() || temp < 0) {
        return false;  // Invalid format or negative value
    }
    value = static_cast<size_t>(temp);
    return true;
}

FileLoader::BoardInfo FileLoader::loadBoardWithSatelliteView(const std::string& filePath) {
    BoardInfo info{};
    
    std::vector<std::string> boardData = loadBoardFile(filePath, info.rows, info.cols, info.maxSteps, info.numShells, info.mapName);
    
    if (!boardData.empty()) {
        info.satelliteView = std::make_unique<FileSatelliteView>(boardData, info.rows, info.cols);
    }
    
    return info;
}

// BoardInfo validation interface implementations
bool FileLoader::BoardInfo::isValid() const {
    if (!satelliteView) {
        return false;  // No satellite view means failed to load
    }
    
    // Cast to FileSatelliteView to access validation methods
    FileSatelliteView* fileSatelliteView = static_cast<FileSatelliteView*>(satelliteView.get());
    return fileSatelliteView->isValid();
}

std::string FileLoader::BoardInfo::getErrorReason() const {
    if (!satelliteView) {
        return "Failed to load board file";  // Fallback error message
    }
    
    // Cast to FileSatelliteView to access validation methods
    FileSatelliteView* fileSatelliteView = static_cast<FileSatelliteView*>(satelliteView.get());
    return fileSatelliteView->getErrorReason();
}

std::vector<std::string> FileLoader::BoardInfo::getWarnings() const {
    if (!satelliteView) {
        return {};  // No warnings if no satellite view
    }
    
    // Cast to FileSatelliteView to access validation methods
    FileSatelliteView* fileSatelliteView = static_cast<FileSatelliteView*>(satelliteView.get());
    return fileSatelliteView->getWarnings();
}