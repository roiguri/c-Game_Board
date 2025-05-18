// src/bonus/logger/logger_config.cpp
#include "bonus/logger/logger_config.h"
#include <string>
#include <algorithm>
#include <cctype>

bool LoggerConfig::configureFromCommandLine(int argc, char* argv[]) {
    // Default settings
    Logger::Level level = Logger::Level::INFO;
    bool useConsole = true;
    bool useFile = false;
    std::string logFile = "tankbattle.log";
    bool enableLogging = false;
    
    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--enable-logging") {
            enableLogging = true;
        }
        else if (arg.substr(0, 12) == "--log-level=") {
            std::string levelStr = arg.substr(12);
            level = stringToLevel(levelStr);
        } 
        else if (arg == "--log-to-file") {
            useFile = true;
        }
        else if (arg == "--no-console-log") {
            useConsole = false;
        }
        else if (arg.substr(0, 11) == "--log-file=") {
            logFile = arg.substr(11);
            useFile = true;
        }
    }
    
    // Initialize the logger with the configured settings
    bool success = Logger::getInstance().initialize(level, useConsole, useFile, logFile);
    
    // Enable logging if requested
    if (success && enableLogging) {
        Logger::getInstance().setEnabled(true);
    }
    
    return success;
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