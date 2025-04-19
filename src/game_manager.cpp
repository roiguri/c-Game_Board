#include "game_manager.h"
#include "file_loader.h"
#include "collision_handler.h"
#include <iostream>

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

bool GameManager::initialize(const std::string& filePath) {
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
    
    bool initSuccess = m_board.initialize(boardLines, errors);
    for (const auto& error : errors) {
      m_gameLog.push_back("Error: " + error);
    }
    if (!initSuccess) {
        // Unrecoverable error in board initialization
        return false;
    }
    
    // TODO: Create tanks from board
    
    // Create algorithms
    createAlgorithms();
    
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
    // This is a stub implementation
    return Action::None;
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

void GameManager::logAction(int playerId, Action action, bool valid) {
    // This is a stub implementation
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