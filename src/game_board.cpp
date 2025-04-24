#include "game_board.h"
#include <sstream>
#include <iostream>

GameBoard::GameBoard() : m_width(0), m_height(0) {}

GameBoard::GameBoard(int width, int height) : m_width(width), m_height(height) {
    m_board.resize(height);
    for (int y = 0; y < height; ++y) {
        m_board[y].resize(width, CellType::Empty);
    }
}

bool GameBoard::initialize(const std::vector<std::string>& boardLines, 
  std::vector<std::string>& errors,
  std::vector<std::pair<int, Point>>& tankPositions) {
  if (boardLines.empty()) {
      std::cerr << "Error: Input board is empty. Cannot initialize game." 
        << std::endl;
      return false;
  }
  m_wallHealth.clear();
  
  int tank1Count = 0;
  int tank2Count = 0;
  for (int y = 0; y < m_height; ++y) {
      for (int x = 0; x < m_width; ++x) {
          setCellType(x, y, CellType::Empty);
      }
  }
  
  for (int y = 0; y < m_height; ++y) {
      if (y >= boardLines.size()) {
          errors.push_back("Missing row " + std::to_string(y) + 
            ". Filled with empty spaces.");
          continue;
      }
      
      const std::string& line = boardLines[y];
      
      if (line.length() < m_width) {
          errors.push_back(
            "Row " + std::to_string(y) + " is shorter than expected width. " +
            "Missing positions filled with empty spaces."
          );
      }
      
      for (int x = 0; x < m_width; ++x) {
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
                  tank1Count++;
                  if (tank1Count > 1) {
                      errors.push_back(
                        "Multiple tanks for player 1 found. Tank at position ("
                          + std::to_string(x) + "," + std::to_string(y) 
                          + ") ignored."
                      );
                  } else {
                    tankPositions.push_back({1, Point(x, y)});
                  }
                  cellType = CellType::Empty;
                  break;
              case '2':
                  tank2Count++;
                  if (tank2Count > 1) {
                      errors.push_back(
                        "Multiple tanks for player 2 found. Tank at position (" 
                          + std::to_string(x) + "," + std::to_string(y) 
                          + ") ignored."
                      );
                  } else {
                    tankPositions.push_back({2, Point(x, y)});
                  }
                  cellType = CellType::Empty;
                  break;
              case '@':
                  cellType = CellType::Mine;
                  break;
              case ' ':
                  cellType = CellType::Empty;
                  break;
              default:
                  errors.push_back(
                    "Unrecognized character '" + std::string(1, currentChar) 
                      + "' at position (" + std::to_string(x) + "," 
                      + std::to_string(y) + "). Treated as empty space."
                  );
                  cellType = CellType::Empty;
                  break;
          }
          setCellType(x, y, cellType);
      }
      
      if (line.length() > m_width) {
          errors.push_back(
            "Row " + std::to_string(y) 
            + " is longer than expected width. Extra characters ignored."
          );
      }
  }
  
  if (boardLines.size() > m_height) {
      errors.push_back(
        "Input has more rows than expected height. Extra rows ignored."
      );
  }
  if (tank1Count == 0) {
      std::cerr << "Error: No tank found for player 1. Cannot start game."
        << std::endl;
      return false;
  }
  if (tank2Count == 0) {
      std::cerr << "Error: No tank found for player 2. Cannot start game."
        << std::endl;
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

int GameBoard::getWidth() const {
    return m_width;
}

int GameBoard::getHeight() const {
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
    
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
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