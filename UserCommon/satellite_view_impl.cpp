#include <cstddef>
#include <iostream>

#include "satellite_view_impl.h"

namespace UserCommon_318835816_211314471 {

// Character mapping
constexpr char WALL_CHAR = '#';
constexpr char MINE_CHAR = '@';
constexpr char EMPTY_CHAR = ' ';
constexpr char SHELL_CHAR = '*';
constexpr char TANK_CHARS[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'}; // Up to 9 players
constexpr char CURRENT_TANK_CHAR = '%';
constexpr char OUT_OF_BOARD_CHAR = '&';

SatelliteViewImpl::SatelliteViewImpl(const GameBoard& board,
    const std::vector<Tank>& tanks,
    const std::vector<Shell>& shells,
    const Point& currentTankPos)
    : m_width(board.getWidth()), m_height(board.getHeight()) {
    populateBoardData(board, tanks, shells, currentTankPos, true);
}

SatelliteViewImpl::SatelliteViewImpl(const GameBoard& board,
    const std::vector<Tank>& tanks,
    const std::vector<Shell>& shells)
    : m_width(board.getWidth()), m_height(board.getHeight()) {
    populateBoardData(board, tanks, shells, Point(-1, -1), false);
}

char SatelliteViewImpl::getObjectAt(size_t x, size_t y) const {
    if (x >= m_width || y >= m_height) {
        return OUT_OF_BOARD_CHAR;
    }
    return m_boardData[y][x];
}

void SatelliteViewImpl::populateBoardData(const GameBoard& board,
                                          const std::vector<Tank>& tanks,
                                          const std::vector<Shell>& shells,
                                          const Point& currentTankPos,
                                          bool hasCurrentTank) {
    // Initialize board data with correct dimensions
    m_boardData.resize(m_height, std::vector<char>(m_width, EMPTY_CHAR));
    
    // First, populate the board with walls and mines
    for (size_t y = 0; y < m_height; ++y) {
        for (size_t x = 0; x < m_width; ++x) {
            GameBoard::CellType cellType = board.getCellType(x, y);
            switch (cellType) {
                case GameBoard::CellType::Wall:
                    m_boardData[y][x] = WALL_CHAR;
                    break;
                case GameBoard::CellType::Mine:
                    m_boardData[y][x] = MINE_CHAR;
                    break;
                case GameBoard::CellType::Empty:
                default:
                    m_boardData[y][x] = EMPTY_CHAR;
                    break;
            }
        }
    }
    
    // Then, place shells (shells appear on top of board cells)
    for (const auto& shell : shells) {
        if (shell.isDestroyed()) continue;
        int shell_x = shell.getPosition().getX();
        int shell_y = shell.getPosition().getY();
        if (shell_x >= 0 && shell_x < static_cast<int>(m_width) && 
            shell_y >= 0 && shell_y < static_cast<int>(m_height)) {
            m_boardData[shell_y][shell_x] = SHELL_CHAR;
        }
    }
    
    // Finally, place tanks (tanks appear on top of shells)
    for (const auto& tank : tanks) {
        if (tank.isDestroyed()) continue;
        int tank_x = tank.getPosition().getX();
        int tank_y = tank.getPosition().getY();
        if (tank_x >= 0 && tank_x < static_cast<int>(m_width) && 
            tank_y >= 0 && tank_y < static_cast<int>(m_height)) {
            int playerId = tank.getPlayerId();
            if (playerId >= 1 && playerId <= 9) {
                m_boardData[tank_y][tank_x] = TANK_CHARS[playerId - 1];
            }
        }
    }
    
    // Mark the current tank position with '%' only if we have a current tank
    if (hasCurrentTank) {
        int current_x = currentTankPos.getX();
        int current_y = currentTankPos.getY();
        if (current_x >= 0 && current_x < static_cast<int>(m_width) && 
            current_y >= 0 && current_y < static_cast<int>(m_height)) {
            m_boardData[current_y][current_x] = CURRENT_TANK_CHAR;
        }
    }
}

} // namespace UserCommon_318835816_211314471 