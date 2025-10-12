#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>

#include "bonus/logger/logger_config.h"

namespace UserCommon_318835816_211314471 {

bool LoggerConfig::configure(const CliParser& parser) { 
    bool enableLogging = parser.isEnableLogging();
    
    if (!enableLogging) {
        if (parser.isLogToFile() || parser.isNoConsoleLog() ||
            parser.isLogLevelSet() || parser.isLogFileSet()) {
            std::cerr << "Warning: Logging-related arguments were provided, but logging is not enabled (missing --enable_logging)." << std::endl;
        }
        Logger::getInstance().setEnabled(false);
        return true;
    }
    
    if (enableLogging && parser.isLogFileSet() && !parser.isLogToFile()) {
        std::cerr << "Warning: --log_file was provided, but --log_to_file is not enabled. The log file will be ignored." << std::endl;
    }
    
    Logger::Level level = stringToLevel(parser.getLogLevel());
    bool useFile = parser.isLogToFile();
    bool useConsole = !parser.isNoConsoleLog(); // Correctly interpret this
    std::string logFile = parser.getLogFile();

    if (useFile && logFile.empty()) {
        logFile = "tankbattle.log";
    }
    
    // Don't allow disabling both console and file logging
    if (!useConsole && !useFile) {
        std::cerr << "Warning: Cannot disable both console and file logging. "
                  << "Enabling console logging." << std::endl;
        useConsole = true;
    }
    
    // Try to initialize the logger
    bool initSuccess = Logger::getInstance().initialize(level, useConsole, useFile, logFile);
    
    // If file logging failed, try to fall back to console only
    if (!initSuccess && useFile) {
        std::cerr << "Failed to open log file: " << logFile 
                  << ". Falling back to console logging." << std::endl;
        initSuccess = Logger::getInstance().initialize(level, true, false);
    }
    
    // Only enable logging if initialization was successful
    if (initSuccess) {
        Logger::getInstance().setEnabled(true);
        return true;
    } else {
        std::cerr << "Warning: Failed to initialize logger. Continuing without logging." << std::endl;
        return false; // Logger initialization failed, but game can still run
    }
}

Logger::Level LoggerConfig::stringToLevel(const std::string& levelStr) {
    // Convert to lowercase
    std::string lowerLevel = levelStr;
    std::transform(lowerLevel.begin(), lowerLevel.end(), lowerLevel.begin(), 
                  [](unsigned char c) { return std::tolower(c); });
    
    if (lowerLevel == "debug") return Logger::Level::DEBUG;
    if (lowerLevel == "info") return Logger::Level::INFO;
    if (lowerLevel == "warning") return Logger::Level::WARNING;
    if (lowerLevel == "error") return Logger::Level::ERROR;
    
    // Default to INFO if not recognized
    return Logger::Level::INFO;
}

} // namespace UserCommon_318835816_211314471