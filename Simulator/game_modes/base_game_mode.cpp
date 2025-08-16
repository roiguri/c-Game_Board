#include "base_game_mode.h"
#include "utils/file_enumerator.h"
#include "utils/output_generator.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

BaseGameMode::BaseGameMode() 
    : m_libraryManager(LibraryManager::getInstance()) {
}

BaseGameMode::~BaseGameMode() {
    cleanup();
}

GameResult BaseGameMode::execute(const BaseParameters& params) {
    GameResult result;
    
    try {
        // Load libraries (subclass responsibility)
        if (!loadLibraries(params)) {
            return createErrorResult();
        }
        
        // Load map (subclass responsibility)
        if (!loadMap(params.mapFile)) {
            return createErrorResult();
        }
        
        // Execute game logic (subclass responsibility)
        result = executeGameLogic(params);
        
        displayResults(result);
    } catch (const std::exception& e) {
        // Should not happen
        std::cout << "Error: Unexpected exception during execution: " + std::string(e.what()) << std::endl;
        result = createErrorResult();
    }
    
    return result;
}

void BaseGameMode::cleanup() {
    // Subclasses handle their own cleanup
}

std::string BaseGameMode::generateTimestamp(bool includeMilliseconds) const {
    return OutputGenerator::generateTimestamp(includeMilliseconds);
}

std::vector<std::string> BaseGameMode::enumerateFiles(const std::string& directory, const std::string& extension) const {
    if (extension == ".so") {
        return FileEnumerator::enumerateSoFiles(directory);
    } else if (extension == ".txt") {
        return FileEnumerator::enumerateMapFiles(directory);
    } else {
        return {};
    }
}

LibraryValidator::LibraryInfo BaseGameMode::validateLibrary(const std::string& libraryPath, const std::string& type) const {
    if (type == "GameManager") {
        return LibraryValidator::validateGameManager(libraryPath);
    } else if (type == "Algorithm") {
        return LibraryValidator::validateAlgorithm(libraryPath);
    } else {
        LibraryValidator::LibraryInfo info(libraryPath);
        info.error = "Unknown library type: " + type;
        return info;
    }
}

GameResult BaseGameMode::createErrorResult() const {
    GameResult result;
    result.winner = 0;  // No winner
    result.reason = GameResult::Reason::ALL_TANKS_DEAD;  // Default reason
    result.remaining_tanks.clear();
    result.gameState = nullptr;
    result.rounds = 0;
    return result;
}

bool BaseGameMode::writeToFile(const std::string& filePath, const std::string& content, bool fallbackToConsole) const {
    return OutputGenerator::writeToFile(filePath, content, fallbackToConsole);
}
