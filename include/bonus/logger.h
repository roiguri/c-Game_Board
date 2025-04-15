#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <iostream>

// TODO: consider adding flags for changing log levels and set default.

/**
 * @brief Logger class for centralized logging throughout the application
 * 
 * This class implements the Singleton pattern to ensure a single logging instance.
 * It supports multiple log levels and can output to both file and console.
 */
class Logger {
public:
    /**
     * @brief Log severity levels
     */
    enum class LogLevel { 
        DEBUG,  ///< Detailed information for debugging
        INFO,   ///< General information about program execution
        WARNING, ///< Potential issues that don't prevent execution
        ERROR   ///< Serious problems that affect program functionality
    };
    
    /**
     * @brief Get the singleton Logger instance
     * 
     * @return Reference to the Logger singleton
     */
    static Logger& getInstance();
    
    /**
     * @brief Set the minimum severity level for logging
     * 
     * Messages with severity below this level will be ignored
     * 
     * @param level The minimum log level to output
     */
    void setLogLevel(LogLevel level);
    
    /**
     * @brief Set the output file for logging
     * 
     * @param filePath Path to the log file
     * @return true if file was successfully opened, false otherwise
     */
    bool setLogFile(const std::string& filePath);
    
    /**
     * @brief Enable or disable console output
     * 
     * @param enable True to enable console output, false to disable
     */
    void enableConsoleOutput(bool enable);
    
    /**
     * @brief Log a debug message
     * 
     * @param message The message to log
     */
    void debug(const std::string& message);
    
    /**
     * @brief Log an info message
     * 
     * @param message The message to log
     */
    void info(const std::string& message);
    
    /**
     * @brief Log a warning message
     * 
     * @param message The message to log
     */
    void warning(const std::string& message);
    
    /**
     * @brief Log an error message
     * 
     * @param message The message to log
     */
    void error(const std::string& message);
    
    /**
     * @brief Log a message with specified severity
     * 
     * @param level The severity level of the message
     * @param message The message to log
     */
    void log(LogLevel level, const std::string& message);
    
    /**
     * @brief Check if a particular log level is currently enabled
     * 
     * @param level The log level to check
     * @return true if the level is enabled, false otherwise
     */
    bool isLevelEnabled(LogLevel level) const;
    
    /**
     * @brief Close the log file if open
     */
    void closeLogFile();

private:
    // Private constructor and destructor for singleton
    Logger();
    ~Logger();
    
    // Prevent copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    LogLevel m_currentLogLevel;
    std::string m_logFilePath;
    std::ofstream m_logFile;
    bool m_consoleOutput;
    std::mutex m_mutex; // For thread safety
    
    // Helper methods
    std::string getTimestamp() const;
    std::string logLevelToString(LogLevel level) const;
    void writeToOutputs(const std::string& formattedMessage, LogLevel level);
};