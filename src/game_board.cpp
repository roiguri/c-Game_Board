#include "game_board.h"
#include <sstream>
#include <iostream>

// Default constructor creates an empty board
GameBoard::GameBoard() : m_width(0), m_height(0) {}

GameBoard::GameBoard(int width, int height) : m_width(width), m_height(height) {
    // Initialize the board with given dimensions
    m_board.resize(height);
    for (int y = 0; y < height; ++y) {
        m_board[y].resize(width, CellType::Empty);
    }
}

GameBoard::~GameBoard() {}

bool GameBoard::initialize(const std::vector<std::string>& boardLines, std::vector<std::string>& errors) {
  if (boardLines.empty()) {
      std::cerr << "Error: Input board is empty. Cannot initialize game." << std::endl;
      return false;
  }

  // Clear existing data
  m_wallHealth.clear();
  
  // Count tanks to detect duplicates
  int tank1Count = 0;
  int tank2Count = 0;
  
  // Initialize all cells to empty
  for (int y = 0; y < m_height; ++y) {
      for (int x = 0; x < m_width; ++x) {
          setCellType(x, y, CellType::Empty);
      }
  }
  
  // Fill the board based on input lines
  for (int y = 0; y < m_height; ++y) {
      // Handle missing lines
      if (y >= boardLines.size()) {
          errors.push_back("Missing row " + std::to_string(y) + ". Filled with empty spaces.");
          continue;
      }
      
      const std::string& line = boardLines[y];
      
      // Check if the line is shorter than expected
      if (line.length() < m_width) {
          errors.push_back("Row " + std::to_string(y) + " is shorter than expected width. " +
                         "Missing positions filled with empty spaces.");
      }
      
      // Process the available characters in the line
      for (int x = 0; x < m_width; ++x) {
          // Skip processing for missing characters, but don't add errors here
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
                      errors.push_back("Multiple tanks for player 1 found. Tank at position (" + 
                                      std::to_string(x) + "," + std::to_string(y) + ") ignored.");
                      cellType = CellType::Empty;
                  } else {
                    cellType = CellType::Tank1;
                  }
                  break;
              case '2':
                  tank2Count++;
                  if (tank2Count > 1) {
                      errors.push_back("Multiple tanks for player 2 found. Tank at position (" + 
                                      std::to_string(x) + "," + std::to_string(y) + ") ignored.");
                      cellType = CellType::Empty;
                  } else {
                    cellType = CellType::Tank2;
                  }
                  break;
              case '@':
                  cellType = CellType::Mine;
                  break;
              case ' ':
                  cellType = CellType::Empty;
                  break;
              default:
                  errors.push_back("Unrecognized character '" + std::string(1, currentChar) + 
                                  "' at position (" + std::to_string(x) + "," + std::to_string(y) + 
                                  "). Treated as empty space.");
                  cellType = CellType::Empty;
                  break;
          }
          
          setCellType(x, y, cellType);
      }
      
      // Handle extra characters in a line
      if (line.length() > m_width) {
          errors.push_back("Row " + std::to_string(y) + " is longer than expected width. Extra characters ignored.");
      }
  }
  
  // Handle extra lines
  if (boardLines.size() > m_height) {
      errors.push_back("Input has more rows than expected height. Extra rows ignored.");
  }
  
  // Check if both tanks are present
  if (tank1Count == 0) {
      std::cerr << "Error: No tank found for player 1. Cannot start game." << std::endl;
      return false;
  }
  
  if (tank2Count == 0) {
      std::cerr << "Error: No tank found for player 2. Cannot start game." << std::endl;
      return false;
  }
  
  return true;
}

GameBoard::CellType GameBoard::getCellType(int x, int y) const {
    Point wrapped = wrapPosition(Point(x, y));

    return m_board[wrapped.y][wrapped.x];
}

GameBoard::CellType GameBoard::getCellType(const Point& position) const {
    return getCellType(position.x, position.y);
}

void GameBoard::setCellType(int x, int y, CellType type) {
    Point wrapped = wrapPosition(Point(x, y));
    
    // If we're setting a new wall, initialize its health
    if (type == CellType::Wall && m_board[wrapped.y][wrapped.x] != CellType::Wall) {
        m_wallHealth[wrapped] = WALL_STARTING_HEALTH;
    }
    
    m_board[wrapped.y][wrapped.x] = type;
}

void GameBoard::setCellType(const Point& position, CellType type) {
    setCellType(position.x, position.y, type);
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

bool GameBoard::damageWall(const Point& position) {
    Point wrapped = wrapPosition(position);
    
    // Check if there's a wall at the position
    if (!isWall(wrapped)) {
        return false;
    }
    
    // Reduce wall health
    int& health = m_wallHealth[wrapped];
    health--;
    
    // If wall is destroyed, update the board
    if (health <= 0) {
        setCellType(wrapped, CellType::Empty);
        m_wallHealth.erase(wrapped);
        return true; // Wall was destroyed
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

// TODO: add to documentation - range of values supported
Point GameBoard::wrapPosition(const Point& position) const {   
    int wrappedX = ((position.x + m_width) % m_width);
    int wrappedY = ((position.y + m_height) % m_height);
    
    return Point(wrappedX, wrappedY);
}

bool GameBoard::canMoveTo(const Point& position) const {
  Point wrapped = wrapPosition(position);
  
  return m_board[wrapped.y][wrapped.x] != CellType::Wall &&
         m_board[wrapped.y][wrapped.x] != CellType::Mine;
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
                case CellType::Tank1:
                    ss << '1';
                    break;
                case CellType::Tank2:
                    ss << '2';
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