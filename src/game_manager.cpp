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
#include <satellite_view_impl.h>

GameManager::GameManager(PlayerFactory& playerFactory,
                       TankAlgorithmFactory& tankAlgorithmFactory)
    : m_PlayerFactory(playerFactory),
      m_tankAlgorithmFactory(tankAlgorithmFactory),
      m_currentStep(0),
      m_gameOver(false),
      m_remaining_steps(40),
      m_maximum_steps(1000) { // TODO: update maximum steps    
    #ifdef ENABLE_VISUALIZATION
    m_visualizationManager = createVisualizationManager();
    #endif
}

GameManager::~GameManager() {
    #ifdef ENABLE_VISUALIZATION
    m_visualizationManager.reset();
    #endif
}

// TODO: add handling when no tanks are found
// TODO: move algorithms from readBoard to constructor
bool GameManager::readBoard(const std::string& filePath) {
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

    m_player1 = m_PlayerFactory.create(1, cols, rows, maxSteps, numShells);
    m_player2 = m_PlayerFactory.create(2, cols, rows, maxSteps, numShells);
    
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
  m_currentStep = 1;
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
      
      // TODO: optimize to store out of shells state
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
      m_currentStep++;
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
  // TODO: Consider creating a GameState abstraction to encapsulate board, tanks, and shells
  GameBoard m_currentBoard = m_board;
  std::vector<Tank> m_currentTanks = m_tanks;
  std::vector<Shell> m_currentShells = m_shells;

  for (auto& controller : m_tankControllers) {
    if (!controller.tank.isDestroyed() && controller.algorithm) {
      controller.nextAction = controller.algorithm->getAction();
    } else {
      controller.nextAction = ActionRequest::DoNothing;
    }
  }

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
  
  // TODO: fix log actions
  for (auto& controller : m_tankControllers) {
    if (!controller.tank.isDestroyed() && controller.algorithm) {
      applyAction(controller);
    }
  }

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

  logAction();
}

void GameManager::applyAction(TankWithAlgorithm& controller) {
  if (controller.tank.isDestroyed()) {
      return;
  }

  int playerId = controller.tank.getPlayerId();
  Tank& playerTank = controller.tank;
  
  if (playerTank.isDestroyed()) {
      return;
  }
  
  bool actionResult = false;
  
  switch (controller.nextAction) {
      case ActionRequest::MoveForward: {
          Point newPosition = playerTank.getNextForwardPosition();
          newPosition = m_board.wrapPosition(newPosition);
          
          if (m_board.canMoveTo(newPosition)) {
              actionResult = playerTank.moveForward(newPosition);
              std::cout << "actionResult: " << actionResult << std::endl;
          }
          break;
      }
          
      case ActionRequest::MoveBackward: {
          Point newPosition = playerTank.getNextBackwardPosition();
          newPosition = m_board.wrapPosition(newPosition);
          
          if (m_board.canMoveTo(newPosition)) {
              actionResult = playerTank.requestMoveBackward(newPosition);
          }
          break;
      }
              
      case ActionRequest::RotateLeft45:
          actionResult = playerTank.rotateLeft(false);
          break;
          
      case ActionRequest::RotateRight45:
          actionResult = playerTank.rotateRight(false);
          break;
          
      case ActionRequest::RotateLeft90:
          actionResult = playerTank.rotateLeft(true);
          break;
          
      case ActionRequest::RotateRight90:
          actionResult = playerTank.rotateRight(true);
          break;
          
      case ActionRequest::Shoot:
          if (playerTank.canShoot()) {
              Point shellPosition = playerTank.getPosition();
              Direction shellDirection = playerTank.getDirection();
              
              m_shells.emplace_back(playerId, shellPosition, shellDirection);
              
              actionResult = playerTank.shoot();
          }
          break;
      case ActionRequest::GetBattleInfo: {
            SatelliteViewImpl satteliteView(m_currentBoard, m_currentTanks, m_currentShells, playerTank.getPosition());
            if (controller.tank.getPlayerId() == 1) {
                m_player1->updateTankWithBattleInfo(*controller.algorithm, satteliteView);
            } else {
                m_player2->updateTankWithBattleInfo(*controller.algorithm, satteliteView);
            }
          }
          break;
      case ActionRequest::DoNothing:
          playerTank.doNothing(); // update tank state
          actionResult = true;  // No action is always "successful"
          break;
      default:
          actionResult = false;
          break;
  }
  controller.actionSuccess = actionResult;
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

/**
 * @brief Checks if the game is over and sets m_gameResult accordingly.
 *
 * Last line in file will announce the winner in the following format:
 *   Player <X> won with <Y> tanks still alive
 * Or:
 *   Tie, both players have zero tanks
 * Or:
 *   Tie, reached max steps = <max_steps>, player 1 has <X> tanks, player 2 has <Y> tanks
 */
bool GameManager::checkGameOver() {
    int player1Alive = 0;
    int player2Alive = 0;
    for (const auto& tank : m_tanks) {
        if (!tank.isDestroyed()) {
            if (tank.getPlayerId() == 1) player1Alive++;
            else if (tank.getPlayerId() == 2) player2Alive++;
        }
    }

    if (player1Alive > 0 && player2Alive == 0) {
        m_gameResult = "Player 1 won with " + std::to_string(player1Alive) + " tanks still alive";
        return true;
    }
    if (player2Alive > 0 && player1Alive == 0) {
        m_gameResult = "Player 2 won with " + std::to_string(player2Alive) + " tanks still alive";
        return true;
    }
    if (player1Alive == 0 && player2Alive == 0) {
        m_gameResult = "Tie, both players have zero tanks";
        return true;
    }
    if (m_currentStep >= m_maximum_steps) {
        m_gameResult = "Tie, reached max steps = " + std::to_string(m_maximum_steps) + ", player 1 has " + std::to_string(player1Alive) + " tanks, player 2 has " + std::to_string(player2Alive) + " tanks";
        return true;
    }
    // Game continues
    return false;
}

void GameManager::logAction() {
    std::vector<std::string> stepActions;
    for (auto& controller : m_tankControllers) {
        std::string actionStr;
        if (controller.tank.isDestroyed() && !controller.wasKilledInPreviousStep) {
            actionStr = actionToString(controller.nextAction) + " (killed)";
            controller.wasKilledInPreviousStep = true;
        } else if (controller.tank.isDestroyed()) {
            actionStr = "Killed";
        } else {
            actionStr = actionToString(controller.nextAction);
            if (!controller.actionSuccess) actionStr += " (ignored)";
        }
        stepActions.push_back(actionStr);
    }
    std::string turnLog;
    for (size_t i = 0; i < stepActions.size(); ++i) {
        if (i > 0) turnLog += ", ";
        turnLog += stepActions[i];
    }
    m_gameLog.push_back(turnLog);
}

// Helper to convert ActionRequest to string for logging
std::string GameManager::actionToString(ActionRequest action) {
    switch (action) {
        case ActionRequest::MoveForward: return "MoveForward";
        case ActionRequest::MoveBackward: return "MoveBackward";
        case ActionRequest::RotateLeft45: return "RotateLeft45";
        case ActionRequest::RotateRight45: return "RotateRight45";
        case ActionRequest::RotateLeft90: return "RotateLeft90";
        case ActionRequest::RotateRight90: return "RotateRight90";
        case ActionRequest::Shoot: return "Shoot";
        case ActionRequest::GetBattleInfo: return "GetBattleInfo";
        case ActionRequest::DoNothing: return "DoNothing";
        default: return "Unknown";
    }
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
    // Map from playerId to current tank index for that player
    std::unordered_map<int, int> playerTankCounts;
    for (auto& tank : m_tanks) {
        int playerId = tank.getPlayerId();
        int tankIndex = playerTankCounts[playerId]++;
        auto algo = m_tankAlgorithmFactory.create(playerId, tankIndex);
        m_tankControllers.push_back(TankWithAlgorithm{tank, std::move(algo)});
    }
}