#include "bonus/config/configuration_base.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include <cfloat>

bool ConfigurationBase::loadFromFile(const std::string& configPath) {
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        std::cerr << "Could not open config file: " << configPath << std::endl;
        return false;
    }
    
    bool success = true;
    std::string line;
    while (std::getline(configFile, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key-value pairs
        std::size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            std::cerr << "Invalid config line: " << line << std::endl;
            success = false;
            continue;
        }
        
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        if (!parseConfigValue(key, value)) {
            std::cerr << "Invalid value for " << key << ": " << value << std::endl;
            success = false;
        }
    }
    
    return success;
}

bool ConfigurationBase::parseIntValue(
  const std::string& value, 
  int& outValue, 
  int minValue, 
  int maxValue
) {
    int parsedValue;
    if (!(std::istringstream(value) >> parsedValue)) {
        std::cerr << "Failed to parse integer: " << value << std::endl;
        return false;
    }
    
    if (parsedValue < minValue) {
        std::cerr << "Value too small, minimum is " << minValue << std::endl;
        return false;
    }
    
    if (parsedValue > maxValue) {
        std::cerr << "Value too large, maximum is " << maxValue << std::endl;
        return false;
    }
    
    outValue = parsedValue;
    return true;
}

bool ConfigurationBase::parseFloatValue(
  const std::string& value, 
  float& outValue, 
  float minValue, 
  float maxValue
) {
    float parsedValue;
    if (!(std::istringstream(value) >> parsedValue)) {
        std::cerr << "Failed to parse float: " << value << std::endl;
        return false;
    }
    
    if (parsedValue < minValue) {
        std::cerr << "Value too small, minimum is " << minValue << std::endl;
        return false;
    }
    
    if (parsedValue > maxValue) {
        std::cerr << "Value too large, maximum is " << maxValue << std::endl;
        return false;
    }
    
    outValue = parsedValue;
    return true;
}

bool ConfigurationBase::parseStringValue(
  const std::string& value, 
  std::string& outValue, 
  const std::set<std::string>& allowedValues
) {
    if (!allowedValues.empty() && 
        allowedValues.find(value) == allowedValues.end()) {
        std::cerr << "Invalid value: " << value << ". Allowed values: ";
        for (const auto& allowed : allowedValues) {
            std::cerr << allowed << " ";
        }
        std::cerr << std::endl;
        return false;
    }
    
    outValue = value;
    return true;
}