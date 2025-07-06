#include "game_runner.h"
#include "utils/file_loader.h"
#include "registration/AlgorithmRegistrar.h"
#include "registration/GameManagerRegistrar.h"
#include <stdexcept>

GameResult GameRunner::runSingleGame(
    const FileLoader::BoardInfo& boardInfo,
    const std::string& gameManagerName,
    const std::string& algorithm1Name,
    const std::string& algorithm2Name,
    bool verbose
) {
    // 1. Extract board information
    if (!boardInfo.satelliteView) {
        throw std::runtime_error("Invalid board info: null satellite view");
    }
    
    // TODO: cosider replacing board info to ints
    size_t mapWidth = static_cast<size_t>(boardInfo.cols);
    size_t mapHeight = static_cast<size_t>(boardInfo.rows);
    size_t maxSteps = static_cast<size_t>(boardInfo.maxSteps);
    size_t numShells = static_cast<size_t>(boardInfo.numShells);
    auto& satelliteView = *boardInfo.satelliteView;
    
    // 2. Get GameManager factory from registrar
    auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    const auto* gameManagerEntry = gameManagerRegistrar.findByName(gameManagerName);
    if (!gameManagerEntry) {
        // TODO: decide what to do when fails to load game manager
        throw std::runtime_error("GameManager not found: " + gameManagerName);
    }
    
    // 3. Get Algorithm factories from registrar
    auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    // Find algorithm entries by name using iterators
    auto algo1It = algorithmRegistrar.end();
    auto algo2It = algorithmRegistrar.end();
    
    for (auto it = algorithmRegistrar.begin(); it != algorithmRegistrar.end(); ++it) {
        if (it->getName() == algorithm1Name) {
            algo1It = it;
        }
        if (it->getName() == algorithm2Name) {
            algo2It = it;
        }
    }
    
    if (algo1It == algorithmRegistrar.end()) {
        // TODO: decide what to do when fails to load algorithm
        throw std::runtime_error("Algorithm not found: " + algorithm1Name);
    }
    if (algo2It == algorithmRegistrar.end()) {
        // TODO: decide what to do when fails to load algorithm
        throw std::runtime_error("Algorithm not found: " + algorithm2Name);
    }
    
    if (!algo1It->isComplete()) {
        // TODO: decide what to do when fails to load algorithm
        throw std::runtime_error("Algorithm incomplete: " + algorithm1Name);
    }
    if (!algo2It->isComplete()) {
        // TODO: decide what to do when fails to load algorithm
        throw std::runtime_error("Algorithm incomplete: " + algorithm2Name);
    }
    
    // 4. Create players using algorithm factories
    auto player1 = algo1It->getPlayerFactory()(1, mapWidth, mapHeight, maxSteps, numShells);
    auto player2 = algo2It->getPlayerFactory()(2, mapWidth, mapHeight, maxSteps, numShells);
    
    // 5. Create GameManager
    auto gameManager = gameManagerEntry->create(verbose);
    
    // 6. Execute game using map name from board file
    GameResult result = gameManager->run(
        mapWidth, mapHeight,
        satelliteView,
        boardInfo.mapName,
        maxSteps, numShells,
        *player1, algorithm1Name,
        *player2, algorithm2Name,
        algo1It->getTankAlgorithmFactory(),
        algo2It->getTankAlgorithmFactory()
    );
    
    return result;
}