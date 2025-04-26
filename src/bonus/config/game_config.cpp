// src/bonus/config/game_config.cpp
#include "bonus/config/game_config.h"
#include <iostream>

GameConfig::GameConfig() {
    // Default values already set in the class definition
}

bool GameConfig::parseConfigValue(
  const std::string& key, 
  const std::string& value
) {
    if (key == "initial_shells") {
        return parseIntValue(value, initialShells, 1, 100);
    }
    else if (key == "shoot_cooldown") {
        return parseIntValue(value, shootCooldown, 0, 20);
    }
    else if (key == "wall_health") {
        return parseIntValue(value, wallHealth, 1, 10);
    }
    else if (key == "shells_depleted_countdown") {
        return parseIntValue(value, shellsDepletedCountdown, 0, 1000);
    }
    else if (key == "max_game_steps") {
        return parseIntValue(value, maxGameSteps, 1, 10000);
    } else {
        std::cerr << "Unknown game configuration key: " << key << std::endl;
        return false;
    }
}