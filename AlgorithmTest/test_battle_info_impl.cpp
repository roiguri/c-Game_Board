#include "test_battle_info_impl.h"

using namespace UserCommon_318835816_211314471;

namespace TestAlgorithm_098765432_123456789 {

TestBattleInfoImpl::TestBattleInfoImpl(int boardWidth, int boardHeight)
    : m_boardWidth(boardWidth), m_boardHeight(boardHeight), m_gameBoard(boardWidth, boardHeight) {}

void TestBattleInfoImpl::clear() {
    m_gameBoard = GameBoard(m_boardWidth, m_boardHeight);
    m_enemyTankPositions.clear();
    m_friendlyTankPositions.clear();
    m_shellPositions.clear();
}

void TestBattleInfoImpl::softClear() {
    m_enemyTankPositions.clear();
    m_friendlyTankPositions.clear();
    m_shellPositions.clear();
}

void TestBattleInfoImpl::setCellType(int x, int y, GameBoard::CellType cellType) {
    m_gameBoard.setCellType(x, y, cellType);
}

GameBoard::CellType TestBattleInfoImpl::getCellType(int x, int y) const {
    return m_gameBoard.getCellType(x, y);
}

const GameBoard& TestBattleInfoImpl::getGameBoard() const {
    return m_gameBoard;
}

void TestBattleInfoImpl::addEnemyTankPosition(const Point& position) {
    m_enemyTankPositions.push_back(position);
}

void TestBattleInfoImpl::addFriendlyTankPosition(const Point& position) {
    m_friendlyTankPositions.push_back(position);
}

const std::vector<Point>& TestBattleInfoImpl::getEnemyTankPositions() const {
    return m_enemyTankPositions;
}

const std::vector<Point>& TestBattleInfoImpl::getFriendlyTankPositions() const {
    return m_friendlyTankPositions;
}

void TestBattleInfoImpl::addShellPosition(const Point& position) {
    m_shellPositions.push_back(position);
}

const std::vector<Point>& TestBattleInfoImpl::getShellPositions() const {
    return m_shellPositions;
}

void TestBattleInfoImpl::setOwnTankPosition(const Point& pos) {
    m_ownTankPosition = pos;
}

const Point& TestBattleInfoImpl::getOwnTankPosition() const {
    return m_ownTankPosition;
}

} // namespace Algorithm_098765432_123456789 