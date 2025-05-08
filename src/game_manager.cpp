#include "game_manager.h"
#include "file_loader.h"
#include "collision_handler.h"
#include "algo/chase_algorithm.h"
#include "algo/defensive_algorithm.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

GameManager::GameManager(std::unique_ptr<PlayerFactory> playerFactory,
                       std::unique_ptr<TankAlgorithmFactory> tankAlgorithmFactory)
    : m_player1Algorithm(nullptr), // FIXME: remove
      m_player2Algorithm(nullptr), // FIXME: remove
      m_PlayerFactory(std::move(playerFactory)),
      m_tankAlgorithmFactory(std::move(tankAlgorithmFactory)),
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

// TODO: add handling when no tanks are found
// TODO: move algorithms from readBoard to constructor
bool GameManager::readBoard(const std::string& filePath,
                             Algorithm* player1Algorithm,
                             Algorithm* player2Algorithm) {
    // TODO: consider creating a struct to store game configuration
    int rows = 0;
    int cols = 0;
    int maxSteps = 0;
    int numShells = 0;
    std::vector<std::string> boardLines = 
      FileLoader::loadBoardFile(filePath, rows, cols, maxSteps, numShells);
    
    if (boardLines.empty()) {
        return false;
    }
    
    m_board = GameBoard(cols, rows);
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
    createAlgorithms(player1Algorithm, player2Algorithm); // FIXME: remove

    createTankAlgorithms();

    // TODO: extract to private helper function
    // Set output file path based on input file path
    std::filesystem::path inputPath(filePath);
    std::string directory = inputPath.parent_path().string();
    std::string filename = inputPath.filename().string();
    if (directory.empty()) {
        m_outputFilePath = "output_" + filename;
    } else {
        m_outputFilePath = directory + "/output_" + filename;
    }

    return true;
}

void GameManager::run() {
  m_currentStep = 0;
  m_gameOver = false;
  m_gameLog.clear();

  #ifdef ENABLE_VISUALIZATION
  if (m_visualizationManager) {
      std::string stepMessage = "Game started";
      
      // Capture the game state for visualization
      m_visualizationManager->captureGameState(
          m_currentStep,
          m_board,
          m_tanks,
          m_shells,
          m_remaining_steps,
          stepMessage
      );
  }
  #endif
  
  // TODO: update log structure
  while (!m_gameOver) {
      processStep();
      m_currentStep++;
      
      // Add a step separator to log
      m_gameLog.push_back(
        "Step " + std::to_string(m_currentStep) + " completed"
      );

      bool tanksOutOfShells = true;
      for (const auto& tank : m_tanks) {
          if (!tank.isDestroyed() && tank.getRemainingShells() > 0) {
              tanksOutOfShells = false;
              break;
          }
      }
      
      if (tanksOutOfShells) {
          m_remaining_steps--;
      }
      m_gameOver = checkGameOver();
  }
  
  m_gameLog.push_back(
    "Game ended after " + std::to_string(m_currentStep) + " steps"
  );
  m_gameLog.push_back("Result: " + m_gameResult);

  // Call saveResults with the stored output file path
  saveResults(m_outputFilePath);
}

bool GameManager::saveResults(const std::string& outputFilePath) {
  std::ofstream outputFile(outputFilePath);
  if (!outputFile.is_open()) {
      std::cerr << "Error: Could not open output file " << 
        outputFilePath << std::endl;
      return false;
  }
  for (const auto& logEntry : m_gameLog) {
      outputFile << logEntry << std::endl;
  }
  
  outputFile.close();

  #ifdef ENABLE_VISUALIZATION
  if (m_visualizationManager) {
      std::string visualizationPath = outputFilePath;
      size_t dotPos = visualizationPath.find_last_of('.');
      if (dotPos != std::string::npos) {
          visualizationPath = visualizationPath.substr(0, dotPos);
      }
      visualizationPath += "_visualization";
      
      if (m_visualizationManager->generateOutputs(visualizationPath)) {
          std::cout << "Visualization generated at " << visualizationPath <<
           ".html" << std::endl;
      } else {
          std::cerr << "Failed to generate visualization." << std::endl;
          return false;
      }
  }
  #endif
  return true;
}

void GameManager::processStep() {
  Action player1Action = getPlayerAction(1);
  Action player2Action = getPlayerAction(2);

  moveShellsOnce();

  m_collisionHandler.resolveAllCollisions(
    m_tanks, m_shells, m_board
  );

  #ifdef ENABLE_VISUALIZATION
  if (m_visualizationManager) {
      std::string midStepMessage = "Step " + std::to_string(m_currentStep)+": ";
      midStepMessage += "Shells moved once";
      
      m_visualizationManager->captureGameState(
          m_currentStep,
          m_board,
          m_tanks,
          m_shells,
          m_remaining_steps, 
          midStepMessage
      );
  }
  #endif
  
  applyAction(1, player1Action);
  applyAction(2, player2Action);

  moveShellsOnce();

  m_collisionHandler.resolveAllCollisions(
    m_tanks, m_shells, m_board
  );
  
  removeDestroyedShells();

  for (auto& tank : m_tanks) {
    tank.updateCooldowns();
  }

  #ifdef ENABLE_VISUALIZATION
  if (m_visualizationManager) {
      std::string stepMessage = "Step " + std::to_string(m_currentStep) + ": ";
      stepMessage += "P1: " + actionToString(player1Action);
      stepMessage += ", P2: " + actionToString(player2Action);
      
      m_visualizationManager->captureGameState(
          m_currentStep,
          m_board,
          m_tanks,
          m_shells,
          m_remaining_steps,
          stepMessage
      );
  }
  #endif
}

Action GameManager::getPlayerAction(int playerId) {
  if (playerId != 1 && playerId != 2) {
    return Action::None;
  }

  Tank& playerTank = getPlayerTank(playerId);
  Tank& enemyTank = getPlayerTank(playerId == 1 ? 2 : 1);

  Algorithm* algorithm = (playerId == 1) ? 
                         m_player1Algorithm : m_player2Algorithm;
  if (!algorithm) {
      return Action::None;
  }
  return algorithm->getNextAction(m_board, playerTank, enemyTank, m_shells);
}

Tank& GameManager::getPlayerTank(int playerId) {
  for (auto& tank : m_tanks) {
    if (tank.getPlayerId() == playerId) {
        return tank;
    }
  }
  return m_tanks[0];
}

void GameManager::applyAction(int playerId, Action action) {
  if (playerId != 1 && playerId != 2) {
      return;
  }
  
  Tank& playerTank = getPlayerTank(playerId);
  
  if (playerTank.isDestroyed()) {
      return;
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
              Point shellPosition = playerTank.getPosition();
              Direction shellDirection = playerTank.getDirection();
              
              m_shells.emplace_back(playerId, shellPosition, shellDirection);
              
              actionResult = playerTank.shoot();
          }
          break;
          
      case Action::None:
          playerTank.doNothing(); // update tank state
          actionResult = true;  // No action is always "successful"
          break;
          
      default:
          actionResult = false;
          break;
  }  
  logAction(playerId, action, actionResult);
}

void GameManager::moveShellsOnce() {
  for (auto& shell : m_shells) {
      if (shell.isDestroyed()) {
          continue;
      }
      
      Point newPosition = 
        shell.getPosition() + getDirectionDelta(shell.getDirection());
      
      newPosition = m_board.wrapPosition(newPosition);
      shell.setPosition(newPosition);
  }
}

// TODO: fix log
bool GameManager::checkGameOver() {
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
    m_gameResult = 
      "Player " + std::to_string(playerWinner) + " wins - Enemy tank destroyed";
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

void GameManager::createAlgorithms(
  Algorithm* player1Algorithm, 
  Algorithm* player2Algorithm
) {
  m_player1Algorithm = player1Algorithm ? 
                        player1Algorithm : new ChaseAlgorithm();
  m_player2Algorithm = player2Algorithm ? 
                        player2Algorithm : new DefensiveAlgorithm();
}

// TODO: consider adding sequence number to tank object members
void GameManager::createTanks(const std::vector<std::pair<int, Point>>& tankPositions) {
    m_tanks.clear();
    for (const auto& [playerId, position] : tankPositions) {
        Direction dir = (playerId == 1) ? Direction::Left : Direction::Right;
        m_tanks.emplace_back(playerId, position, dir);
    }
}

bool GameManager::saveErrorsToFile(
  const std::vector<std::string>& errors
) const {
  if (errors.empty()) {
      // No errors to report
      return true;
  }
  
  std::ofstream errorFile("input_errors.txt");
  if (!errorFile.is_open()) {
      std::cerr << "Error: Could not create input_errors.txt file" << std::endl;
      return false;
  }
  
  for (const auto& error : errors) {
      errorFile << error << std::endl;
  }
  
  errorFile.close();
  return true;
}

void GameManager::removeDestroyedShells() {
  m_shells.erase(
      std::remove_if(m_shells.begin(), m_shells.end(),
          [](const Shell& shell) { return shell.isDestroyed(); }),
      m_shells.end()
  );
}

void GameManager::createTankAlgorithms() {
    m_tankAlgorithms.clear();
    // Map from playerId to current tank index for that player
    std::unordered_map<int, int> playerTankCounts;
    for (const auto& tank : m_tanks) {
        int playerId = tank.getPlayerId();
        int tankIndex = playerTankCounts[playerId]++;
        m_tankAlgorithms.push_back(
            m_tankAlgorithmFactory->create(playerId, tankIndex)
        );
    }
}