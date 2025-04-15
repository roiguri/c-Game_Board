#include "bonus/visualization/core/visualization_manager.h"
#include <iostream>

VisualizationManager::VisualizationManager()
    : m_liveVisualizationEnabled(false) {
}

VisualizationManager::~VisualizationManager() {
    // Smart pointers will handle cleanup of visualizers
}

void VisualizationManager::addVisualizer(std::unique_ptr<VisualizerBase> visualizer) {
    if (visualizer) {
        m_visualizers.push_back(std::move(visualizer));
    }
}

void VisualizationManager::captureGameState(
    int step,
    const GameBoard& board,
    const std::vector<Tank>& tanks,
    const std::vector<Shell>& shells,
    const std::string& message
) {
    // Create a snapshot of the current state
    GameSnapshot snapshot(step, board, tanks, shells, message);
    
    // Process the snapshot in all visualizers
    for (auto& visualizer : m_visualizers) {
        visualizer->processSnapshot(snapshot);
    }
}

bool VisualizationManager::generateOutputs(const std::string& basePath) {
    bool allSucceeded = true;
    for (size_t i = 0; i < m_visualizers.size(); ++i) {
        // Create a unique output path for each visualizer
        std::string outputPath = basePath;
        if (m_visualizers.size() > 1) {
            outputPath += "_" + std::to_string(i + 1);
        }
        
        // Generate output and track success
        if (!m_visualizers[i]->generateOutput(outputPath)) {
            std::cerr << "Failed to generate output for visualizer " << i << std::endl;
            allSucceeded = false;
        }
    }
    
    return allSucceeded;
}

void VisualizationManager::setLiveVisualizationEnabled(bool enabled) {
    m_liveVisualizationEnabled = enabled;
    
    // If enabling live visualization, check if any visualizers support it
    if (enabled) {
        bool anySupportsLive = false;
        for (const auto& visualizer : m_visualizers) {
            if (visualizer->supportsLiveMode()) {
                anySupportsLive = true;
                break;
            }
        }
        
        if (!anySupportsLive) {
            std::cerr << "Warning: Live visualization enabled, but no visualizers support live mode" << std::endl;
        }
    }
}

bool VisualizationManager::isLiveVisualizationEnabled() const {
    return m_liveVisualizationEnabled;
}

void VisualizationManager::displayCurrentState() {
    if (!m_liveVisualizationEnabled) {
        return;
    }
    
    for (auto& visualizer : m_visualizers) {
        if (visualizer->supportsLiveMode()) {
            visualizer->displayCurrentState();
        }
    }
}

size_t VisualizationManager::getVisualizerCount() const {
    return m_visualizers.size();
}