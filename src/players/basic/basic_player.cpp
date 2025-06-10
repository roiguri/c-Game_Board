#include "players/basic/basic_player.h"
#include "players/basic/battle_info_impl.h"
#include <cassert>
#include <iostream>

BasicPlayer::BasicPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
    : Player(playerIndex, x, y, maxSteps, numShells),
      m_playerIndex(playerIndex),
      m_boardWidth(x),
      m_boardHeight(y),
      m_maxSteps(maxSteps),
      m_numShells(numShells),
      // Cast to int as BattleInfoImpl constructor expects int parameters for board dimensions
      m_battleInfo(static_cast<int>(x), static_cast<int>(y)) {}

BasicPlayer::~BasicPlayer() = default;

void BasicPlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satelliteView) {
    // 1. Populate battle info from satellite view
    populateBattleInfo(satelliteView);
    // 2. Pass to tank algorithm
    tank.updateBattleInfo(m_battleInfo);
}

void BasicPlayer::populateBattleInfo(SatelliteView& satelliteView) {
    parseSatelliteViewToBattleInfo(m_battleInfo, satelliteView, m_playerIndex, m_boardWidth, m_boardHeight);
}

void BasicPlayer::parseSatelliteViewToBattleInfo(BattleInfoImpl& battleInfo, 
                                                 SatelliteView& satelliteView,
                                                 int playerIndex,
                                                 size_t boardWidth, 
                                                 size_t boardHeight) {
    battleInfo.softClear(); // clear only tanks and shells, board will be overriden.
    for (size_t y = 0; y < boardHeight; ++y) {
        for (size_t x = 0; x < boardWidth; ++x) {
            char obj = satelliteView.getObjectAt(x, y);
            GameBoard::CellType cellType = battleInfo.getCellType(x, y);
            switch (obj) {
                case '#':
                    cellType = GameBoard::CellType::Wall;
                    break;
                case '@':
                    cellType = GameBoard::CellType::Mine;
                    break;
                case '%':
                    cellType = GameBoard::CellType::Empty;
                    battleInfo.setOwnTankPosition(Point(x, y));
                    break;
                case '*':
                    battleInfo.addShellPosition(Point(x, y));
                    break;
                case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                    cellType = GameBoard::CellType::Empty;
                    int tankPlayerId = obj - '0';
                    if (tankPlayerId == playerIndex) {
                        battleInfo.addFriendlyTankPosition(Point(x, y));
                    } else {
                        battleInfo.addEnemyTankPosition(Point(x, y));
                    }
                    break;
                }
                case ' ':
                default:
                    cellType = GameBoard::CellType::Empty;
                    break;
            }
            battleInfo.setCellType(x, y, cellType);
        }
    }
}