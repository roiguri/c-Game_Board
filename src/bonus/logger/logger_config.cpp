// src/bonus/logger/logger_config.cpp
#include "bonus/logger/logger_config.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>

bool LoggerConfig::configureFromCommandLine(int argc, char* argv[]) {
    // Check if logging is even requested
    bool enableLogging = false;
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--enable-logging") {
            enableLogging = true;
            break;
        }
    }
    
    // If logging is not requested, don't even try to configure it
    if (!enableLogging) {
        Logger::getInstance().setEnabled(false); // Explicitly disable logger
        return true; // Success - no logging is fine
    }
    
    // Default settings
    Logger::Level level = Logger::Level::INFO;
    bool useConsole = true;
    bool useFile = false;
    std::string logFile = "tankbattle.log";
    
    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--enable-logging") {
            // Already processed
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