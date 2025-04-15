# Visualization System Integration Guide

This manual explains how to integrate the visualization system with the Tank Battle game manager.

## Overview

The visualization system allows you to visualize the game state in various formats, including HTML and terminal output. It's designed to be modular, extensible, and can be enabled or disabled via compile-time options.

## Directory Structure

```
include/bonus/visualization/
├── game_snapshot.h        # Game state snapshot data structure
├── visualization.h        # Main include file with helper functions
├── visualization_manager.h # Manager for multiple visualizers
├── visualizer_base.h      # Abstract interface for visualizers
└── ...                    # Concrete visualizer implementations

src/bonus/visualization/
├── game_snapshot.cpp      # GameSnapshot implementation
├── visualization_manager.cpp # VisualizationManager implementation
└── ...                    # Concrete visualizer implementations

test/bonus/visualization/
├── game_snapshot_test.cpp # Tests for GameSnapshot
├── visualization_manager_test.cpp # Tests for VisualizationManager
└── ...                    # Tests for concrete visualizers
```

## Compilation and Build

The visualization system is controlled by the `ENABLE_VISUALIZATION` CMake option:

```bash
# Build with visualization (default)
cmake -DENABLE_VISUALIZATION=ON ..

# Build without visualization
cmake -DENABLE_VISUALIZATION=OFF ..
```

## Integration with GameManager

### Step 1: Include the visualization header

In `game_manager.h`, add:

```cpp
#ifdef ENABLE_VISUALIZATION
#include "bonus/visualization/visualization.h"
#endif
```

### Step 2: Add a visualization manager member

In the `GameManager` class definition, add:

```cpp
class GameManager {
    // ... existing code
private:
    #ifdef ENABLE_VISUALIZATION
    std::unique_ptr<VisualizationManager> m_visualizationManager;
    #endif
    // ... other members
};
```

### Step 3: Initialize the visualization manager

In the constructor or initialization method:

```cpp
bool GameManager::initialize(const std::string& filePath) {
    // ... existing initialization
    
    #ifdef ENABLE_VISUALIZATION
    m_visualizationManager = createVisualizationManager();
    
    // Optional: Enable live visualization
    // m_visualizationManager->setLiveVisualizationEnabled(true);
    #endif
    
    // ... more initialization
    return true;
}
```

### Step 4: Capture game state at each step

In the game step processing method:

```cpp
void GameManager::processStep() {
    // ... existing step processing
    
    #ifdef ENABLE_VISUALIZATION
    if (m_visualizationManager) {
        // Create a message describing what happened in this step
        std::string stepMessage = "Step " + std::to_string(m_currentStep);
        
        // Add more details if available
        if (!m_lastActionMessage.empty()) {
            stepMessage += ": " + m_lastActionMessage;
        }
        
        // Capture the game state
        m_visualizationManager->captureGameState(
            m_currentStep, 
            m_board, 
            m_tanks, 
            m_shells, 
            stepMessage
        );
        
        // If live visualization is enabled, display the current state
        if (m_visualizationManager->isLiveVisualizationEnabled()) {
            m_visualizationManager->displayCurrentState();
        }
    }
    #endif
    
    // ... more step processing
}
```

### Step 5: Generate output at the end of the game

In the method that saves game results:

```cpp
void GameManager::saveResults(const std::string& outputFilePath) {
    // ... existing code to save text results
    
    #ifdef ENABLE_VISUALIZATION
    if (m_visualizationManager) {
        // Generate visualization outputs with the same base path
        m_visualizationManager->generateOutputs(outputFilePath + "_visualization");
    }
    #endif
    
    // ... more code
}
```

## Conditional Compilation

All visualization code is wrapped with `#ifdef ENABLE_VISUALIZATION` / `#endif` to ensure it doesn't affect the core game functionality when disabled.

## Creating Custom Visualizers

To create a custom visualizer:

1. Create a class that inherits from `VisualizerBase`
2. Implement all the required virtual methods
3. Add your visualizer to the manager:

```cpp
m_visualizationManager->addVisualizer(std::make_unique<MyCustomVisualizer>());
```

## Example Usage

Here's a complete example of integration:

```cpp
#include "game_manager.h"
#ifdef ENABLE_VISUALIZATION
#include "bonus/visualization/visualization.h"
#endif

GameManager::GameManager() {
    // ... other initialization
    
    #ifdef ENABLE_VISUALIZATION
    // Create the visualization manager with default visualizers
    m_visualizationManager = createVisualizationManager();
    
    // Optionally add custom visualizers
    // m_visualizationManager->addVisualizer(std::make_unique<MyCustomVisualizer>());
    
    // Enable live visualization if desired
    // m_visualizationManager->setLiveVisualizationEnabled(true);
    #endif
}

void GameManager::runGame() {
    // ... game initialization
    
    while (!m_gameOver) {
        processStep();
    }
    
    // ... generate final results
    
    #ifdef ENABLE_VISUALIZATION
    if (m_visualizationManager) {
        std::string visualizationPath = m_outputFilePath + "_visualization";
        m_visualizationManager->generateOutputs(visualizationPath);
        std::cout << "Visualization generated at: " << visualizationPath << std::endl;
    }
    #endif
}

void GameManager::processStep() {
    // ... process actions, move objects, handle collisions
    
    #ifdef ENABLE_VISUALIZATION
    if (m_visualizationManager) {
        std::string stepMessage = "Player " + std::to_string(m_currentPlayer) + 
                                 " " + actionToString(m_lastAction);
        
        m_visualizationManager->captureGameState(
            m_currentStep, m_board, m_tanks, m_shells, stepMessage
        );
        
        if (m_visualizationManager->isLiveVisualizationEnabled()) {
            m_visualizationManager->displayCurrentState();
        }
    }
    #endif
    
    // ... update game state
}
```

## Visualizer Types

The system supports different types of visualizers:

1. **HTML Visualizer**: Generates an HTML file with JavaScript for interactive replay
2. **Terminal Visualizer**: Shows the game state in the terminal (useful for debugging)
3. **Custom Visualizers**: You can implement additional visualizers as needed

## Best Practices

1. Keep visualization code separate from game logic
2. Use conditional compilation to ensure visualization can be disabled
3. Capture meaningful messages at each step for better understanding
4. Consider performance implications when using live visualization

## Troubleshooting

If you encounter issues with the visualization system:

1. Ensure the `ENABLE_VISUALIZATION` flag is properly set
2. Check that all required headers are included
3. Verify that the visualization manager is properly initialized
4. Test the visualizers individually to identify any issues

---

For more information on individual visualizers and their customization options, see the specific documentation for each visualizer component.