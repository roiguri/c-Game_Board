#include "bonus/board_generator.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <queue>
#include <set>
#include <ctime>
#include <algorithm>
#include <chrono>

BoardGenerator::BoardGenerator() {
    // Initialize with default values from BoardConfig
    // If seed is -1, use current time
    if (m_config.seed == -1) {
        m_config.seed = static_cast<int>(std::chrono::steady_clock::now().time_since_epoch().count());
    }
    m_rng.seed(m_config.seed);
}

BoardGenerator::BoardGenerator(const BoardConfig& config) 
    : m_config(config) {
    // If seed is -1, use current time
    if (m_config.seed == -1) {
        m_config.seed = static_cast<int>(std::chrono::steady_clock::now().time_since_epoch().count());
    }
    m_rng.seed(m_config.seed);
}

bool BoardGenerator::loadConfig(const std::string& configPath) {
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        std::cerr << "Could not open config file: " << configPath << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(configFile, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key-value pairs
        std::size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            std::cerr << "Invalid config line: " << line << std::endl;
            continue;
        }
        
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        if (!parseConfigValue(key, value)) {
            std::cerr << "Invalid value for " << key << ": " << value << std::endl;
            continue;
        }
    }
    
    return true;
}

bool BoardGenerator::parseConfigValue(const std::string& key, const std::string& value) {
    if (key == "dimensions") {
        std::istringstream iss(value);
        int width, height;
        if (!(iss >> width >> height)) {
            return false;
        }
        if (width < 5 || height < 5) {
            std::cerr << "Dimensions too small, minimum size is 5x5" << std::endl;
            return false;
        }
        m_config.width = width;
        m_config.height = height;
    } 
    else if (key == "wall_density") {
        float wallDensity;
        if (!(std::istringstream(value) >> wallDensity)) {
            return false;
        }
        if (wallDensity < 0.0f || wallDensity > 0.9f) {
            std::cerr << "Wall density must be between 0.0 and 0.9" << std::endl;
            return false;
        }
        m_config.wallDensity = wallDensity;
    } 
    else if (key == "mine_density") {
        float mineDensity;
        if (!(std::istringstream(value) >> mineDensity)) {
            return false;
        }
        if (mineDensity < 0.0f || mineDensity > 0.5f) {
            std::cerr << "Mine density must be between 0.0 and 0.5" << std::endl;
            return false;
        }
        m_config.mineDensity = mineDensity;
    } 
    else if (key == "symmetry") {
        if (value != "none" && value != "horizontal" && 
            value != "vertical" && value != "diagonal") {
            std::cerr << "Invalid symmetry type" << std::endl;
            return false;
        }
        m_config.symmetry = value;
    } 
    else if (key == "seed") {
        if (!(std::istringstream(value) >> m_config.seed)) {
            return false;
        }
    }
    else if (key == "max_steps") {
        int ms;
        if (!(std::istringstream(value) >> ms)) return false;
        m_config.maxSteps = ms;
    }
    else if (key == "num_shells") {
        int ns;
        if (!(std::istringstream(value) >> ns)) return false;
        m_config.numShells = ns;
    }
    else if (key == "map_name") {
        m_config.mapName = value;
    }
    else if (key == "num_tanks_per_player") {
        int nt;
        if (!(std::istringstream(value) >> nt)) return false;
        if (nt < 1) return false;
        m_config.numTanksPerPlayer = nt;
    }
    else {
        std::cerr << "Unknown configuration key: " << key << std::endl;
        return false;
    }
    
    return true;
}

bool BoardGenerator::generateBoard() {
    // Reset RNG with seed
    m_rng.seed(m_config.seed);
    
    // Initialize the board with empty spaces
    initializeEmptyBoard();
    
    // Place tanks
    if (!placeTanks()) {
        return false;
    }
    
    // Place walls
    placeWalls();
    
    // Place mines
    placeMines();
    
    // Validate the board (check connectivity, etc.)
    return validateBoard();
}

void BoardGenerator::initializeEmptyBoard() {
    m_board.clear();
    m_board.resize(m_config.height, std::vector<char>(m_config.width, ' '));
}

bool BoardGenerator::placeTanks() {
    m_tankPositions.clear();
    int tanksPlaced1 = 0, tanksPlaced2 = 0;
    int maxAttempts = 10000;
    std::uniform_int_distribution<int> xDist(0, m_config.width - 1);
    std::uniform_int_distribution<int> yDist(0, m_config.height - 1);
    std::uniform_int_distribution<int> playerDist(0, 1); // 0 for P1, 1 for P2
    int attempts = 0;
    while ((tanksPlaced1 < m_config.numTanksPerPlayer || tanksPlaced2 < m_config.numTanksPerPlayer) && attempts < maxAttempts) {
        int x = xDist(m_rng);
        int y = yDist(m_rng);
        auto [mx, my] = getMirror(x, y);
        // Check if either position is occupied
        if (isOccupied(x, y) || isOccupied(mx, my)) {
            ++attempts;
            continue;
        }
        // Self-mirrored position (center)
        if (x == mx && y == my) {
            int chosenPlayer = playerDist(m_rng) ? 2 : 1;
            if (chosenPlayer == 1 && tanksPlaced1 < m_config.numTanksPerPlayer) {
                placeTank(x, y, 1);
                ++tanksPlaced1;
            } else if (chosenPlayer == 2 && tanksPlaced2 < m_config.numTanksPerPlayer) {
                placeTank(x, y, 2);
                ++tanksPlaced2;
            }
            ++attempts;
            continue;
        }
        // Assign players randomly to original/mirror
        int p1x = x, p1y = y, p2x = mx, p2y = my;
        if (playerDist(m_rng)) {
            std::swap(p1x, p2x);
            std::swap(p1y, p2y);
        }
        // Check if both players have room
        if (tanksPlaced1 < m_config.numTanksPerPlayer && tanksPlaced2 < m_config.numTanksPerPlayer) {
            placeTank(p1x, p1y, 1);
            placeTank(p2x, p2y, 2);
            ++tanksPlaced1;
            ++tanksPlaced2;
        } else if (tanksPlaced1 < m_config.numTanksPerPlayer) {
            placeTank(p1x, p1y, 1);
            ++tanksPlaced1;
        } else if (tanksPlaced2 < m_config.numTanksPerPlayer) {
            placeTank(p2x, p2y, 2);
            ++tanksPlaced2;
        }
        ++attempts;
    }
    // If we failed to place all tanks, return false
    return tanksPlaced1 == m_config.numTanksPerPlayer && tanksPlaced2 == m_config.numTanksPerPlayer;
}

void BoardGenerator::placeWalls() {
    // Calculate how many wall cells to place based on density
    int boardArea = m_config.width * m_config.height;
    int interiorArea = boardArea - 2; // Subtract tank positions

    int symmetryFactor = 1;
    if (m_config.symmetry != "none") {
        symmetryFactor = 2;
    }
    
    int wallsToPlace = static_cast<int>((interiorArea * m_config.wallDensity)
                                                       / symmetryFactor);
    
    // Create a list of candidate positions
    std::vector<std::pair<int, int>> candidates;
    for (int y = 0; y < m_config.height; ++y) {
        for (int x = 0; x < m_config.width; ++x) {
            if (m_board[y][x] == ' ') {
                candidates.push_back(std::make_pair(x, y));
            }
        }
    }
    
    // Shuffle the candidates
    std::shuffle(candidates.begin(), candidates.end(), m_rng);
    
    // Place walls while respecting symmetry
    int wallsPlaced = 0;
    for (const auto& pos : candidates) {
        if (wallsPlaced >= wallsToPlace) {
            break;
        }
        
        int x = pos.first;
        int y = pos.second;
        
        // Skip if already occupied
        if (m_board[y][x] != ' ') {
            continue;
        }
        
        bool isPrimaryPosition = true;
        
        if (m_config.symmetry == "horizontal") {
            int mirrorX = m_config.width - 1 - x;
            isPrimaryPosition = (x <= mirrorX);
        } 
        else if (m_config.symmetry == "vertical") {
            int mirrorY = m_config.height - 1 - y;
            isPrimaryPosition = (y <= mirrorY);
        }
        else if (m_config.symmetry == "diagonal") {
            int mirrorX = m_config.width - 1 - x;
            int mirrorY = m_config.height - 1 - y;
            isPrimaryPosition = (x <= mirrorX && y <= mirrorY);
        }
        
        if (!isPrimaryPosition) {
            continue;
        }
        
        // Place wall and apply symmetry
        applySymmetry(x, y, '#');
        wallsPlaced++;
        // Verify connectivity after placing each wall
        bool allConnected = true;
        for (size_t i = 0; i < m_tankPositions.size(); ++i) {
            for (size_t j = i + 1; j < m_tankPositions.size(); ++j) {
                if (!canReach(m_tankPositions[i].first, m_tankPositions[i].second,
                              m_tankPositions[j].first, m_tankPositions[j].second)) {
                    allConnected = false;
                    break;
                }
            }
            if (!allConnected) break;
        }
        if (!allConnected) {
            applySymmetry(x, y, ' ');
            wallsPlaced--;
        }
    }
}

void BoardGenerator::placeMines() {
    // Calculate how many mine cells to place based on density
    int boardArea = m_config.width * m_config.height - 2; // Exclude tanks

    int symmetryFactor = 1;
    if (m_config.symmetry != "none") {
        symmetryFactor = 2;
    }

    int minesToPlace = static_cast<int>((boardArea * m_config.mineDensity) 
                                                       / symmetryFactor);

    // Create a list of candidate positions (cells not occupied by tanks or walls)
    std::vector<std::pair<int, int>> candidates;
    for (int y = 0; y < m_config.height; ++y) {
        for (int x = 0; x < m_config.width; ++x) {
            if (m_board[y][x] == ' ') {
                candidates.push_back(std::make_pair(x, y));
            }
        }
    }
    
    // Shuffle the candidates
    std::shuffle(candidates.begin(), candidates.end(), m_rng);
    
    // Place mines while respecting symmetry
    int minesPlaced = 0;
    for (const auto& pos : candidates) {
        if (minesPlaced >= minesToPlace) {
            break;
        }
        
        int x = pos.first;
        int y = pos.second;
        
        // Skip if already occupied
        if (m_board[y][x] != ' ') {
            continue;
        }

        bool isPrimaryPosition = true;
        
        if (m_config.symmetry == "horizontal") {
            int mirrorX = m_config.width - 1 - x;
            isPrimaryPosition = (x <= mirrorX);
        } 
        else if (m_config.symmetry == "vertical") {
            int mirrorY = m_config.height - 1 - y;
            isPrimaryPosition = (y <= mirrorY);
        }
        else if (m_config.symmetry == "diagonal") {
            int mirrorX = m_config.width - 1 - x;
            int mirrorY = m_config.height - 1 - y;
            isPrimaryPosition = (x <= mirrorX && y <= mirrorY);
        }
        
        if (!isPrimaryPosition) {
            continue;
        }
        
        // Place mine and apply symmetry
        applySymmetry(x, y, '@');
        minesPlaced++;
    }
}

bool BoardGenerator::validateBoard() {
    // Check if tanks are placed
    int p1 = 0, p2 = 0;
    std::vector<std::pair<int, int>> tankPositions;
    for (int y = 0; y < m_config.height; ++y) {
        for (int x = 0; x < m_config.width; ++x) {
            if (m_board[y][x] == '1') { ++p1; tankPositions.push_back({x, y}); }
            if (m_board[y][x] == '2') { ++p2; tankPositions.push_back({x, y}); }
        }
    }
    if (p1 < 1 || p2 < 1) {
        std::cerr << "Tanks are not properly placed on the board" << std::endl;
        return false;
    }
    // Check connectivity between all tanks
    for (size_t i = 0; i < tankPositions.size(); ++i) {
        for (size_t j = i + 1; j < tankPositions.size(); ++j) {
            if (!canReach(tankPositions[i].first, tankPositions[i].second, tankPositions[j].first, tankPositions[j].second)) {
                std::cerr << "There is no valid path between tanks at (" << tankPositions[i].first << "," << tankPositions[i].second << ") and (" << tankPositions[j].first << "," << tankPositions[j].second << ")" << std::endl;
                return false;
            }
        }
    }
    return true;
}

bool BoardGenerator::canReach(int startX, int startY, int endX, int endY) {
    // BFS to check if there's a path from start to end
    std::queue<std::pair<int, int>> queue;
    std::set<std::pair<int, int>> visited;
    
    queue.push(std::make_pair(startX, startY));
    visited.insert(std::make_pair(startX, startY));
    
    // Direction vectors for 4-way movement
    const int dx[] = {0, 1, 0, -1};
    const int dy[] = {-1, 0, 1, 0};
    
    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();
        
        int x = current.first;
        int y = current.second;
        
        // Check if we've reached the target
        if (x == endX && y == endY) {
            return true;
        }
        
        // Try all four directions
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            
            // Check if the new position is valid
            if (nx >= 0 && nx < m_config.width &&
                ny >= 0 && ny < m_config.height) {
                if (m_board[ny][nx] != '#' && visited.find(std::make_pair(nx, ny)) == visited.end()) {
                    visited.insert(std::make_pair(nx, ny));
                    queue.push(std::make_pair(nx, ny));
                }
            }
        }
    }
    
    // If we get here, there's no path
    return false;
}

void BoardGenerator::applySymmetry(int x, int y, char cellType) {
    // Place cell at original position
    m_board[y][x] = cellType;
    
    // Apply symmetry based on configuration
    if (m_config.symmetry == "horizontal") {
        int mirrorX = m_config.width - 1 - x;
        // Skip if it's the same position or if it would override a tank
        if (mirrorX != x && m_board[y][mirrorX] != '1' && m_board[y][mirrorX] != '2') {
            m_board[y][mirrorX] = cellType;
        }
    } 
    else if (m_config.symmetry == "vertical") {
        int mirrorY = m_config.height - 1 - y;
        if (mirrorY != y && m_board[mirrorY][x] != '1' && m_board[mirrorY][x] != '2') {
            m_board[mirrorY][x] = cellType;
        }
    } 
    else if (m_config.symmetry == "diagonal") {
        int mirrorX = m_config.width - 1 - x;
        int mirrorY = m_config.height - 1 - y;
        if ((mirrorX != x || mirrorY != y) && 
            m_board[mirrorY][mirrorX] != '1' && m_board[mirrorY][mirrorX] != '2') {
            m_board[mirrorY][mirrorX] = cellType;
        }
    }
}

std::vector<std::pair<int, int>> BoardGenerator::getSymmetryPositions(int x, int y) {
    std::vector<std::pair<int, int>> positions;
    
    // Original position
    positions.push_back(std::make_pair(x, y));
    
    // Apply symmetry based on configuration
    if (m_config.symmetry == "horizontal") {
        int mirrorX = m_config.width - 1 - x;
        if (mirrorX != x) {
            positions.push_back(std::make_pair(mirrorX, y));
        }
    } 
    else if (m_config.symmetry == "vertical") {
        int mirrorY = m_config.height - 1 - y;
        if (mirrorY != y) {
            positions.push_back(std::make_pair(x, mirrorY));
        }
    } 
    else if (m_config.symmetry == "diagonal") {
        int mirrorX = m_config.width - 1 - x;
        int mirrorY = m_config.height - 1 - y;
        if (mirrorX != x || mirrorY != y) {
            positions.push_back(std::make_pair(mirrorX, mirrorY));
        }
    }
    
    return positions;
}

bool BoardGenerator::isValidPosition(int x, int y) const {
    if (x < 0 || x >= m_config.width || y < 0 || y >= m_config.height)
        return false;
    for (const auto& pos : m_tankPositions) {
        if (pos.first == x && pos.second == y)
            return false;
    }
    return true;
}

bool BoardGenerator::saveToFile(const std::string& outputPath) const {
    std::ofstream outputFile(outputPath);
    if (!outputFile.is_open()) {
        std::cerr << "Could not open output file: " << outputPath << std::endl;
        return false;
    }
    // Write 5-line header
    outputFile << m_config.mapName << std::endl;
    outputFile << "MaxSteps = " << m_config.maxSteps << std::endl;
    outputFile << "NumShells = " << m_config.numShells << std::endl;
    outputFile << "Rows = " << m_config.height << std::endl;
    outputFile << "Cols = " << m_config.width << std::endl;
    // Write the board
    for (const auto& row : m_board) {
        for (char cell : row) {
            outputFile << cell;
        }
        outputFile << std::endl;
    }
    outputFile.close();
    return true;
}

std::vector<std::string> BoardGenerator::getBoardLines() const {
    std::vector<std::string> lines;
    // 5-line header
    lines.push_back(m_config.mapName);
    lines.push_back("MaxSteps = " + std::to_string(m_config.maxSteps));
    lines.push_back("NumShells = " + std::to_string(m_config.numShells));
    lines.push_back("Rows = " + std::to_string(m_config.height));
    lines.push_back("Cols = " + std::to_string(m_config.width));
    // Board content
    for (const auto& row : m_board) {
        std::string line(row.begin(), row.end());
        lines.push_back(line);
    }
    return lines;
}

const BoardConfig& BoardGenerator::getConfig() const {
    return m_config;
}

void BoardGenerator::setConfig(const BoardConfig& config) {
    m_config = config;
    if (m_config.seed == -1) {
        m_config.seed = static_cast<int>(std::chrono::steady_clock::now().time_since_epoch().count());
    }
    m_rng.seed(m_config.seed);
}

std::pair<int, int> BoardGenerator::getMirror(int x, int y) const {
    if (m_config.symmetry == "horizontal") {
        return {m_config.width - 1 - x, y};
    } else if (m_config.symmetry == "vertical") {
        return {x, m_config.height - 1 - y};
    } else if (m_config.symmetry == "diagonal") {
        return {m_config.width - 1 - x, m_config.height - 1 - y};
    } else {
        return {x, y}; // No symmetry
    }
}

bool BoardGenerator::isOccupied(int x, int y) const {
    for (const auto& pos : m_tankPositions) {
        if (pos.first == x && pos.second == y)
            return true;
    }
    return false;
}

void BoardGenerator::placeTank(int x, int y, int player) {
    m_tankPositions.push_back({x, y});
    m_board[y][x] = (player == 1) ? '1' : '2';
}