#pragma once

#include "SatelliteView.h"
#include <vector>
#include <memory>
#include "game_board.h"
#include "objects/tank.h"
#include "objects/shell.h"
#include "utils/point.h"

/**
 * @class SatelliteViewImpl
 * @brief Concrete implementation of SatelliteView providing a read-only view of the game board.
 *
 * This class exposes the current state of the board, tanks, and shells as characters for algorithms.
 * It does not allow modification and does not expose references to underlying objects.
 */
class SatelliteViewImpl : public SatelliteView {
public:
    /**
     * @brief Constructs a SatelliteViewImpl
     * @param board Reference to the game board
     * @param tanks Vector of shared pointers to tanks (const)
     * @param shells Vector of shared pointers to shells (const)
     * @param currentTankPos The position of the current tank (to be marked with '%')
     */
    SatelliteViewImpl(const GameBoard& board,
                     const std::vector<std::shared_ptr<const Tank>>& tanks,
                     const std::vector<std::shared_ptr<const Shell>>& shells,
                     const Point& currentTankPos);

    /**
     * @brief Gets the object at a specific board position as a character
     * @param x The x-coordinate (column)
     * @param y The y-coordinate (row)
     * @return char representing the object at the specified position
     */
    char getObjectAt(size_t x, size_t y) const override;

private:
    const GameBoard& m_board;
    const std::vector<std::shared_ptr<const Tank>>& m_tanks;
    const std::vector<std::shared_ptr<const Shell>>& m_shells;
    Point m_currentTankPos;
}; 