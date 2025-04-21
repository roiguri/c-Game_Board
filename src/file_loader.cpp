#include "file_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<std::string> FileLoader::loadBoardFile(
    const std::string& filePath,
    int& width,
    int& height
) {
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return std::vector<std::string>();
    }
    
    std::vector<std::string> lines;
    std::string line;
    
    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }
    inputFile.close();
    
    if (lines.empty()) {
        std::cerr << "Error: File is empty" << std::endl;
        return std::vector<std::string>();
    }
    
    if (!parseDimensions(lines[0], width, height)) {
        std::cerr << "Error: First line must contain valid board dimensions" << std::endl;
        return std::vector<std::string>();
    }
    
    // Check for valid dimensions
    if (width <= 0 || height <= 0) {
        std::cerr << "Error: Invalid board dimensions: " << width << "x" << height << std::endl;
        return std::vector<std::string>();
    }
    lines.erase(lines.begin());
    
    return lines;
}

bool FileLoader::parseDimensions(
    const std::string& line,
    int& width,
    int& height
) {
    std::istringstream iss(line);
    iss >> width >> height;
    return !iss.fail();
}