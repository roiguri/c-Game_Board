// demo_visualization.cpp
#include "bonus/visualization/visualization.h"
#include "game_board.h"
#include "tank.h"
#include "shell.h"
#include <iostream>

int main() {
    // Create a simple game board
    GameBoard board(10, 10);
    std::vector<std::string> boardLines = {
        "#   #####",
        "#     ###  #",
        "#  1     #",
        "#        #",
        "#    @   #",
        "#        #",
        "#     2  #",
        "#        #",
        "#@@       #",
        "##########"
    };
    std::vector<std::string> errors;
    board.initialize(boardLines, errors);
    
    // Create some tanks
    std::vector<Tank> tanks;
    tanks.emplace_back(1, Point(3, 2), Direction::Right);
    tanks.emplace_back(2, Point(6, 6), Direction::Left);
    
    // Create visualization manager
    auto visualizationManager = createVisualizationManager();
    std::vector<Shell> shells;
    
    // Generate a few snapshots of "game play"
    for (int step = 0; step < 10; step++) {
        // Move the tanks in some pattern
        if (step > 0) {
            if (step % 2 == 0) {
                // Rotate tank 1 left
                tanks[0].rotateLeft(false);
                // Rotate tank 2 right
                tanks[1].rotateRight(true);
            } else {
              // Move tank 1 forward
              Point tank1Pos = tanks[0].getPosition();
              Point nextPos1 = tanks[0].getNextForwardPosition();
              tanks[0].moveForward(nextPos1);
              
              // Move tank 2 forward
              Point tank2Pos = tanks[1].getPosition();
              Point nextPos2 = tanks[1].getNextForwardPosition();
              tanks[1].moveForward(nextPos2);
            }
        }
        
        // Create a shell for odd numbered steps
        if (step % 2 == 1) {
            shells.emplace_back(1, Point(step,step), tanks[0].getDirection());
        }
        
        // Capture the game state
        visualizationManager->captureGameState(
            step, 
            board, 
            tanks, 
            shells, 
            "Demo Step " + std::to_string(step)
        );
    }
    
    // Generate the visualization output
    std::string outputPath = "tank_battle_demo_visualization";
    if (visualizationManager->generateOutputs(outputPath)) {
        std::cout << "Visualization generated at " << outputPath << ".html" << std::endl;
        std::cout << "Open this file in a web browser to view the visualization." << std::endl;
    } else {
        std::cerr << "Failed to generate visualization." << std::endl;
    }
    
    return 0;
}