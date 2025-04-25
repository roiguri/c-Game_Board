#pragma once

#include "bonus/config/configuration_base.h"

/**
 * @brief Configuration for board generation
 */
class BoardConfig : public ConfigurationBase {
public:
    /**
     * @brief Constructor with default values
     */
    BoardConfig();
    
    // Configuration values
    int width = 15;
    int height = 10;
    float wallDensity = 0.25f;
    float mineDensity = 0.05f;
    std::string symmetry = "none"; // none, horizontal, vertical, diagonal
    int seed = -1; // -1 for random seed
    
protected:
    /**
     * @brief Parse a key-value pair from config file
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @return true if parsing was successful, false otherwise
     */
    bool parseConfigValue(
      const std::string& key, 
      const std::string& value
    ) override;
};