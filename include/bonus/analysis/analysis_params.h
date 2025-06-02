#pragma once

#include <vector>
#include <string>

/**
 * @brief Configuration parameters for analysis tool
 * 
 * Defines all the dimensions and values to test during analysis.
 * Each vector represents the different values to test for that dimension.
 */
struct AnalysisParams {
    // Board dimensions
    std::vector<int> widths = {10, 20};
    std::vector<int> heights = {10, 15};
    
    // Board content densities (0.0 to 1.0)
    std::vector<float> wallDensities = {0.1f, 0.25f};
    std::vector<float> mineDensities = {0.05f};
    
    // Board generation options
    std::vector<std::string> symmetryTypes = {"none", "horizontal"};
    
    // Statistical sampling
    int numSamples = 5;  // Number of different random seeds per configuration
    
    // Game rules
    std::vector<int> maxSteps = {500, 1000};
    std::vector<int> numShells = {10};
    std::vector<int> numTanksPerPlayer = {1, 2};
};