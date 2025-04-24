#pragma once

#include <memory>
#include <vector>
#include <string>
#include "bonus/visualization/core/visualizer_base.h"
#include "game_board.h"
#include "tank.h"
#include "objects/shell.h"

/**
 * @brief Manager class that coordinates multiple visualizers
 * 
 * This class manages a collection of visualizers and orchestrates
 * the capture and visualization of game states.
 */
class VisualizationManager {
public:
    /**
     * @brief Constructor
     */
    VisualizationManager();
    
    /**
     * @brief Destructor
     */
    ~VisualizationManager();
    
    /**
     * @brief Add a visualizer to the manager
     * 
     * The manager takes ownership of the visualizer.
     * 
     * @param visualizer The visualizer to add
     */
    void addVisualizer(std::unique_ptr<VisualizerBase> visualizer);
    
    /**
     * @brief Process a game state
     * 
     * Creates a snapshot from the current game state and
     * passes it to all registered visualizers.
     * 
     * @param step Current step number
     * @param board Current board state
     * @param tanks Current tank states
     * @param shells Current shell states
     * @param countdown The countdown timer value (if active, -1 otherwise)
     * @param message Optional message describing the step
     */
    void captureGameState(
        int step,
        const GameBoard& board,
        const std::vector<Tank>& tanks,
        const std::vector<Shell>& shells,
        int countdown = -1,
        const std::string& message = ""
    );
    
    /**
     * @brief Generate outputs from all visualizers
     * 
     * @param basePath Base path for output files
     * @return true if all outputs were generated successfully, false otherwise
     */
    bool generateOutputs(const std::string& basePath);
    
    /**
     * @brief Enable or disable live visualization
     * 
     * @param enabled Whether live visualization should be enabled
     */
    void setLiveVisualizationEnabled(bool enabled);
    
    /**
     * @brief Check if live visualization is enabled
     * 
     * @return true if live visualization is enabled, false otherwise
     */
    bool isLiveVisualizationEnabled() const;
    
    /**
     * @brief Display current state for live visualization
     * 
     * This is called when live visualization is enabled to
     * update the display after each game step.
     */
    void displayCurrentState();
    
    /**
     * @brief Get the number of registered visualizers
     * 
     * @return The number of visualizers
     */
    size_t getVisualizerCount() const;
    
private:
    std::vector<std::unique_ptr<VisualizerBase>> m_visualizers;
    bool m_liveVisualizationEnabled;
};