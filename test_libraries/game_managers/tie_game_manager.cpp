#include "tie_game_manager.h"
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

namespace TestGameManager_AlwaysTie_098765432_123456789 {

using namespace UserCommon_098765432_123456789;

TieGameManager::TieGameManager(bool verbose) 
    : m_verbose(verbose) {
}

TieGameManager::~TieGameManager() = default;

GameResult TieGameManager::run(
    size_t map_width, size_t map_height,
    const SatelliteView& map,
    size_t max_steps, size_t num_shells,
    Player& player1, Player& player2,
    TankAlgorithmFactory player1_tank_algo_factory,
    TankAlgorithmFactory player2_tank_algo_factory) {
    
    if (m_verbose) {
        std::cout << "TieGameManager: Always returning tie result" << std::endl;
    }
    
    // Create minimal game state for tie result
    GameBoard finalBoard(map_width, map_height);
    std::vector<Tank> finalTanks;
    std::vector<Shell> finalShells;
    
    // Add both players' tanks to show tie
    if (map_width > 2 && map_height > 2) {
        finalTanks.emplace_back(1, Point(1, 1), Direction::Up);
    }
    if (map_width > 3 && map_height > 2) {
        finalTanks.emplace_back(2, Point(map_width - 2, 1), Direction::Up);
    }
    
    auto finalState = std::make_unique<SatelliteViewImpl>(finalBoard, finalTanks, finalShells);
    
    // Create and return tie result
    GameResult result;
    result.winner = 0; // Always tie
    result.rounds = 0; // No rounds played
    result.reason = GameResult::Reason::MAX_STEPS;
    result.gameState = std::move(finalState);
    
    // Both players have remaining tanks in a tie
    result.remaining_tanks.push_back(1);
    result.remaining_tanks.push_back(2);
    
    return result;
}

} // namespace TestGameManager_AlwaysTie_098765432_123456789

// Registration at global scope
using namespace TestGameManager_AlwaysTie_098765432_123456789;
REGISTER_GAME_MANAGER(TieGameManager);