#pragma once

#include <string>

#include "bonus/cli/cli_parser.h"
#include "bonus/logger/logger.h"

namespace UserCommon_098765432_123456789 {

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

} // namespace UserCommon_098765432_123456789