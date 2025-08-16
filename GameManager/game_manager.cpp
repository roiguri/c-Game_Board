#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <set>
#include <typeinfo>
#include <unordered_map>

#include "collision_handler.h"
#include "game_manager.h"
#include "satellite_view_impl.h"
#include "bonus/logger/logger.h"
#include "common/GameManagerRegistration.h"

namespace GameManager_098765432_123456789 {

using namespace UserCommon_098765432_123456789;

GameManager::GameManager(bool verbose)
    : m_currentStep(0),
      m_gameOver(false),
      m_isClassic2PlayerGame(false),
      m_remaining_steps(DEFAULT_NO_SHELLS_STEPS),
      m_maximum_steps(100),
      m_verbose(verbose) {
    // This constructor is for registration system only
    // Actual factories will be provided through run() method
    #ifdef ENABLE_VISUALIZATION
    if (verbose) {
        m_visualizationManager = createVisualizationManager();
    }
    #endif
}


GameManager::~GameManager() {
    #ifdef ENABLE_VISUALIZATION
    m_visualizationManager.reset();
    #endif
}

bool GameManager::readBoard(const SatelliteView& satellite_view, size_t map_width, size_t map_height, 
                           size_t max_steps, size_t num_shells,
                           TankAlgorithmFactory player1_factory, TankAlgorithmFactory player2_factory) {
    std::vector<std::string> boardLines = 
      readSatelliteView(satellite_view, map_width, map_height);
    if (boardLines.empty()) {
        return false;
    }

    m_maximum_steps = max_steps;
    Tank::setInitialShells(num_shells);

    m_board = GameBoard(map_width, map_height);

    std::vector<std::pair<int, Point>> tankPositions;
    if (!m_board.initialize(boardLines, tankPositions)) {
        // Unrecoverable error in board initialization
        return false;
    }
    
    // TODO: consider support for multi-player games
    m_isClassic2PlayerGame = true;
    
    createTanks(tankPositions);
    createTankAlgorithms(player1_factory, player2_factory);

    return true;
}

GameResult GameManager::run(
        size_t map_width, size_t map_height,
        const SatelliteView& map, // <= assume it is a snapshot, NOT updated
        string map_name,
        size_t max_steps, size_t num_shells,
        Player& player1, string name1,
        Player& player2, string name2,
        TankAlgorithmFactory player1_tank_algo_factory,
        TankAlgorithmFactory player2_tank_algo_factory) {

    // Store names for output file generation
    m_mapName = map_name;
    m_player1Name = extractBaseName(name1);
    m_player2Name = extractBaseName(name2);

    // Store references to the provided players
    m_players.push_back(PlayerWithId(1, player1));
    m_players.push_back(PlayerWithId(2, player2));
    
    // TODO: figure out when (and if) we have unrecoverable errors
    // Initialize game using readBoard method
    if (!readBoard(map, map_width, map_height, max_steps, num_shells, player1_tank_algo_factory, player2_tank_algo_factory)) {
        // Return error result if board initialization failed
        GameResult errorResult;
        errorResult.winner = 0; // tie
        errorResult.reason = GameResult::Reason::ALL_TANKS_DEAD;
        errorResult.remaining_tanks = {0, 0};
        return errorResult;
    }
 
    // Start Game:
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

    if (m_verbose) {
        saveResults();
    }
    return std::move(m_finalGameResult);
}

bool GameManager::saveResults() {
    std::string player1Name = cleanFilename(m_player1Name);
    std::string player2Name = cleanFilename(m_player2Name);
    
    // Generate unique output file path
    std::string outputFilePath = generateOutputFilePath(player1Name, player2Name);
    
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
                    playerWithId.player.updateTankWithBattleInfo(*controller.algorithm, satteliteView);
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
    std::set<int> playerIds; // TODO: move this to a class member
    // Count alive tanks per player
    std::unordered_map<int, int> playersAlive;
    for (const auto& tank : m_tanks) {
        playerIds.insert(tank.getPlayerId());
        if (!tank.isDestroyed()) {
            playersAlive[tank.getPlayerId()]++;
        }
    }

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
    
    std::vector<size_t> remainingTanks(playerIds.size(), 0);
    for (const auto& [playerId, tankCount] : playersAlive) {
        if (playerId > 0) {
            remainingTanks[playerId - 1] = tankCount; // Convert to 0-based index
        }
    }
    // TODO: verify current step is correct in GameResult
    // Check win conditions
    if (playersWithTanks == 1) {
        m_gameResult = "Player " + std::to_string(winningPlayer) + " won with " + std::to_string(winningPlayerTanks) + " tanks still alive";
        populateGameResult(winningPlayer, GameResult::ALL_TANKS_DEAD, remainingTanks);
        return true;
    }
    if (playersWithTanks == 0) {
        if (m_isClassic2PlayerGame) {
            m_gameResult = "Tie, both players have zero tanks";
        } else {
            m_gameResult = "Tie, all players have zero tanks";
        }
        populateGameResult(0, GameResult::ALL_TANKS_DEAD, remainingTanks);
        return true;
    }
    if (m_remaining_steps <= 0) {
        if (m_isClassic2PlayerGame) {
            m_gameResult = "Tie, both players have zero shells for " + std::to_string(DEFAULT_NO_SHELLS_STEPS) + " steps";
        } else {
            m_gameResult = "Tie, all players have zero shells for " + std::to_string(DEFAULT_NO_SHELLS_STEPS) + " steps";
        }
        populateGameResult(0, GameResult::ZERO_SHELLS, remainingTanks);
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
        populateGameResult(0, GameResult::MAX_STEPS, remainingTanks);
        return true;
    }
    // Game continues
    return false;
}

void GameManager::populateGameResult(int winner, GameResult::Reason reason, const std::vector<size_t>& remainingTanks) {
    m_finalGameResult.winner = winner;
    m_finalGameResult.reason = reason;
    m_finalGameResult.remaining_tanks = remainingTanks;
    m_finalGameResult.rounds = m_currentStep;
    m_finalGameResult.gameState = std::make_unique<SatelliteViewImpl>(m_board, m_tanks, m_shells);
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

void GameManager::removeDestroyedShells() {
  m_shells.erase(
      std::remove_if(m_shells.begin(), m_shells.end(),
          [](const Shell& shell) { return shell.isDestroyed(); }),
      m_shells.end()
  );
}

void GameManager::createTankAlgorithms(TankAlgorithmFactory player1_factory, TankAlgorithmFactory player2_factory) {
    LOG_INFO("Creating tank algorithms");
    // Map from playerId to current tank index for that player
    std::unordered_map<int, int> playerTankCounts;
    for (auto& tank : m_tanks) {
        int playerId = tank.getPlayerId();
        int tankIndex = playerTankCounts[playerId]++;
        
        // Use the appropriate factory based on player ID
        std::unique_ptr<TankAlgorithm> algo;
        if (playerId == 1) {
            algo = player1_factory(playerId, tankIndex);
        } else if (playerId == 2) {
            algo = player2_factory(playerId, tankIndex);
        } else {
            LOG_ERROR("Invalid player ID: " + std::to_string(playerId));
            continue;
        }
        
        m_tankControllers.push_back(TankWithAlgorithm{tank, std::move(algo)});
    }
    LOG_INFO("Tank algorithms created");
}

std::string GameManager::generateOutputFilePath(const std::string& player1Name, const std::string& player2Name) {
    using namespace std::chrono;
    auto now = system_clock::now();
    duration<double> ts = now.time_since_epoch();
    constexpr size_t NUM_DIGITS = 9;
    size_t NUM_DIGITS_P = std::pow(10, NUM_DIGITS);    
    std::ostringstream oss;
    oss << std::setw(NUM_DIGITS) << std::setfill('0') << size_t(ts.count() * NUM_DIGITS_P) % NUM_DIGITS_P;
    std::string uniqueId = oss.str();
    
    std::string cleanMapName = cleanFilename(m_mapName);
    return "game_" + player1Name + "_vs_" + player2Name + "_" + cleanMapName + "_" + uniqueId + ".txt";
}

std::string GameManager::cleanFilename(const std::string& name) {
    std::string cleaned = name;
    
    // Replace characters that are invalid in filenames with underscores
    const std::string invalidChars = "\\/:*?\"<>|";
    for (char& c : cleaned) {
        if (invalidChars.find(c) != std::string::npos) {
            c = '_';
        }
    }
    
    return cleaned;
}

std::string GameManager::extractBaseName(const std::string& fullPath) {
    std::filesystem::path path(fullPath);
    std::string fileName = path.filename().string();
    
    return cleanFilename(fileName);
}

std::vector<std::string> GameManager::readSatelliteView(const SatelliteView& satellite_view, 
                                                       size_t map_width, 
                                                       size_t map_height) const {
    std::vector<std::string> boardLines;
    boardLines.reserve(map_height);
    
    for (size_t y = 0; y < map_height; ++y) {
        std::string row;
        row.reserve(map_width);
        
        for (size_t x = 0; x < map_width; ++x) {
            char ch = satellite_view.getObjectAt(x, y);
            row.push_back(ch);
        }
        
        boardLines.push_back(row);
    }
    
    return boardLines;
}

} // namespace GameManager_098765432_123456789

// TODO: change game manager class name
using namespace GameManager_098765432_123456789;
REGISTER_GAME_MANAGER(GameManager);
