// include/bonus/logger/logger_config.h
#pragma once

#include "bonus/logger/logger.h"
#include "bonus/cli/cli_parser.h" // Add this
#include <string>

/**
 * @brief Simple logger configuration from command line
 */
class LoggerConfig {
public:
    /**
     * @brief Configure logger from command line arguments
     * 
     * Checks for --enable-logging flag and configures logger if found
     * 
     * @param parser The CliParser instance with parsed arguments
     * @return true if configuration was successful
     */
    static bool configure(const CliParser& parser); // New
    
private:
    // Convert string to log level
    static Logger::Level stringToLevel(const std::string& levelStr);
};