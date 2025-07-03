#pragma once

#include <vector>

#include "UserCommon/game_board.h"
#include "UserCommon/utils/point.h"
#include "common/BattleInfo.h"

using namespace UserCommon_098765432_123456789;

namespace TestAlgorithm_098765432_123456789 {

/**
 * @class TestBattleInfoImpl
 * @brief Contains battlefield information shared between Player and Tank
 */
class TestBattleInfoImpl : public BattleInfo {
public:
    // Constructor & destructor
    TestBattleInfoImpl(int boardWidth, int boardHeight);
    virtual ~TestBattleInfoImpl() = default;
    
    // Board management
    void clear();
    void softClear(); // clear shells and tanks vectors
    void setCellType(int x, int y, GameBoard::CellType cellType);
    GameBoard::CellType getCellType(int x, int y) const;
    const GameBoard& getGameBoard() const;
    
    // Tank position management
    void addEnemyTankPosition(const Point& position);
    void addFriendlyTankPosition(const Point& position);
    const std::vector<Point>& getEnemyTankPositions() const;
    const std::vector<Point>& getFriendlyTankPositions() const;
    
    // Shell position management
    void addShellPosition(const Point& position);
    const std::vector<Point>& getShellPositions() const;
    
    void setOwnTankPosition(const Point& pos);
    const Point& getOwnTankPosition() const;
    
private:
    int m_boardWidth;
    int m_boardHeight;
    GameBoard m_gameBoard;
    std::vector<Point> m_enemyTankPositions;
    std::vector<Point> m_friendlyTankPositions;
    std::vector<Point> m_shellPositions;
    Point m_ownTankPosition;
};

} // namespace TestAlgorithm_098765432_123456789