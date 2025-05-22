// src/bonus/logger/logger_config.cpp
#include "bonus/logger/logger_config.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>

// New method signature
bool LoggerConfig::configure(const CliParser& parser) { 
    bool enableLogging = parser.isEnableLogging();
    
    if (!enableLogging) {
        Logger::getInstance().setEnabled(false); // Explicitly disable logger
        return true; // Success - no logging is fine
    }
    
    Logger::Level level = stringToLevel(parser.getLogLevel());
    bool useFile = parser.isLogToFile();
    bool useConsole = !parser.isNoConsoleLog(); // Correctly interpret this
    std::string logFile = parser.getLogFile();

    // If logFile is empty from parser (meaning default was used or not set by parser explicitly to empty)
    // and logToFile is true, ensure we still use the default "tankbattle.log" if parser's default is empty.
    // The CliParser already defaults logFile_ to "tankbattle.log", so this specific check might be redundant
    // if the parser's default for getLogFile() is never truly empty when logToFile is true.
    // However, if logFile is empty due to an explicit empty argument (which CliParser doesn't currently allow for log-file),
    // this would catch it. Given CliParser's current design, logFile will always have a value (its default or parsed).
    if (useFile && logFile.empty()) { // This case should ideally not be hit if parser has a default.
        logFile = "tankbattle.log"; // Ensure a fallback, though parser default should handle.
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