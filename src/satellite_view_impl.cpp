#include "satellite_view_impl.h"
#include <cstddef>
#include <iostream>

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
    : m_board(board), m_tanks(tanks), m_shells(shells), m_currentTankPos(currentTankPos) {}

char SatelliteViewImpl::getObjectAt(size_t x, size_t y) const {
    if (x < 0 || x >= m_board.getWidth() || y < 0 || y >= m_board.getHeight()) {
        return OUT_OF_BOARD_CHAR;
    }
    // Mark the current tank position with '%'
    if (m_currentTankPos.getX() == x && m_currentTankPos.getY() == y) {
        return CURRENT_TANK_CHAR;
    }
    // Check for tank at (x, y)
    for (const auto& tank : m_tanks) {
        if (tank.isDestroyed()) continue;
        if (tank.getPosition().getX() == x && tank.getPosition().getY() == y) {
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
        if (shell.getPosition().getX() == x && shell.getPosition().getY() == y) {
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