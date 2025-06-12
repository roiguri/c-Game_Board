#include "game_manager.h"
#include "file_loader.h"
#include "collision_handler.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <set>
#include <satellite_view_impl.h>
#include "bonus/logger/logger.h"

GameManager::GameManager(PlayerFactory& playerFactory,
                       TankAlgorithmFactory& tankAlgorithmFactory)
    : m_playerFactory(playerFactory),
      m_tankAlgorithmFactory(tankAlgorithmFactory),
      m_currentStep(0),
      m_gameOver(false),
      m_isClassic2PlayerGame(false),
      m_remaining_steps(DEFAULT_NO_SHELLS_STEPS),
      m_maximum_steps(100) { 
    #ifdef ENABLE_VISUALIZATION
    m_visualizationManager = createVisualizationManager();
    #endif
}

GameManager::~GameManager() {
    #ifdef ENABLE_VISUALIZATION
    m_visualizationManager.reset();
    #endif
}

bool GameManager::readBoard(const std::string& filePath) {
    int rows = 0;
    int cols = 0;
    int maxSteps = 0;
    int numShells = 0;
    std::vector<std::string> boardLines = 
      FileLoader::loadBoardFile(filePath, rows, cols, maxSteps, numShells);
    if (boardLines.empty()) {
        return false;
    }

    m_maximum_steps = maxSteps;
    Tank::setInitialShells(numShells);

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
    
    // Determine unique player IDs from tank positions and create players
    std::set<int> uniquePlayerIds;
    for (const auto& [playerId, position] : tankPositions) {
        uniquePlayerIds.insert(playerId);
    }
    
    for (int playerId : uniquePlayerIds) {
        auto player = m_playerFactory.create(playerId, cols, rows, maxSteps, numShells);
        m_players.push_back({playerId, std::move(player)});
    }
    
    // Determine if this is a classic 2-player game (only players 1 and 2, no others)
    m_isClassic2PlayerGame = (uniquePlayerIds.size() == 2 && 
                             uniquePlayerIds.find(1) != uniquePlayerIds.end() && 
                             uniquePlayerIds.find(2) != uniquePlayerIds.end());
    
    createTanks(tankPositions);
    createTankAlgorithms();

    setOutputFilePath(filePath);

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

  LOG_INFO("Starting game loop");
  
  while (!m_gameOver) {
      LOG_DEBUG("Processing step " + std::to_string(m_currentStep));
      processStep();
      
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
  LOG_INFO("Game ended after " + std::to_string(m_currentStep) + " steps");
  LOG_INFO("Result: " + m_gameResult);

  #ifdef ENABLE_VISUALIZATION
  if (m_visualizationManager) {
      std::string finalMessage = "Game ended after " + std::to_string(m_currentStep) + " steps. Result: " + m_gameResult;
      
      m_visualizationManager->captureGameState(
          m_currentStep + 1,
          m_board,
          m_tanks,
          m_shells,
          m_remaining_steps,
          finalMessage
      );
  }
  #endif

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
          LOG_INFO("Visualization generated at " + visualizationPath + ".html");
      } else {
          LOG_ERROR("Failed to generate visualization.");
          return false;
      }
  }
  #endif
  return true;
}

void GameManager::processStep() {
  m_currentBoard = m_board;
  m_currentTanks = m_tanks;
  m_currentShells = m_shells;

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
      std::string midStepMessage = "Step " + std::to_string(m_currentStep);
      
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

  std::string turnLog = logAction();
  #ifdef ENABLE_VISUALIZATION
  if (m_visualizationManager) {
      std::string stepMessage = "Step " + std::to_string(m_currentStep) + ": " + turnLog;
      
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
           // Ignore GetBattleInfo if tank is in backward movement, but still update state
           if (playerTank.isMovingBackward()) {
              playerTank.doNothing(); // Update counters and state
              actionResult = false;   // Action ignored, but still update state
              break;
           }

           SatelliteViewImpl satteliteView(m_currentBoard, m_currentTanks, m_currentShells, playerTank.getPosition());
            
            // Find the corresponding player
            for (auto& playerWithId : m_players) {
                if (playerWithId.playerId == controller.tank.getPlayerId()) {
                    playerWithId.player->updateTankWithBattleInfo(*controller.algorithm, satteliteView);
                    break;
                }
            }
          }
          actionResult = true;
          break;
      case ActionRequest::DoNothing:
          playerTank.doNothing(); // update tank state
          actionResult = true;    // No action is always "successful"
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

bool GameManager::checkGameOver() {
    // Count alive tanks per player
    std::unordered_map<int, int> playersAlive;
    for (const auto& tank : m_tanks) {
        if (!tank.isDestroyed()) {
            playersAlive[tank.getPlayerId()]++;
        }
    }

    // Count how many players have tanks alive
    int playersWithTanks = 0;
    int winningPlayer = -1;
    int winningPlayerTanks = 0;
    
    for (const auto& [playerId, tankCount] : playersAlive) {
        if (tankCount > 0) {
            playersWithTanks++;
            winningPlayer = playerId;
            winningPlayerTanks = tankCount;
        }
    }

    // Check win conditions
    if (playersWithTanks == 1) {
        m_gameResult = "Player " + std::to_string(winningPlayer) + " won with " + std::to_string(winningPlayerTanks) + " tanks still alive";
        return true;
    }
    if (playersWithTanks == 0) {
        if (m_isClassic2PlayerGame) {
            m_gameResult = "Tie, both players have zero tanks";
        } else {
            m_gameResult = "Tie, all players have zero tanks";
        }
        return true;
    }
    if (m_remaining_steps <= 0) {
        if (m_isClassic2PlayerGame) {
            m_gameResult = "Tie, both players have zero shells for " + std::to_string(DEFAULT_NO_SHELLS_STEPS) + " steps";
        } else {
            m_gameResult = "Tie, all players have zero shells for " + std::to_string(DEFAULT_NO_SHELLS_STEPS) + " steps";
        }
        return true;
    }
    if (m_currentStep >= m_maximum_steps) {
        if (m_isClassic2PlayerGame) {
            // Preserve original 2-player format
            int player1Tanks = 0, player2Tanks = 0;
            for (const auto& [playerId, tankCount] : playersAlive) {
                if (playerId == 1) player1Tanks = tankCount;
                else if (playerId == 2) player2Tanks = tankCount;
            }
            m_gameResult = "Tie, reached max steps = " + std::to_string(m_maximum_steps) + 
                          ", player 1 has " + std::to_string(player1Tanks) + 
                          " tanks, player 2 has " + std::to_string(player2Tanks) + " tanks";
        } else {
            // Multi-player format
            std::string resultStr = "Tie, reached max steps = " + std::to_string(m_maximum_steps);
            for (const auto& [playerId, tankCount] : playersAlive) {
                if (tankCount > 0) {
                    resultStr += ", player " + std::to_string(playerId) + " has " + std::to_string(tankCount) + " tanks";
                }
            }
            m_gameResult = resultStr;
        }
        return true;
    }
    // Game continues
    return false;
}

std::string GameManager::logAction() {
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

    // Debug log for all tanks actions with their ids
    std::string debugInfo = "Step " + std::to_string(m_currentStep) + " actions: ";
    for (size_t i = 0; i < m_tankControllers.size(); ++i) {
        auto& controller = m_tankControllers[i];
        int playerId = controller.tank.getPlayerId();
        
        if (i > 0) debugInfo += ", ";
        debugInfo += "P" + std::to_string(playerId) + 
                     "-T" + std::to_string(i) + 
                     " @(" + std::to_string(controller.tank.getPosition().getX()) + "," + std::to_string(controller.tank.getPosition().getY()) + ")" +
                     "-" + directionToString(controller.tank.getDirection()) + 
                     ": " + actionToString(controller.nextAction) +
                     (controller.actionSuccess ? "" : " (ignored)") +
                     (controller.tank.isDestroyed() ? " (killed)" : "");
    }
    LOG_DEBUG(debugInfo);
    return turnLog;
}

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

Direction GameManager::getInitialDirection(int playerId) {
    const Direction directions[] = {
        Direction::Left,        // Player 1
        Direction::Right,       // Player 2  
        Direction::Up,          // Player 3
        Direction::Down,        // Player 4
        Direction::DownLeft,   // Player 5
        Direction::DownRight,   // Player 6
        Direction::UpLeft,   // Player 7
        Direction::UpRight,   // Player 8
        Direction::Left         // Player 9 (wrap around)
    };
    
    if (playerId >= 1 && playerId <= 9) {
        return directions[playerId - 1];
    }
    return Direction::Left; // Default fallback
}

void GameManager::createTanks(const std::vector<std::pair<int, Point>>& tankPositions) {
    LOG_INFO("Creating tanks");
    m_tanks.clear();
    for (const auto& [playerId, position] : tankPositions) {
        Direction dir = getInitialDirection(playerId);
        m_tanks.emplace_back(playerId, position, dir);
    }
    LOG_INFO("Tanks created");
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
    LOG_INFO("Creating tank algorithms");
    // Map from playerId to current tank index for that player
    std::unordered_map<int, int> playerTankCounts;
    for (auto& tank : m_tanks) {
        int playerId = tank.getPlayerId();
        int tankIndex = playerTankCounts[playerId]++;
        auto algo = m_tankAlgorithmFactory.create(playerId, tankIndex);
        m_tankControllers.push_back(TankWithAlgorithm{tank, std::move(algo)});
    }
    LOG_INFO("Tank algorithms created");
}

void GameManager::setOutputFilePath(const std::string& inputFilePath) {
    std::filesystem::path inputPath(inputFilePath);
    std::string directory = inputPath.parent_path().string();
    std::string filename = inputPath.filename().string();
    
    if (directory.empty()) {
        m_outputFilePath = "output_" + filename;
    } else {
        m_outputFilePath = directory + "/output_" + filename;
    }
}