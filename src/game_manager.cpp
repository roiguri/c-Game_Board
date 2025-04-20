#include "game_manager.h"
#include "file_loader.h"
#include "collision_handler.h"
#include <iostream>
#include <fstream>

GameManager::GameManager()
    : m_player1Algorithm(nullptr),
      m_player2Algorithm(nullptr),
      m_currentStep(0),
      m_gameOver(false),
      m_remaining_steps(40) {
    // Initialize empty game state
}

GameManager::~GameManager() {
    // Clean up resources
    cleanup();
}

// Accessors // TODO - remove the accessors
const std::vector<std::string>& GameManager::getGameLog() const {
    return m_gameLog;
}
std::vector<Tank> GameManager::getTanks() const {
    return m_tanks;
}
std::vector<Shell> GameManager::getShells() const {
    return m_shells;
}
GameBoard GameManager::getGameBoard() const {
    return m_board;
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
    createAlgorithms();
    return true;
}

void GameManager::runGame() {
  m_currentStep = 0;
  m_gameOver = false;
  m_gameLog.clear();
  
  // Game continues until game over condition is reached
  while (!m_gameOver) {
      // Process a single step of the game
      processStep();
      
      // Check if the game is over after this step
      m_gameOver = checkGameOver();
      
      // Increment step counter
      m_currentStep++;
      
      // Add a step separator to log
      m_gameLog.push_back("Step " + std::to_string(m_currentStep) + " completed");
      
      // Check for maximum step count (if both tanks are out of shells)
      bool bothOutOfShells = true;
      for (const auto& tank : m_tanks) {
          if (!tank.isDestroyed() && tank.getRemainingShells() > 0) {
              bothOutOfShells = false;
              break;
          }
      }
      
      if (bothOutOfShells) {
          m_remaining_steps--;
      }
      if (m_remaining_steps < 0) {
        m_gameOver = true;
        m_gameResult = "Tie - Both tanks out of shells";
    }
  }
  
  // Log the final game result
  m_gameLog.push_back("Game ended after " + std::to_string(m_currentStep) + " steps"); // TODO: remove if necessary
  m_gameLog.push_back("Result: " + m_gameResult);
}

// TODO: consider changing to boolean
void GameManager::saveResults(const std::string& outputFilePath) {
  std::ofstream outputFile(outputFilePath);
  if (!outputFile.is_open()) {
      std::cerr << "Error: Could not open output file " << outputFilePath << std::endl;
      return;
  }
  
  // Write all the game log entries to the file
  for (const auto& logEntry : m_gameLog) {
      outputFile << logEntry << std::endl;
  }
  
  outputFile.close();
}

void GameManager::processStep() {
  // Get actions from both player algorithms
  Action player1Action = getPlayerAction(1);
  Action player2Action = getPlayerAction(2);

  // Move shells once
  moveShellsOnce();

  // Check for collisions after first shell movement
  bool tankDestroyed = m_collisionHandler.resolveAllCollisions(m_tanks, m_shells, m_board);
  
  // Apply tank actions
  bool player1ActionSuccessful = applyAction(1, player1Action);
  bool player2ActionSuccessful = applyAction(2, player2Action);

  // Move shells a second time
  moveShellsOnce();

  // Check for collisions for shell and tank movement
  tankDestroyed = m_collisionHandler.resolveAllCollisions(m_tanks, m_shells, m_board);
  

  // Update cooldowns for all tanks
  for (auto& tank : m_tanks) {
    tank.updateCooldowns();
  }

  // TODO: update game state - remove shells, move tanks in board etc.
  // Game over check will be performed in the main game loop
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
  // TODO: check if pointer okay here
  return algorithm->getNextAction(m_board, *playerTank, *enemyTank, m_shells);
}

// TODO: add unit tests to cover this
bool GameManager::applyAction(int playerId, Action action) {
  // Validate player ID
  if (playerId != 1 && playerId != 2) {
      return false;
  }
  
  // Find the player's tank
  Tank* playerTank = nullptr;
  for (auto& tank : m_tanks) {
      if (tank.getPlayerId() == playerId) {
          playerTank = &tank;
          break;
      }
  }
  
  // Check tank exists and is not destroyed
  if (playerTank == nullptr || playerTank->isDestroyed()) {
      return false;
  }
  
  bool actionResult = false;
  
  // Execute action based on type
  switch (action) {
      case Action::MoveForward: {
          Point newPosition = playerTank->getNextForwardPosition();
          newPosition = m_board.wrapPosition(newPosition);
          
          if (m_board.canMoveTo(newPosition)) {
              actionResult = playerTank->moveForward(newPosition);
          }
          break;
      }
          
      case Action::MoveBackward: {
          Point newPosition = playerTank->getNextBackwardPosition();
          newPosition = m_board.wrapPosition(newPosition);
          
          // TODO: consider checking only when actually applying movement
          if (m_board.canMoveTo(newPosition)) {
              actionResult = playerTank->requestMoveBackward(newPosition);
          }
          break;
      }
              
      case Action::RotateLeftEighth:
          actionResult = playerTank->rotateLeft(false);
          break;
          
      case Action::RotateRightEighth:
          actionResult = playerTank->rotateRight(false);
          break;
          
      case Action::RotateLeftQuarter:
          actionResult = playerTank->rotateLeft(true);
          break;
          
      case Action::RotateRightQuarter:
          actionResult = playerTank->rotateRight(true);
          break;
          
      case Action::Shoot:
          if (playerTank->canShoot()) {
              // Get tank's current position and direction
              Point shellPosition = playerTank->getPosition();
              Direction shellDirection = playerTank->getDirection();
              
              // TODO: consider creating shells in the next cell.
              // Create a new shell
              m_shells.emplace_back(playerId, shellPosition, shellDirection);
              
              // Update tank's state (decrement shells and set cooldown)
              actionResult = playerTank->shoot();
          }
          break;
          
      case Action::None:
          actionResult = true;  // No action is always "successful"
          break;
          
      default:
          actionResult = false;
          break;
  }
  
  // Log the action and its result
  logAction(playerId, action, actionResult);
  
  return actionResult;
}

void GameManager::moveShellsOnce() {
  for (auto& shell : m_shells) {
      // Skip destroyed shells
      if (shell.isDestroyed()) {
          continue;
      }
      
      // Calculate new position based on direction
      Point newPosition = shell.getPosition() + getDirectionDelta(shell.getDirection());
      
      // Handle board wrapping
      newPosition = m_board.wrapPosition(newPosition);
      
      // Update shell position
      shell.setPosition(newPosition);
  }
}

bool GameManager::checkGameOver() {
  // Check if any tank was destroyed
  int destroyedTankCount = 0;
  int playerWinner = 0;
  
  for (const auto& tank : m_tanks) {
      if (tank.isDestroyed()) {
          destroyedTankCount++;
          // If only one tank is destroyed, the other player wins
          if (destroyedTankCount == 1) {
              playerWinner = (tank.getPlayerId() == 1) ? 2 : 1;
          }
      }
  }

  if (destroyedTankCount == 1) {
    m_gameResult = "Player " + std::to_string(playerWinner) + " wins - Enemy tank destroyed";
    return true;
  }
  
  // If both tanks are destroyed, it's a tie
  if (destroyedTankCount == 2) {
      m_gameResult = "Tie - Both tanks destroyed";
      return true;
  }
  
  // If all shells are used and additional steps have passed, it's a tie
  if (m_remaining_steps < 0) {
      m_gameResult = "Tie - Maximum steps reached after shells depleted";
      return true;
  }
  
  // Game continues
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
    m_player1Algorithm = Algorithm::createAlgorithm("chase");
    m_player2Algorithm = Algorithm::createAlgorithm("defensive");
}

// TODO: when do we call clean up at the end of the run
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