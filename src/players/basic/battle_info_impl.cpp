#include "players/basic/battle_info_impl.h"

BattleInfoImpl::BattleInfoImpl(int boardWidth, int boardHeight)
    : m_boardWidth(boardWidth), m_boardHeight(boardHeight), m_gameBoard(boardWidth, boardHeight) {}

void BattleInfoImpl::clear() {
    m_gameBoard = GameBoard(m_boardWidth, m_boardHeight);
    m_enemyTankPositions.clear();
    m_friendlyTankPositions.clear();
    m_shellPositions.clear();
}

void BattleInfoImpl::setCellType(int x, int y, GameBoard::CellType cellType) {
    m_gameBoard.setCellType(x, y, cellType);
}

GameBoard::CellType BattleInfoImpl::getCellType(int x, int y) const {
    return m_gameBoard.getCellType(x, y);
}

const GameBoard& BattleInfoImpl::getGameBoard() const {
    return m_gameBoard;
}

void BattleInfoImpl::addEnemyTankPosition(const Point& position) {
    m_enemyTankPositions.push_back(position);
}

void BattleInfoImpl::addFriendlyTankPosition(const Point& position) {
    m_friendlyTankPositions.push_back(position);
}

const std::vector<Point>& BattleInfoImpl::getEnemyTankPositions() const {
    return m_enemyTankPositions;
}

const std::vector<Point>& BattleInfoImpl::getFriendlyTankPositions() const {
    return m_friendlyTankPositions;
}

void BattleInfoImpl::addShellPosition(const Point& position) {
    m_shellPositions.push_back(position);
}

const std::vector<Point>& BattleInfoImpl::getShellPositions() const {
    return m_shellPositions;
}

void BattleInfoImpl::setOwnTankPosition(const Point& pos) {
    m_ownTankPosition = pos;
}

const Point& BattleInfoImpl::getOwnTankPosition() const {
    return m_ownTankPosition;
} 