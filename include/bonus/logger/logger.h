// include/bonus/logger/logger.h
#pragma once

#include <string>
#include <fstream>

/**
 * @brief Simple logging system for the Tank Battle game
 * 
 * Always compiled but only activated with --enable-logging flag
 */
class Logger {
public:
    /**
     * @brief Log severity levels
     */
    enum class Level {
        DEBUG,   // Detailed debugging information
        INFO,    // General information about game progress
        WARNING, // Potential issues that don't prevent game operation
        ERROR    // Errors that affect gameplay
    };

    /**
     * @brief Get the singleton instance of the logger
     */
    static Logger& getInstance();

    /**
     * @brief Initialize the logger
     * 
     * @param level Minimum severity level to log
     * @param useConsole Whether to log to console
     * @param useFile Whether to log to file
     * @param filename Log file path (if useFile is true)
     * @return true if initialization was successful
     */
    bool initialize(Level level, bool useConsole, bool useFile, 
                   const std::string& filename = "tankbattle.log");

    /**
     * @brief Set the minimum log level
     */
    void setLevel(Level level);

    /**
     * @brief Enable or disable logging
     * 
     * @param enabled Whether logging should be enabled
     */
    void setEnabled(bool enabled);

    /**
     * @brief Check if logging is enabled
     * 
     * @return true if logging is enabled
     */
    bool isEnabled() const;

    /**
     * @brief Log a message if logging is enabled
     * 
     * @param level Message severity level
     * @param message The message to log
     * @param file Source file (optional)
     * @param line Line number (optional)
     */
    void log(Level level, const std::string& message, const char* file = nullptr, int line = 0);

    // Convenience methods for different log levels
    void debug(const std::string& message, const char* file = nullptr, int line = 0);
    void info(const std::string& message, const char* file = nullptr, int line = 0);
    void warning(const std::string& message, const char* file = nullptr, int line = 0);
    void error(const std::string& message, const char* file = nullptr, int line = 0);

private:
    Logger(); // Private constructor for singleton
    ~Logger(); // Private destructor
    
    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string levelToString(Level level) const;
    std::string getTimestamp() const;
    
    Level m_level;
    bool m_useConsole;
    bool m_useFile;
    std::string m_filename;
    std::ofstream m_fileStream;
    bool m_initialized;
    bool m_enabled;
};

// Convenience macros for logging - these check if logging is enabled at runtime
#define LOG_DEBUG(msg) if(Logger::getInstance().isEnabled()) Logger::getInstance().debug(msg, __FILE__, __LINE__)
#define LOG_INFO(msg) if(Logger::getInstance().isEnabled()) Logger::getInstance().info(msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) if(Logger::getInstance().isEnabled()) Logger::getInstance().warning(msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) if(Logger::getInstance().isEnabled()) Logger::getInstance().error(msg, __FILE__, __LINE__)