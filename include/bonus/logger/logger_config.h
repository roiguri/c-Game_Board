// include/bonus/logger/logger_config.h
#pragma once

#include "bonus/logger/logger.h"
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
     * @param argc Argument count
     * @param argv Argument values
     * @return true if configuration was successful
     */
    static bool configureFromCommandLine(int argc, char* argv[]);
    
private:
    // Convert string to log level
    static Logger::Level stringToLevel(const std::string& levelStr);
};