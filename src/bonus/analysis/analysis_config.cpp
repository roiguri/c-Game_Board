#include "bonus/analysis/analysis_config.h"
#include <iostream>
#include <fstream>

AnalysisConfig::AnalysisConfig() {
    setDefaults();
}

void AnalysisConfig::setDefaults() {
    // These match the default values from AnalysisParams struct
    m_params.boardSizes = {10, 20, 30};
    m_params.wallDensities = {0.1f, 0.25f, 0.35f};
    m_params.mineDensities = {0.05f, 0.1f, 0.15f};
    m_params.symmetryType = "horizontal";
    m_params.numSamples = 5;
    m_params.maxSteps = 500;
    m_params.numShells = {20};
    m_params.numTanksPerPlayer = {1, 2, 3};
}

AnalysisParams AnalysisConfig::getParams() const {
    return m_params;
}

bool AnalysisConfig::loadFromFile(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open config file '" << filename 
                      << "'. Using default values." << std::endl;
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        file.close();
        
        // Parse each field, keeping defaults if field is missing
        if (j.contains("boardSizes") && j["boardSizes"].is_array()) {
            m_params.boardSizes = j["boardSizes"].get<std::vector<int>>();
        }
        
        if (j.contains("wallDensities") && j["wallDensities"].is_array()) {
            m_params.wallDensities = j["wallDensities"].get<std::vector<float>>();
        }
        
        if (j.contains("mineDensities") && j["mineDensities"].is_array()) {
            m_params.mineDensities = j["mineDensities"].get<std::vector<float>>();
        }
        
        if (j.contains("symmetryType") && j["symmetryType"].is_string()) {
            m_params.symmetryType = j["symmetryType"].get<std::string>();
        }
        
        if (j.contains("numSamples") && j["numSamples"].is_number_integer()) {
            m_params.numSamples = j["numSamples"].get<int>();
        }
        
        if (j.contains("maxSteps") && j["maxSteps"].is_number_integer()) {
            m_params.maxSteps = j["maxSteps"].get<int>();
        }
        
        if (j.contains("numShells") && j["numShells"].is_array()) {
            m_params.numShells = j["numShells"].get<std::vector<int>>();
        }
        
        if (j.contains("numTanksPerPlayer") && j["numTanksPerPlayer"].is_array()) {
            m_params.numTanksPerPlayer = j["numTanksPerPlayer"].get<std::vector<int>>();
        }
        
        // Validate loaded values
        if (!validateParams()) {
            std::cerr << "Warning: Invalid values in config file. Using defaults for invalid fields." << std::endl;
            return false;
        }
        
        std::cout << "Configuration loaded successfully from: " << filename << std::endl;
        return true;
        
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Warning: JSON parsing error in '" << filename 
                  << "': " << e.what() << ". Using default values." << std::endl;
        setDefaults();  // Reset to defaults on JSON error
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Warning: Error loading config file '" << filename 
                  << "': " << e.what() << ". Using default values." << std::endl;
        setDefaults();  // Reset to defaults on any other error
        return false;
    }
}

bool AnalysisConfig::validateParams() {
    bool valid = true;
    
    // Validate arrays are not empty
    if (m_params.boardSizes.empty() || 
        m_params.wallDensities.empty() || m_params.mineDensities.empty() ||
        m_params.symmetryType.empty() || m_params.maxSteps <= 0 ||
        m_params.numShells.empty() || m_params.numTanksPerPlayer.empty()) {
        std::cerr << "Warning: Empty parameter arrays found." << std::endl;
        valid = false;
    }
    
    // Validate densities are between 0.0 and 1.0
    for (float density : m_params.wallDensities) {
        if (density < 0.0f || density > 1.0f) {
            std::cerr << "Warning: Invalid wall density: " << density << std::endl;
            valid = false;
        }
    }
    
    for (float density : m_params.mineDensities) {
        if (density < 0.0f || density > 1.0f) {
            std::cerr << "Warning: Invalid mine density: " << density << std::endl;
            valid = false;
        }
    }
    
    // Validate numSamples is positive
    if (m_params.numSamples <= 0) {
        std::cerr << "Warning: Invalid numSamples: " << m_params.numSamples << std::endl;
        valid = false;
    }
    
    return valid;
}