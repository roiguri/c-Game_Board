#include "error_collector.h"
#include <fstream>
#include <iostream>

ErrorCollector::ErrorCollector() : m_errorCount(0) {
    // Initialize error count to zero, m_errors vector is initialized by default
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
        std::cerr << "Error: Could not create input_errors.txt file" << std::endl;
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
}

std::string ErrorCollector::formatMapError(const std::string& mapName, const std::string& error) const {
    return "[Error] Map '" + mapName + "': " + error;
}

std::string ErrorCollector::formatMapWarning(const std::string& mapName, const std::string& warning) const {
    return "[Warning] Map '" + mapName + "': " + warning;
}