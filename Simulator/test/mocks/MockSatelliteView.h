#pragma once

#include "common/SatelliteView.h"
#include <vector>
#include <string>

/**
 * @brief Mock implementation of SatelliteView for testing purposes
 * 
 * Provides a simple mock implementation that stores a 2D board as a vector of strings.
 * Used for testing components that need to read game state without requiring 
 * a full game simulation.
 */
class MockSatelliteView : public SatelliteView {
public:
    /**
     * @brief Construct mock with board data
     * 
     * @param board Vector of strings representing the board rows
     */
    explicit MockSatelliteView(const std::vector<std::string>& board) : m_board(board) {}
    
    /**
     * @brief Get object at specified position
     * 
     * @param x Column coordinate (0-based)
     * @param y Row coordinate (0-based)
     * @return Character at position, or ' ' if out of bounds
     */
    char getObjectAt(size_t x, size_t y) const override {
        if (y < m_board.size() && x < m_board[y].length()) {
            return m_board[y][x];
        }
        return ' ';
    }
    
    /**
     * @brief Get the board data for verification
     * 
     * @return Reference to the internal board representation
     */
    const std::vector<std::string>& getBoardData() const {
        return m_board;
    }
    
    /**
     * @brief Set board data for dynamic testing
     * 
     * @param board New board data to use
     */
    void setBoardData(const std::vector<std::string>& board) {
        m_board = board;
    }

private:
    std::vector<std::string> m_board;
};