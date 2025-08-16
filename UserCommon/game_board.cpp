#include <iostream>
#include <sstream>

#include "game_board.h"

namespace UserCommon_098765432_123456789 {

GameBoard::GameBoard() : m_width(0), m_height(0) {}

GameBoard::GameBoard(size_t width, size_t height) : m_width(width), m_height(height) {
    m_board.resize(height);
    for (size_t y = 0; y < height; ++y) {
        m_board[y].resize(width, CellType::Empty);
    }
}

bool GameBoard::initialize(const std::vector<std::string>& boardLines, 
  std::vector<std::pair<int, Point>>& tankPositions) {
  if (boardLines.empty()) {
      return false;
  }
  m_wallHealth.clear();
  
  for (size_t y = 0; y < m_height; ++y) {
      for (size_t x = 0; x < m_width; ++x) {
          setCellType(x, y, CellType::Empty);
      }
  }
  
  for (size_t y = 0; y < m_height; ++y) {
      if (y >= boardLines.size()) {
          continue;
      }
      
      const std::string& line = boardLines[y];
      
      for (size_t x = 0; x < m_width; ++x) {
          if (x >= line.length()) {
              continue;
          }
          
          CellType cellType = CellType::Empty;
          char currentChar = line[x];
          
          switch (currentChar) {
              case '#':
                  cellType = CellType::Wall;
                  m_wallHealth[Point(x, y)] = WALL_STARTING_HEALTH;
                  break;
              case '1':
                  tankPositions.push_back({1, Point(x, y)});
                  cellType = CellType::Empty;
                  break;
              case '2':
                  tankPositions.push_back({2, Point(x, y)});
                  cellType = CellType::Empty;
                  break;
              case '3':
                  tankPositions.push_back({3, Point(x, y)});
                  cellType = CellType::Empty;
                  break;
              case '4':
                  tankPositions.push_back({4, Point(x, y)});
                  cellType = CellType::Empty;
                  break;
              case '5':
                  tankPositions.push_back({5, Point(x, y)});
                  cellType = CellType::Empty;
                  break;
              case '6':
                  tankPositions.push_back({6, Point(x, y)});
                  cellType = CellType::Empty;
                  break;
              case '7':
                  tankPositions.push_back({7, Point(x, y)});
                  cellType = CellType::Empty;
                  break;
              case '8':
                  tankPositions.push_back({8, Point(x, y)});
                  cellType = CellType::Empty;
                  break;
              case '9':
                  tankPositions.push_back({9, Point(x, y)});
                  cellType = CellType::Empty;
                  break;
              case '@':
                  cellType = CellType::Mine;
                  break;
              case ' ':
                  cellType = CellType::Empty;
                  break;
              default:
                  cellType = CellType::Empty;
                  break;
          }
          setCellType(x, y, cellType);
      }
  }
  
  if (tankPositions.empty()) {
      return false;
  }
  return true;
}

GameBoard::CellType GameBoard::getCellType(int x, int y) const {
    Point wrapped = wrapPosition(Point(x, y));

    return m_board[wrapped.getY()][wrapped.getX()];
}

GameBoard::CellType GameBoard::getCellType(const Point& position) const {
    return getCellType(position.getX(), position.getY());
}

void GameBoard::setCellType(int x, int y, CellType type) {
    Point wrapped = wrapPosition(Point(x, y));
    
    if (type == CellType::Wall && 
          m_board[wrapped.getY()][wrapped.getX()] != CellType::Wall) {
      m_wallHealth[wrapped] = WALL_STARTING_HEALTH;
    }
    
    m_board[wrapped.getY()][wrapped.getX()] = type;
}

void GameBoard::setCellType(const Point& position, CellType type) {
    setCellType(position.getX(), position.getY(), type);
}

size_t GameBoard::getWidth() const {
    return m_width;
}

size_t GameBoard::getHeight() const {
    return m_height;
}

bool GameBoard::isWall(const Point& position) const {
    return getCellType(position) == CellType::Wall;
}

bool GameBoard::isMine(const Point& position) const {
    return getCellType(position) == CellType::Mine;
}

bool GameBoard::damageWall(const Point& position) {
    Point wrapped = wrapPosition(position);
    
    if (!isWall(wrapped)) {
        return false;
    }
    
    int& health = m_wallHealth[wrapped];
    health--;
    
    if (health <= 0) {
        setCellType(wrapped, CellType::Empty);
        m_wallHealth.erase(wrapped);
        return true; // Wall destroyed
    }
    
    return false; // Wall was damaged but not destroyed
}

int GameBoard::getWallHealth(const Point& position) const {
    Point wrapped = wrapPosition(position);
    
    auto it = m_wallHealth.find(wrapped);
    if (it != m_wallHealth.end()) {
        return it->second;
    }
    
    return 0;
}

Point GameBoard::wrapPosition(const Point& position) const {
    if (m_width == 0 || m_height == 0) {
        return Point(-1, -1);
    }
    int wrappedX = ((position.getX() + m_width) % m_width);
    int wrappedY = ((position.getY() + m_height) % m_height);
    return Point(wrappedX, wrappedY);
}

bool GameBoard::canMoveTo(const Point& position) const {
  Point wrapped = wrapPosition(position);
  
  return m_board[wrapped.getY()][wrapped.getX()] != CellType::Wall;
}

std::string GameBoard::toString() const {
    std::stringstream ss;
    
    for (size_t y = 0; y < m_height; ++y) {
        for (size_t x = 0; x < m_width; ++x) {
            CellType cell = m_board[y][x];
            
            switch (cell) {
                case CellType::Empty:
                    ss << ' ';
                    break;
                case CellType::Wall:
                    ss << '#';
                    break;
                case CellType::Mine:
                    ss << '@';
                    break;
                default:
                    ss << '?';
                    break;
            }
        }
        ss << '\n';
    }
    
    return ss.str();
}

int GameBoard::stepDistance(const Point& a, const Point& b, size_t width, size_t height) {
    // Cast to int for signed arithmetic needed in wrapping distance calculation
    int w = static_cast<int>(width);
    int h = static_cast<int>(height);
    int dx = std::abs(a.getX() - b.getX());
    int dy = std::abs(a.getY() - b.getY());
    // Consider wrapping
    dx = std::min(dx, w - dx);
    dy = std::min(dy, h - dy);
    // Chebyshev distance (since diagonal and orthogonal moves cost the same)
    return std::max(dx, dy);
}

} // namespace UserCommon_098765432_123456789