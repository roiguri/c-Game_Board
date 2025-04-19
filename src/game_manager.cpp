#include "game_manager.h"
#include "file_loader.h"
#include "collision_handler.h"
#include <iostream>
#include <fstream>

GameManager::GameManager()
    : m_player1Algorithm(nullptr),
      m_player2Algorithm(nullptr),
      m_currentStep(0),
      m_gameOver(false) {
    // Initialize empty game state
}

GameManager::~GameManager() {
    // Clean up resources
    cleanup();
}

// Accessors
const std::vector<std::string>& GameManager::getGameLog() const {
    return m_gameLog;
}
std::vector<Tank> GameManager::getTanks() const {
    return m_tanks;
}

bool GameManager::initialize(const std::string& filePath) {
    // TODO: potentially add reset state method.

    // Load board file
    int boardWidth = 0;
    int boardHeight = 0;
    std::vector<std::string> boardLines = FileLoader::loadBoardFile(filePath, boardWidth, boardHeight);
    
    if (boardLines.empty()) {
        return false;
    }
    
    // Initialize the board
    m_board = GameBoard(boardWidth, boardHeight);
    std::vector<std::string> errors;
    
    if (!m_board.initialize(boardLines, errors)) {
        // Unrecoverable error in board initialization
        return false;
    }
    if (!saveErrorsToFile(errors)) {
        std::cerr << "Error: Could not save errors to file" << std::endl;
        return false;
    }
    
    createTanksFromBoard();
    createAlgorithms(); // TODO: optional add unit tests to cover this
    
    return true;
}

void GameManager::runGame() {
    // TODO: Implement game loop
}

void GameManager::saveResults(const std::string& outputFilePath) {
    // TODO: Save game log and results to file
}

void GameManager::processStep() {
    // This is a stub implementation
}

Action GameManager::getPlayerAction(int playerId) {
  if (playerId != 1 && playerId != 2) {
    return Action::None;
  }

  // Find the player's tank
  Tank* playerTank = nullptr;
  Tank* enemyTank = nullptr;
  
  for (auto& tank : m_tanks) {
      if (tank.getPlayerId() == playerId) {
          playerTank = &tank;
      } else {
          enemyTank = &tank;
      }
  }
  
  if (!playerTank || !enemyTank) {
      return Action::None;
  }
  // Get the appropriate algorithm
  Algorithm* algorithm = (playerId == 1) ? m_player1Algorithm : m_player2Algorithm;
  if (!algorithm) {
      return Action::None;
  }
  // Get action from algorithm
  return algorithm->getNextAction(m_board, *playerTank, *enemyTank, m_shells);
}

bool GameManager::applyAction(int playerId, Action action) {
    // This is a stub implementation
    return false;
}

void GameManager::moveShells() {
    // This is a stub implementation
}

bool GameManager::checkGameOver() {
    // This is a stub implementation
    return false;
}

// TODO: consider logging game state for each action logged - tank position, etc
void GameManager::logAction(int playerId, Action action, bool valid) {
  std::string actionType = actionToString(action);
  std::string status = valid ? "Success" : "Invalid";
  
  // Format: "Player X: [Action] - [Status]"
  std::string logEntry = "Player " + std::to_string(playerId) + ": " + 
                         actionType + " - " + status;
  
  m_gameLog.push_back(logEntry);
}

void GameManager::createAlgorithms() {
    // This is a stub implementation
    // Clean up any existing algorithms
    cleanup();
    
    // Create placeholder algorithms (will be replaced with proper algorithm factory calls)
    m_player1Algorithm = Algorithm::createAlgorithm("chase");
    m_player2Algorithm = Algorithm::createAlgorithm("defensive");
}

void GameManager::cleanup() {
    // Free algorithm resources
    delete m_player1Algorithm;
    m_player1Algorithm = nullptr;
    
    delete m_player2Algorithm;
    m_player2Algorithm = nullptr;
}

void GameManager::createTanksFromBoard() {
  m_tanks.clear();
  bool foundTank1 = false;
  bool foundTank2 = false;
  
  // Scan the board for tank positions
  for (int y = 0; y < m_board.getHeight(); ++y) {
      for (int x = 0; x < m_board.getWidth(); ++x) {
          Point pos(x, y);
          GameBoard::CellType cellType = m_board.getCellType(pos);
          
          if (cellType == GameBoard::CellType::Tank1) {
              // Create tank for player 1 (cannon pointing left)
              m_tanks.emplace_back(1, pos, Direction::Left);
              foundTank1 = true; 
          } else if (cellType == GameBoard::CellType::Tank2) {
              // Create tank for player 2 (cannon pointing right)
              m_tanks.emplace_back(2, pos, Direction::Right);
              foundTank2 = true;
          }
          if (foundTank1 && foundTank2) {
            break; // Both tanks found, exit loop
          }
      }
      if (foundTank1 && foundTank2) {
          break; // Both tanks found, exit loop
      }
  }
}

// TODO: consider output file location
bool GameManager::saveErrorsToFile(const std::vector<std::string>& errors) const {
  // Only create file if there are errors to report
  if (errors.empty()) {
      return true;
  }
  
  // Create input_errors.txt file
  std::ofstream errorFile("input_errors.txt");
  if (!errorFile.is_open()) {
      std::cerr << "Error: Could not create input_errors.txt file" << std::endl;
      return false;
  }
  
  // Write all recoverable errors to the file
  for (const auto& error : errors) {
      errorFile << error << std::endl;
  }
  
  errorFile.close();
  return true;
}