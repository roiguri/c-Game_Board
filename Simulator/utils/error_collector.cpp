#include "error_collector.h"
#include <fstream>
#include <iostream>

ErrorCollector::ErrorCollector() : m_errorCount(0), m_gameManagerErrorCount(0), m_algorithmErrorCount(0) {
    // Initialize error counts to zero, m_errors vector is initialized by default
}

ErrorCollector::~ErrorCollector() {
    // Default destructor - vector cleanup is automatic
}

void ErrorCollector::addMapError(const std::string& mapName, const std::string& error) {
    std::string formattedError = formatMapError(mapName, error);
    m_errors.push_back(formattedError);
    m_errorCount++;
}

void ErrorCollector::addMapWarning(const std::string& mapName, const std::string& warning) {
    std::string formattedWarning = formatMapWarning(mapName, warning);
    m_errors.push_back(formattedWarning);
}

void ErrorCollector::addMapWarnings(const std::string& mapName, const std::vector<std::string>& warnings) {
    for (const auto& warning : warnings) {
        addMapWarning(mapName, warning);
    }
}

bool ErrorCollector::hasErrors() const {
    return !m_errors.empty();
}

bool ErrorCollector::hasMapErrors() const {
    return m_errorCount > 0;
}

bool ErrorCollector::hasMapWarnings() const {
    return m_errors.size() > m_errorCount;
}

size_t ErrorCollector::getErrorCount() const {
    return m_errorCount;
}

const std::vector<std::string>& ErrorCollector::getAllErrors() const {
    return m_errors;
}

bool ErrorCollector::saveToFile() const {
    if (m_errors.empty()) {
        // No errors to report
        return true;
    }
    
    std::ofstream errorFile("input_errors.txt");
    if (!errorFile.is_open()) {
        std::cerr << "Warning: Could not create input_errors.txt file" << std::endl;
        return false;
    }
    
    for (const auto& error : m_errors) {
        errorFile << error << std::endl;
    }
    
    errorFile.close();
    return true;
}

void ErrorCollector::clear() {
    m_errors.clear();
    m_errorCount = 0;
    m_gameManagerErrorCount = 0;
    m_algorithmErrorCount = 0;
}

std::string ErrorCollector::formatMapError(const std::string& mapName, const std::string& error) const {
    return "[Error] Map '" + mapName + "': " + error;
}

std::string ErrorCollector::formatMapWarning(const std::string& mapName, const std::string& warning) const {
    return "[Warning] Map '" + mapName + "': " + warning;
}

void ErrorCollector::addGameManagerError(const std::string& gameManagerPath, const std::string& error) {
    std::string formattedError = formatGameManagerError(gameManagerPath, error);
    m_errors.push_back(formattedError);
    m_gameManagerErrorCount++;
}

void ErrorCollector::addAlgorithmError(const std::string& algorithmPath, const std::string& error) {
    std::string formattedError = formatAlgorithmError(algorithmPath, error);
    m_errors.push_back(formattedError);
    m_algorithmErrorCount++;
}

size_t ErrorCollector::getGameManagerErrorCount() const {
    return m_gameManagerErrorCount;
}

size_t ErrorCollector::getAlgorithmErrorCount() const {
    return m_algorithmErrorCount;
}

std::string ErrorCollector::formatGameManagerError(const std::string& gameManagerPath, const std::string& error) const {
    return "[Error] GameManager '" + extractFilename(gameManagerPath) + "': " + error;
}

std::string ErrorCollector::formatAlgorithmError(const std::string& algorithmPath, const std::string& error) const {
    return "[Error] Algorithm '" + extractFilename(algorithmPath) + "': " + error;
}

std::string ErrorCollector::extractFilename(const std::string& filePath) const {
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return filePath.substr(lastSlash + 1);
    }
    return filePath;
}