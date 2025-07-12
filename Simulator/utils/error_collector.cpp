#include "error_collector.h"
#include <fstream>
#include <iostream>

ErrorCollector::ErrorCollector() {
    // Empty constructor - m_errors is initialized by default
}

ErrorCollector::~ErrorCollector() {
    // Default destructor - vector cleanup is automatic
}

void ErrorCollector::addMapWarnings(const std::string& mapName, const std::vector<std::string>& warnings) {
    for (const auto& warning : warnings) {
        std::string formattedWarning = formatMapWarning(mapName, warning);
        m_errors.push_back(formattedWarning);
    }
}

bool ErrorCollector::hasErrors() const {
    return !m_errors.empty();
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
}

std::string ErrorCollector::formatMapWarning(const std::string& mapName, const std::string& warning) const {
    return "Map '" + mapName + "': " + warning;
}