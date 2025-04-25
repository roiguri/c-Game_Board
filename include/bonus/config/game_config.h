#pragma once

#include "bonus/config/configuration_base.h"

/**
 * @brief Configuration for game parameters
 */
class GameConfig : public ConfigurationBase {
public:
    /**
     * @brief Constructor with default values
     */
    GameConfig();
    
    // Tank properties
    int initialShells = 16;
    int shootCooldown = 4;
    int backwardDelay = 2;
    
    // Wall properties
    int wallHealth = 2;
    
    // Game rules
    int shellsDepletedCountdown = 40;
    int maxGameSteps = 1000;
    
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