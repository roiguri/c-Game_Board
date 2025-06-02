#pragma once

#include "bonus/analysis/analysis_params.h"
#include <string>
#include "nlohmann/json.hpp"

/**
 * @brief Configuration manager for the analysis tool
 * 
 * Handles loading analysis parameters from JSON files and provides
 * default values when no configuration is specified.
 */
class AnalysisConfig {
public:
    /**
     * @brief Constructor that sets default values
     */
    AnalysisConfig();
    
    /**
     * @brief Load configuration from JSON file
     * 
     * @param filename Path to JSON configuration file
     * @return true if file was loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filename);
    
    /**
     * @brief Get the current analysis parameters
     * 
     * @return AnalysisParams structure with current configuration
     */
    AnalysisParams getParams() const;

private:
    AnalysisParams m_params;
    
    /**
     * @brief Set default parameter values
     * 
     * Called by constructor to initialize with sensible defaults
     */
    void setDefaults();

    /**
     * @brief Validate loaded parameters
     * 
     * @return true if all parameters are valid, false otherwise
     */
    bool validateParams();
};