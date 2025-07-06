#include "predictable_game_manager.h"
#include "satellite_view_impl.h"
#include "game_board.h"
#include "objects/tank.h"
#include "objects/shell.h"
#include "utils/point.h"
#include "utils/direction.h"
#include "common/GameManagerRegistration.h"
#include <iostream>
#include <memory>
#include <vector>
#include <string>


namespace TestGameManager_Predictable_098765432_123456789 {

using namespace UserCommon_098765432_123456789;

PredictableGameManager::PredictableGameManager(bool verbose) 
    : m_verbose(verbose) {
}

PredictableGameManager::~PredictableGameManager() = default;

GameResult PredictableGameManager::run(
    size_t map_width, size_t map_height,
    const SatelliteView& map,
    size_t max_steps, size_t num_shells,
    Player& player1, Player& player2,
    TankAlgorithmFactory player1_tank_algo_factory,
    TankAlgorithmFactory player2_tank_algo_factory) {
    
    if (m_verbose) {
        std::cout << "PredictableGameManager: Always returning player 1 wins" << std::endl;
    }
    
    // Create minimal game state for player 1 win result
    GameBoard finalBoard(map_width, map_height);
    std::vector<Tank> finalTanks;
    std::vector<Shell> finalShells;
    
    // Add only player 1's tank to show win
    if (map_width > 2 && map_height > 2) {
        finalTanks.emplace_back(1, Point(1, 1), Direction::Up);
    }
    
    auto finalState = std::make_unique<SatelliteViewImpl>(finalBoard, finalTanks, finalShells);
    
    // Create and return player 1 win result
    GameResult result;
    result.winner = 1; // Always player 1 wins
    result.rounds = 0; // No rounds played
    result.reason = GameResult::Reason::ALL_TANKS_DEAD;
    result.gameState = std::move(finalState);
    
    // Only player 1 has remaining tanks
    result.remaining_tanks.push_back(1);
    
    return result;
}

} // namespace TestGameManager_Predictable_098765432_123456789

// Registration at global scope
using namespace TestGameManager_Predictable_098765432_123456789;
REGISTER_GAME_MANAGER(PredictableGameManager);