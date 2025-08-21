#pragma once

#include <vector>

#include "game_board.h"
#include "objects/shell.h"
#include "objects/tank.h"
#include "utils/point.h"
#include "SatelliteView.h"

namespace UserCommon_318835816_211314471 {

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
     * @param tanks Reference to a vector of tanks (const)
     * @param shells Reference to a vector of shells (const)
     * @param currentTankPos The position of the current tank (to be marked with '%')
     */
    SatelliteViewImpl(const GameBoard& board,
                     const std::vector<Tank>& tanks,
                     const std::vector<Shell>& shells,
                     const Point& currentTankPos);

    /**
     * @brief Constructs a SatelliteViewImpl for final game state (no current tank marking)
     * @param board Reference to the game board
     * @param tanks Reference to a vector of tanks (const)
     * @param shells Reference to a vector of shells (const)
     */
    SatelliteViewImpl(const GameBoard& board,
                     const std::vector<Tank>& tanks,
                     const std::vector<Shell>& shells);

    /**
     * @brief Gets the object at a specific board position as a character
     * @param x The x-coordinate (column)
     * @param y The y-coordinate (row)
     * @return char representing the object at the specified position
     */
    char getObjectAt(size_t x, size_t y) const override;

private:
    /**
     * @brief Populates the board data structure from game state
     * @param board Reference to the game board
     * @param tanks Reference to a vector of tanks (const)
     * @param shells Reference to a vector of shells (const)
     * @param currentTankPos The position of the current tank (optional)
     * @param hasCurrentTank Whether to mark current tank with '%'
     */
    void populateBoardData(const GameBoard& board,
                          const std::vector<Tank>& tanks,
                          const std::vector<Shell>& shells,
                          const Point& currentTankPos,
                          bool hasCurrentTank);

    std::vector<std::vector<char>> m_boardData; // Owns the game state data
    size_t m_width;
    size_t m_height;
};

} // namespace UserCommon_318835816_211314471 