#include "game_manager.h"
#include "file_loader.h"
#include "collision_handler.h"
#include "algo/chase_algorithm.h"
#include "algo/defensive_algorithm.h"
#include <iostream>
#include <fstream>
#include <algorithm>

GameManager::GameManager()
    : m_player1Algorithm(nullptr),
      m_player2Algorithm(nullptr),
      m_currentStep(0),
      m_gameOver(false),
      m_remaining_steps(40),
      m_maximum_steps(1000) {

    #ifdef ENABLE_VISUALIZATION
    m_visualizationManager = createVisualizationManager();
    #endif
}

GameManager::~GameManager() {
    delete m_player1Algorithm;
    delete m_player2Algorithm;

    #ifdef ENABLE_VISUALIZATION
    m_visualizationManager.reset();
    #endif
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

bool GameManager::initialize(const std::string& filePath,
                             Algorithm* player1Algorithm,
                             Algorithm* player2Algorithm) {
    // Load board file
    int boardWidth = 0;
    int boardHeight = 0;
    std::vector<std::string> boardLines = FileLoader::loadBoardFile(filePath, boardWidth, boardHeight);
    
    if (boardLines.empty()) {
        return false;
    }
    
    m_board = GameBoard(boardWidth, boardHeight);
    std::vector<std::string> errors;

    std::vector<std::pair<int, Point>> tankPositions;
    if (!m_board.initialize(boardLines, errors, tankPositions)) {
        // Unrecoverable error in board initialization
        return false;
    }
    if (!saveErrorsToFile(errors)) {
        std::cerr << "Error: Could not save errors to file" << std::endl;
        return false;
    }
    
    createTanks(tankPositions);
    createAlgorithms(player1Algorithm, player2Algorithm);
    return true;
}

void GameManager::runGame() {
  m_currentStep = 0;
  m_gameOver = false;
  m_gameLog.clear();

  #ifdef ENABLE_VISUALIZATION
  if (m_visualizationManager) {
      // Create a message describing current step
      std::string stepMessage = "Game started";
      
      // Capture the game state for visualization
      m_visualizationManager->captureGameState(
          m_currentStep,
          m_board,
          m_tanks,
          m_shells,
          m_remaining_steps,  // Use as countdown if relevant
          stepMessage
      );
  }
  #endif
  
  // Game continues until game over condition is reached
  while (!m_gameOver) {
      // Process a single step of the game
      processStep();
      
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
      
      // Check if the game is over after this step
      m_gameOver = checkGameOver();
  }
  
  // Log the final game result
  m_gameLog.push_back("Game ended after " + std::to_string(m_currentStep) + " steps");
  m_gameLog.push_back("Result: " + m_gameResult);
}

bool GameManager::saveResults(const std::string& outputFilePath) {
  std::ofstream outputFile(outputFilePath);
  if (!outputFile.is_open()) {
      std::cerr << "Error: Could not open output file " << outputFilePath << std::endl;
      return false;
  }
  
  // Write all the game log entries to the file
  for (const auto& logEntry : m_gameLog) {
      outputFile << logEntry << std::endl;
  }
  
  outputFile.close();

  #ifdef ENABLE_VISUALIZATION
  if (m_visualizationManager) {
      // Generate visualization output using same base filename
      std::string visualizationPath = outputFilePath;
      // Remove extension if present
      size_t dotPos = visualizationPath.find_last_of('.');
      if (dotPos != std::string::npos) {
          visualizationPath = visualizationPath.substr(0, dotPos);
      }
      visualizationPath += "_visualization";
      
      if (m_visualizationManager->generateOutputs(visualizationPath)) {
          std::cout << "Visualization generated at " << visualizationPath << ".html" << std::endl;
      } else {
          std::cerr << "Failed to generate visualization." << std::endl;
          return false;
      }
  }
  #endif
  return true;
}

void GameManager::processStep() {
  // Get actions from both player algorithms
  Action player1Action = getPlayerAction(1);
  Action player2Action = getPlayerAction(2);

  // Move shells once
  moveShellsOnce();

  // Check for collisions after first shell movement
  bool tankDestroyed = m_collisionHandler.resolveAllCollisions(m_tanks, m_shells, m_board);

  #ifdef ENABLE_VISUALIZATION
  if (m_visualizationManager) {
      // Create a message describing current step
      std::string midStepMessage = "Step " + std::to_string(m_currentStep) + ": ";
      midStepMessage += "Shells moved once";
      
      // Capture the game state for visualization
      m_visualizationManager->captureGameState(
          m_currentStep,
          m_board,
          m_tanks,
          m_shells,
          m_remaining_steps,  // Use as countdown if relevant
          midStepMessage
      );
  }
  #endif
  
  // Apply tank actions
  bool player1ActionSuccessful = applyAction(1, player1Action);
  bool player2ActionSuccessful = applyAction(2, player2Action);

  // Move shells a second time
  moveShellsOnce();

  // Check for collisions for shell and tank movement
  tankDestroyed = m_collisionHandler.resolveAllCollisions(m_tanks, m_shells, m_board);
  
  removeDestroyedShells();

  // Update cooldowns for all tanks
  for (auto& tank : m_tanks) {
    tank.updateCooldowns();
  }

  #ifdef ENABLE_VISUALIZATION
  if (m_visualizationManager) {
      // Create a message describing current step
      std::string stepMessage = "Step " + std::to_string(m_currentStep) + ": ";
      stepMessage += "P1: " + actionToString(player1Action) + 
                    (player1ActionSuccessful ? " (Success)" : " (Failed)");
      stepMessage += ", P2: " + actionToString(player2Action) + 
                    (player2ActionSuccessful ? " (Success)" : " (Failed)");
      
      // Capture the game state for visualization
      m_visualizationManager->captureGameState(
          m_currentStep,
          m_board,
          m_tanks,
          m_shells,
          m_remaining_steps,  // Use as countdown if relevant
          stepMessage
      );
  }
  #endif
}

Action GameManager::getPlayerAction(int playerId) {
  if (playerId != 1 && playerId != 2) {
    return Action::None;
  }

  // Find the player's tank
  Tank& playerTank = getPlayerTank(playerId);
  Tank& enemyTank = getPlayerTank(playerId == 1 ? 2 : 1);

  // Get the appropriate algorithm
  Algorithm* algorithm = (playerId == 1) ? m_player1Algorithm : m_player2Algorithm;
  if (!algorithm) {
      return Action::None;
  }
  // Get action from algorithm 
  return algorithm->getNextAction(m_board, playerTank, enemyTank, m_shells);
}

Tank& GameManager::getPlayerTank(int playerId) {
  for (auto& tank : m_tanks) {
    if (tank.getPlayerId() == playerId) {
        return tank;
    }
  }
  // If no tank found, return the first tank (should not happen)
  return m_tanks[0];
}

// TODO: add unit tests to cover this
bool GameManager::applyAction(int playerId, Action action) {
  // Validate player ID
  if (playerId != 1 && playerId != 2) {
      return false;
  }
  
  // Find the player's tank
  Tank& playerTank = getPlayerTank(playerId);
  
  if (playerTank.isDestroyed()) {
      return false;
  }
  
  bool actionResult = false;
  
  switch (action) {
      case Action::MoveForward: {
          Point newPosition = playerTank.getNextForwardPosition();
          newPosition = m_board.wrapPosition(newPosition);
          
          if (m_board.canMoveTo(newPosition)) {
              actionResult = playerTank.moveForward(newPosition);
          }
          break;
      }
          
      case Action::MoveBackward: {
          Point newPosition = playerTank.getNextBackwardPosition();
          newPosition = m_board.wrapPosition(newPosition);
          
          if (m_board.canMoveTo(newPosition)) {
              actionResult = playerTank.requestMoveBackward(newPosition);
          }
          break;
      }
              
      case Action::RotateLeftEighth:
          actionResult = playerTank.rotateLeft(false);
          break;
          
      case Action::RotateRightEighth:
          actionResult = playerTank.rotateRight(false);
          break;
          
      case Action::RotateLeftQuarter:
          actionResult = playerTank.rotateLeft(true);
          break;
          
      case Action::RotateRightQuarter:
          actionResult = playerTank.rotateRight(true);
          break;
          
      case Action::Shoot:
          if (playerTank.canShoot()) {
              // Get tank's current position and direction
              Point shellPosition = playerTank.getPosition();
              Direction shellDirection = playerTank.getDirection();
              
              // Create a new shell
              m_shells.emplace_back(playerId, shellPosition, shellDirection);
              
              // Update tank's state (decrement shells and set cooldown)
              actionResult = playerTank.shoot();
          }
          break;
          
      case Action::None:
          actionResult = true;  // No action is always "successful"
          break;
          
      default:
          actionResult = false;
          break;
  }  
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

  if (m_currentStep >= m_maximum_steps) {
      m_gameResult = "Tie - Maximum steps reached";
      return true;
  }
  
  // Game continues
  return false;
}

void GameManager::logAction(int playerId, Action action, bool valid) {
  std::string actionType = actionToString(action);
  std::string status = valid ? "Success" : "Bad Step";
  
  // Format: "Player X: [Action] - [Status]"
  std::string logEntry = "Player " + std::to_string(playerId) + ": " + 
                         actionType + " - " + status;
  
  m_gameLog.push_back(logEntry);
}

// TODO: Consider not having a fallback algorithm
void GameManager::createAlgorithms(Algorithm* player1Algorithm, Algorithm* player2Algorithm) {
  m_player1Algorithm = player1Algorithm ? player1Algorithm : new ChaseAlgorithm();
  m_player2Algorithm = player2Algorithm ? player2Algorithm : new DefensiveAlgorithm();
}

void GameManager::createTanks(std::vector<std::pair<int, Point>> tankPositions) {
  m_tanks.clear();
  for (const auto& tankPos : tankPositions) {
      int playerId = tankPos.first;
      Point position = tankPos.second;
      Direction dir;
      if (playerId == 1) {
          dir = Direction::Left;
      } else  {
          dir = Direction::Right;
      }
      m_tanks.emplace_back(playerId, position, dir);
  }
}

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

void GameManager::removeDestroyedShells() {
  // Use the erase-remove idiom to remove destroyed shells
  m_shells.erase(
      std::remove_if(m_shells.begin(), m_shells.end(),
          [](const Shell& shell) { return shell.isDestroyed(); }),
      m_shells.end()
  );
}