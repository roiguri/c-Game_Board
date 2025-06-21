#pragma once

/**
 * @file visualization.h
 * @brief Main include file for the visualization system
 * 
 * This file includes all visualization components and provides
 * a simple way to integrate visualization into the game.
 */

#include "UserCommon/bonus/visualization/core/game_snapshot.h"
#include "UserCommon/bonus/visualization/core/visualizer_base.h"
#include "UserCommon/bonus/visualization/core/visualization_manager.h"
#include "UserCommon/bonus/visualization/visualizers/html_visualizer/html_visualizer.h"

// Forward declarations for concrete visualizers
// When implementing these classes, uncomment these lines
// class HTMLVisualizer;
// class TerminalVisualizer;

/**
 * @brief Creates a VisualizationManager with the default visualizers
 * 
 * This helper function creates and configures a visualization manager
 * with the standard set of visualizers.
 * 
 * @param enableLiveVisualization Whether to enable live visualization
 * @return A unique_ptr to a configured VisualizationManager
 */
inline std::unique_ptr<VisualizationManager> createVisualizationManager(bool enableLiveVisualization = false) {
    auto manager = std::make_unique<VisualizationManager>();
    
    manager->addVisualizer(std::make_unique<HTMLVisualizer>());
    
    manager->setLiveVisualizationEnabled(enableLiveVisualization);
    
    return manager;
}