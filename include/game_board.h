#pragma once
#include <vector>
#include <map>
#include <string>
#include "point.h"

class GameBoard {
public:
    static constexpr int WALL_STARTING_HEALTH = 2;

    enum class CellType {
        Empty,
        Wall,
        Mine,
        Tank1,
        Tank2
    };

    GameBoard();
    GameBoard(int width, int height);
    ~GameBoard();
    
    bool initialize(const std::vector<std::string>& boardLines, std::vector<std::string>& errors);
    
    // Board access
    CellType getCellType(int x, int y) const;
    CellType getCellType(const Point& position) const;
    void setCellType(int x, int y, CellType type);
    void setCellType(const Point& position, CellType type);
    
    // Board properties
    int getWidth() const;
    int getHeight() const;
    
    // Wall handling
    bool isWall(const Point& position) const;
    bool damageWall(const Point& position);
    int getWallHealth(const Point& position) const;
    
    // Position utilities
    Point wrapPosition(const Point& position) const;
    bool canMoveTo(const Point& position) const;
    
    std::string toString() const;

private:
    std::vector<std::vector<CellType>> m_board;
    std::map<Point, int> m_wallHealth;
    int m_width;
    int m_height;
};
