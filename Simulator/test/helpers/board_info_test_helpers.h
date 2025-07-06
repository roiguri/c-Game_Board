#pragma once

#include "utils/file_loader.h"
#include "utils/file_satellite_view.h"
#include <memory>
#include <vector>
#include <string>

/**
 * @brief Helper utilities for creating BoardInfo objects for testing
 */
class BoardInfoTestHelpers {
public:
    /**
     * @brief Create valid BoardInfo for testing
     * @return BoardInfo with valid satellite view and standard parameters
     */
    static FileLoader::BoardInfo createValidBoardInfo() {
        FileLoader::BoardInfo boardInfo;
        boardInfo.rows = 10;
        boardInfo.cols = 10;
        boardInfo.maxSteps = 100;
        boardInfo.numShells = 5;
        
        // Create a simple test board
        std::vector<std::string> board = {
            "##########",
            "#1      2#",
            "#        #",
            "#        #", 
            "#        #",
            "#        #",
            "#        #",
            "#        #",
            "#        #",
            "##########"
        };
        
        boardInfo.satelliteView = std::make_unique<FileSatelliteView>(board, boardInfo.rows, boardInfo.cols);
        return boardInfo;
    }
    
    /**
     * @brief Create invalid BoardInfo for error testing
     * @return BoardInfo with null satellite view
     */
    static FileLoader::BoardInfo createInvalidBoardInfo() {
        FileLoader::BoardInfo boardInfo;
        boardInfo.rows = 10;
        boardInfo.cols = 10;
        boardInfo.maxSteps = 100;
        boardInfo.numShells = 5;
        boardInfo.satelliteView = nullptr; // Invalid
        return boardInfo;
    }
    
    /**
     * @brief Create BoardInfo with custom dimensions
     * @param rows Number of rows
     * @param cols Number of columns
     * @param maxSteps Maximum steps
     * @param numShells Number of shells
     * @return BoardInfo with specified parameters
     */
    static FileLoader::BoardInfo createCustomBoardInfo(int rows, int cols, int maxSteps, int numShells) {
        FileLoader::BoardInfo boardInfo;
        boardInfo.rows = rows;
        boardInfo.cols = cols;
        boardInfo.maxSteps = maxSteps;
        boardInfo.numShells = numShells;
        
        // Create board with specified dimensions
        std::vector<std::string> board;
        for (int i = 0; i < rows; ++i) {
            std::string row(cols, ' ');
            if (i == 0 || i == rows - 1) {
                // Top and bottom walls
                std::fill(row.begin(), row.end(), '#');
            } else {
                // Side walls
                row[0] = '#';
                row[cols - 1] = '#';
                if (i == 1) {
                    // Add players on second row
                    if (cols > 2) row[1] = '1';
                    if (cols > 3) row[cols - 2] = '2';
                }
            }
            board.push_back(row);
        }
        
        boardInfo.satelliteView = std::make_unique<FileSatelliteView>(board, boardInfo.rows, boardInfo.cols);
        return boardInfo;
    }
};