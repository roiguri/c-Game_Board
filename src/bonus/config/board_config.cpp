#include "bonus/config/board_config.h"
#include <set>
#include <sstream>
#include <iostream>

BoardConfig::BoardConfig() {
    // Default values already set in the class definition
}

bool BoardConfig::parseConfigValue(const std::string& key, const std::string& value) {
    if (key == "dimensions") {
        std::istringstream iss(value);
        int width, height;
        if (!(iss >> width >> height)) {
            std::cerr << "Invalid dimensions format: " << value << std::endl;
            return false;
        }
        
        if (!parseIntValue(std::to_string(width), this->width, 5, 100)) {
            return false;
        }
        
        if (!parseIntValue(std::to_string(height), this->height, 5, 100)) {
            return false;
        }
        
        return true;
    } 
    else if (key == "wall_density") {
        return parseFloatValue(value, wallDensity, 0.0f, 0.9f);
    } 
    else if (key == "mine_density") {
        return parseFloatValue(value, mineDensity, 0.0f, 0.5f);
    } 
    else if (key == "symmetry") {
        std::set<std::string> allowedSymmetry = 
          {"none", "horizontal", "vertical", "diagonal"};
        return parseStringValue(value, symmetry, allowedSymmetry);
    } 
    else if (key == "seed") {
        return parseIntValue(value, seed);
    }
    else {
        std::cerr << "Unknown configuration key: " << key << std::endl;
        return false;
    }
}