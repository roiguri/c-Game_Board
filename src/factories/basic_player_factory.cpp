#include "factories/basic_player_factory.h"
#include "players/basic/basic_player.h"
#include "players/offensive/offensive_player.h"

std::unique_ptr<Player> BasicPlayerFactory::create(
    int playerIndex, 
    size_t x, size_t y,  
    size_t maxSteps, 
    size_t numShells) const {
    if (playerIndex % 2 == 1) {
        // Odd players (1,3,5,7,9) get BasicPlayer
        return std::make_unique<BasicPlayer>(playerIndex, x, y, maxSteps, numShells);
    } else {
        // Even players (2,4,6,8) get OffensivePlayer
        return std::make_unique<OffensivePlayer>(playerIndex, x, y, maxSteps, numShells);
    }
}