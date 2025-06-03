#include "factories/basic_player_factory.h"
#include "players/basic/basic_player.h"
#include "players/offensive_player.h"

std::unique_ptr<Player> BasicPlayerFactory::create(
    int playerIndex, 
    size_t x, size_t y,  
    size_t maxSteps, 
    size_t numShells) const {
    if (playerIndex == 1) {
        return std::make_unique<BasicPlayer>(playerIndex, x, y, maxSteps, numShells);
    } else {
        return std::make_unique<OffensivePlayer>(playerIndex, x, y, maxSteps, numShells);
    }
}