#pragma once

#include <string>
#include <vector>

/**
 * @brief Configuration parameters for analysis tool
 * 
 * Defines all the dimensions and values to test during analysis.
 * Each vector represents the different values to test for that dimension.
 */
struct AnalysisParams {
    // Board dimensions
    std::vector<int> boardSizes;
    
    // Board content densities (0.0 to 1.0)
    std::vector<float> wallDensities;
    std::vector<float> mineDensities;
        
    // Statistical sampling
    int numSamples;  // Number of different random seeds per configuration
    
    // Game rules
    std::vector<int> numShells;
    std::vector<int> numTanksPerPlayer;
    
    int maxSteps;
    std::string symmetryType;
};