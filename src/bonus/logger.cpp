#include "bonus/logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>

// Initialize the singleton instance
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

// Constructor - initialize with default values
Logger::Logger() 
    : m_currentLogLevel(LogLevel::INFO),
      m_logFilePath(""),
      m_consoleOutput(true) {
}

// Destructor - ensure log file is closed
Logger::~Logger() {
    closeLogFile();
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentLogLevel = level;
}

bool Logger::setLogFile(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Close existing file if open
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
    
    m_logFilePath = filePath;
    m_logFile.open(filePath, std::ios::out | std::ios::app);
    
    if (!m_logFile.is_open()) {
        std::cerr << "Failed to open log file: " << filePath << std::endl;
        return false;
    }
    
    return true;
}

void Logger::enableConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_consoleOutput = enable;
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    if (!isLevelEnabled(level)) {
        return;
    }
    
    std::string timestamp = getTimestamp();
    std::string levelStr = logLevelToString(level);
    
    // Format: [TIMESTAMP] [LEVEL] Message
    std::string formattedMessage = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    writeToOutputs(formattedMessage, level);
}

bool Logger::isLevelEnabled(LogLevel level) const {
    return level >= m_currentLogLevel;
}

void Logger::closeLogFile() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
}

std::string Logger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << now_ms.count();
    
    return ss.str();
}

std::string Logger::logLevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

void Logger::writeToOutputs(const std::string& formattedMessage, LogLevel level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Write to file if enabled
    if (m_logFile.is_open()) {
        m_logFile << formattedMessage << std::endl;
        m_logFile.flush();
    }
    
    // Write to console if enabled
    if (m_consoleOutput) {
        // Choose output stream based on severity
        if (level >= LogLevel::WARNING) {
            std::cerr << formattedMessage << std::endl;
        } else {
            std::cout << formattedMessage << std::endl;
        }
    }
}