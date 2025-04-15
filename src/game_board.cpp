#include "game_board.h"
#include <sstream>

constexpr int WALL_STARTING_HEALTH = 2;

// Default constructor creates an empty board
GameBoard::GameBoard() : m_width(0), m_height(0) {}

GameBoard::GameBoard(int width, int height) : m_width(width), m_height(height) {
    // Initialize the board with given dimensions
    m_board.resize(height);
    for (int y = 0; y < height; ++y) {
        m_board[y].resize(width, CellType::Empty);
    }
}

// TODO: check if we need a destructor.
GameBoard::~GameBoard() {}

// TODO: add to documentation. assumes gameboard initalized in required dimensions.
bool GameBoard::initialize(const std::vector<std::string>& boardLines) {
    // TODO: unrecoverable error - print to screen and exit safely.
    if (boardLines.empty()) {
        return false;
    }

    // Clear existing data
    m_wallHealth.clear();
    
    // Fill the board based on input lines
    for (int y = 0; y < m_height && y < boardLines.size(); ++y) {
        const std::string& line = boardLines[y];
        
        for (int x = 0; x < m_width && x < line.length(); ++x) {
            CellType cellType = CellType::Empty;
            
            switch (line[x]) {
                case '#':
                    cellType = CellType::Wall;
                    m_wallHealth[Point(x, y)] = WALL_STARTING_HEALTH;
                    break;
                case '1':
                    cellType = CellType::Tank1;
                    break;
                case '2':
                    cellType = CellType::Tank2;
                    break;
                case '@':
                    cellType = CellType::Mine;
                    break;
                default:
                    cellType = CellType::Empty;
                    break;
            }
            
            setCellType(x, y, cellType);
        }
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
    
    // TODO: maybe unnecessary assertion
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
    
    return 0; // TODO: Do we want to return 0 when no walls found?
}

// TODO: add to documentation - range of values supported
Point GameBoard::wrapPosition(const Point& position) const {   
    int wrappedX = ((position.x + m_width) % m_width);
    int wrappedY = ((position.y + m_height) % m_height);
    
    return Point(wrappedX, wrappedY);
}

// TODO: check order of proccesing (shell should hit the wall first)
bool GameBoard::canMoveTo(const Point& position) const {
  Point wrapped = wrapPosition(position);
  
  // Check if the cell is a wall (only walls block movement)
  return m_board[wrapped.y][wrapped.x] != CellType::Wall;
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

bool GameBoard::isValidPosition(int x, int y) const {
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}