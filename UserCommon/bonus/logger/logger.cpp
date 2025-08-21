#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>

#include "bonus/logger/logger.h"

namespace UserCommon_318835816_211314471 {

Logger::Logger()
    : m_level(Level::INFO),
      m_useConsole(true),
      m_useFile(false),
      m_initialized(false),
      m_enabled(false) {
}

Logger::~Logger() {
    if (m_fileStream.is_open()) {
        m_fileStream.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

bool Logger::initialize(Level level, bool useConsole, bool useFile, const std::string& filename) {
    m_level = level;
    m_useConsole = useConsole;
    m_useFile = useFile;
    
    // Close any existing file stream
    if (m_fileStream.is_open()) {
        m_fileStream.close();
    }
    
    // If logging to file, open the file stream
    if (useFile) {
        m_filename = filename;
        m_fileStream.open(m_filename, std::ios::out | std::ios::app);
        
        if (!m_fileStream.is_open()) {
            std::cerr << "Failed to open log file: " << m_filename << std::endl;
            m_useFile = false;
            return false;
        }
    }
    
    m_initialized = true;
    
    // Log initialization message
    if (m_enabled) {
        std::string levelStr = levelToString(m_level);
        log(Level::INFO, "Logger initialized with level " + levelStr);
    }
    
    return true;
}

void Logger::setLevel(Level level) {
    m_level = level;
    
    if (m_enabled && m_initialized) {
        log(Level::INFO, "Log level changed to " + levelToString(level));
    }
}

void Logger::setEnabled(bool enabled) {
    bool wasEnabled = m_enabled;
    m_enabled = enabled;
    
    if (m_initialized) {
        if (enabled && !wasEnabled) {
            // Log when we're turning on logging
            log(Level::INFO, "Logging enabled");
        } else if (!enabled && wasEnabled) {
            // Log that we're turning off logging (will be the last message)
            log(Level::INFO, "Logging disabled");
        }
    }
}

bool Logger::isEnabled() const {
    return m_enabled;
}

void Logger::log(Level level, const std::string& message, const char* file, int line) {
    // Skip logging if disabled or below minimum level
    if (!m_enabled || level < m_level) {
        return;
    }
    
    // Format the log message
    std::stringstream logStream;
    logStream << getTimestamp() << " [" << levelToString(level) << "] ";
    
    // Add file and line information if provided
    if (file != nullptr) {
        std::filesystem::path path(file);
        logStream << path.filename().string();
        if (line > 0) {
            logStream << ":" << line;
        }
        logStream << " - ";
    }
    
    logStream << message;
    std::string formattedMessage = logStream.str();
    
    // Output to console if enabled
    if (m_useConsole) {
        if (level == Level::ERROR) {
            std::cerr << formattedMessage << std::endl;
        } else {
            std::cout << formattedMessage << std::endl;
        }
    }
    
    // Output to file if enabled and open
    if (m_useFile && m_fileStream.is_open()) {
        m_fileStream << formattedMessage << std::endl;
        
        // Auto-flush on error
        if (level == Level::ERROR) {
            m_fileStream.flush();
        }
    }
}

void Logger::debug(const std::string& message, const char* file, int line) {
    log(Level::DEBUG, message, file, line);
}

void Logger::info(const std::string& message, const char* file, int line) {
    log(Level::INFO, message, file, line);
}

void Logger::warning(const std::string& message, const char* file, int line) {
    log(Level::WARNING, message, file, line);
}

void Logger::error(const std::string& message, const char* file, int line) {
    log(Level::ERROR, message, file, line);
}

std::string Logger::levelToString(Level level) const {
    switch (level) {
        case Level::DEBUG:   return "DEBUG";
        case Level::INFO:    return "INFO";
        case Level::WARNING: return "WARNING";
        case Level::ERROR:   return "ERROR";
        default:             return "UNKNOWN";
    }
}

std::string Logger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    struct tm timeInfo;
    
#ifdef _WIN32
    localtime_s(&timeInfo, &time);
#else
    localtime_r(&time, &timeInfo);
#endif
    
    std::stringstream ss;
    ss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
    
    return ss.str();
}

bool Logger::isInitializedAndEnabled() const {
    return m_initialized && m_enabled;
}

} // namespace UserCommon_318835816_211314471