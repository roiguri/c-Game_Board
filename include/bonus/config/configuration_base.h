#pragma once

#include <string>
#include <map>
#include <functional>
#include <climits>
#include <cfloat>
#include <set>

/**
 * @brief Base class for configuration systems
 */
class ConfigurationBase {
public:
    /**
     * @brief Load configuration from file
     * 
     * @param configPath Path to configuration file
     * @return true if configuration was loaded successfully
     */
    bool loadFromFile(const std::string& configPath);
    
protected:
    /**
     * @brief Parse a key-value pair from config file
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @return true if parsing was successful, false otherwise
     */
    virtual bool parseConfigValue(
      const std::string& key, 
      const std::string& value) = 0;
    
    /**
     * @brief Helper to parse and validate integer values
     * 
     * @param value String value to parse
     * @param outValue Reference to store the parsed value
     * @param minValue Minimum allowed value
     * @param maxValue Maximum allowed value
     * @return true if parsing was successful, false otherwise
     */
    bool parseIntValue(
      const std::string& value, 
      int& outValue, int minValue = INT_MIN, 
      int maxValue = INT_MAX
    );
    
    /**
     * @brief Helper to parse and validate float values
     * 
     * @param value String value to parse
     * @param outValue Reference to store the parsed value
     * @param minValue Minimum allowed value
     * @param maxValue Maximum allowed value
     * @return true if parsing was successful, false otherwise
     */
    bool parseFloatValue(
      const std::string& value, 
      float& outValue, 
      float minValue = -FLT_MAX, 
      float maxValue = FLT_MAX);
    
    /**
     * @brief Helper to parse and validate string values against a set of allowed values
     * 
     * @param value String value to parse
     * @param outValue Reference to store the parsed value
     * @param allowedValues Set of allowed values (empty set = allow any string)
     * @return true if parsing was successful, false otherwise
     */
    bool parseStringValue(
      const std::string& value, 
      std::string& outValue,
      const std::set<std::string>& allowedValues = std::set<std::string>());
};