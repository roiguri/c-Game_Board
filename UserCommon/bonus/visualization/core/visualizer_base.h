#pragma once

#include <string>
#include <vector>
#include "bonus/visualization/core/game_snapshot.h"

/**
 * @brief Base interface for all visualizers
 * 
 * This abstract class defines the interface that all visualizers must implement.
 * It provides methods for processing game snapshots, generating output,
 * and supporting live visualization.
 */
class VisualizerBase {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~VisualizerBase() = default;
    
    /**
     * @brief Process a new game state snapshot
     * 
     * @param snapshot The game state to process
     */
    virtual void processSnapshot(const GameSnapshot& snapshot) = 0;
    
    /**
     * @brief Clear all stored snapshots
     */
    virtual void clear() = 0;
    
    /**
     * @brief Generate final output
     * 
     * @param outputPath Base path for output files
     * @return true if output was successfully generated, false otherwise
     */
    virtual bool generateOutput(const std::string& outputPath) = 0;
    
    /**
     * @brief Check if visualizer supports live mode
     * 
     * @return true if live visualization is supported, false otherwise
     */
    virtual bool supportsLiveMode() const = 0;
    
    /**
     * @brief Display current state for live visualization
     * 
     * If the visualizer doesn't support live mode, this method should do nothing.
     */
    virtual void displayCurrentState() = 0;
};