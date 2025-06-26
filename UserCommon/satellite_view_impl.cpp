#include <cstddef>
#include <iostream>

#include "satellite_view_impl.h"

namespace UserCommon_098765432_123456789 {

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
    : m_board(board), m_tanks(tanks), m_shells(shells), m_currentTankPos(currentTankPos), m_hasCurrentTank(true) {}

SatelliteViewImpl::SatelliteViewImpl(const GameBoard& board,
    const std::vector<Tank>& tanks,
    const std::vector<Shell>& shells)
    : m_board(board), m_tanks(tanks), m_shells(shells), m_currentTankPos(-1, -1), m_hasCurrentTank(false) {}

char SatelliteViewImpl::getObjectAt(size_t x, size_t y) const {
    // Cast as board uses int for coordinates (for wrapping calculations)
    int x_int = static_cast<int>(x);
    int y_int = static_cast<int>(y);
    if (x >= m_board.getWidth() || y >= m_board.getHeight()) {
        return OUT_OF_BOARD_CHAR;
    }
    // Mark the current tank position with '%' only if we have a current tank
    if (m_hasCurrentTank && m_currentTankPos.getX() == x_int && m_currentTankPos.getY() == y_int) {
        return CURRENT_TANK_CHAR;
    }
    // Check for tank at (x, y)
    for (const auto& tank : m_tanks) {
        if (tank.isDestroyed()) continue;
        if (tank.getPosition().getX() == x_int && tank.getPosition().getY() == y_int) {
            int playerId = tank.getPlayerId();
            if (playerId >= 1 && playerId <= 9) {
                return TANK_CHARS[playerId - 1];
            } else {
                return EMPTY_CHAR; // fallback for unexpected playerId
            }
        }
    }
    // Check for shell at (x, y)
    for (const auto& shell : m_shells) {
        if (shell.isDestroyed()) continue;
        if (shell.getPosition().getX() == x_int && shell.getPosition().getY() == y_int) {
            return SHELL_CHAR;
        }
    }
    // Check for wall or mine
    GameBoard::CellType cellType = m_board.getCellType(x, y);
    switch (cellType) {
        case GameBoard::CellType::Wall:
            return WALL_CHAR;
        case GameBoard::CellType::Mine:
            return MINE_CHAR;
        case GameBoard::CellType::Empty:
        default:
            return EMPTY_CHAR;
    }
}

} // namespace UserCommon_098765432_123456789 