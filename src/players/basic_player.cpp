#include "players/basic_player.h"
#include "players/battle_info_impl.h"
#include <cassert>
#include <iostream>

BasicPlayer::BasicPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : Player(player_index, x, y, max_steps, num_shells),
      m_player_index(player_index),
      m_board_width(x),
      m_board_height(y),
      m_max_steps(max_steps),
      m_num_shells(num_shells),
      m_battleInfo(static_cast<int>(x), static_cast<int>(y)) {}

BasicPlayer::~BasicPlayer() = default;

void BasicPlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    // 1. Populate battle info from satellite view
    populateBattleInfo(satellite_view);
    // 2. Pass to tank algorithm
    tank.updateBattleInfo(m_battleInfo);
}

void BasicPlayer::populateBattleInfo(SatelliteView& satellite_view) {
    m_battleInfo.clear();
    for (size_t y = 0; y < m_board_height; ++y) {
        for (size_t x = 0; x < m_board_width; ++x) {
            char obj = satellite_view.getObjectAt(x, y);
            GameBoard::CellType cellType = GameBoard::CellType::Empty;
            switch (obj) {
                case '#':
                    cellType = GameBoard::CellType::Wall;
                    break;
                case '@':
                    cellType = GameBoard::CellType::Mine;
                    break;
                case '%':
                    cellType = GameBoard::CellType::Empty;
                    m_battleInfo.setOwnTankPosition(Point(x, y));
                    break;
                case '*':
                    cellType = GameBoard::CellType::Empty;
                    m_battleInfo.addShellPosition(Point(x, y));
                    break;
                case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                    cellType = GameBoard::CellType::Empty;
                    int tankPlayerId = obj - '0';
                    if (tankPlayerId == m_player_index) {
                        m_battleInfo.addFriendlyTankPosition(Point(x, y));
                    } else {
                        m_battleInfo.addEnemyTankPosition(Point(x, y));
                    }
                    break;
                }
                case ' ':
                default:
                    cellType = GameBoard::CellType::Empty;
                    break;
            }
            m_battleInfo.setCellType(x, y, cellType);
        }
    }
}