#include "utils/point.h"
#include "game_board.h"
#include <vector>
#include "BattleInfo.h"
// TODO: consider removing some of the methods that are not needed by the player
/**
 * @class BattleInfo
 * @brief Contains battlefield information shared between Player and Tank
 */
class BattleInfoImpl : public BattleInfo {
public:
    // Constructor & destructor
    BattleInfoImpl(int boardWidth, int boardHeight);
    virtual ~BattleInfoImpl() = default;
    
    // Board management
    void clear();
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
    
    // Feedback mechanism
    void setFeedback(const std::string& feedback);
    const std::string& getFeedback() const;
    
private:
    int m_boardWidth;
    int m_boardHeight;
    GameBoard m_gameBoard;
    std::vector<Point> m_enemyTankPositions;
    std::vector<Point> m_friendlyTankPositions;
    std::vector<Point> m_shellPositions;
    std::string m_feedback;
};