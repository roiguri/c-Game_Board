#include "players/basic_player_factory.h"
#include "players/basic_player.h"

std::unique_ptr<Player> BasicPlayerFactory::create(
    int player_index, 
    size_t x, size_t y,  
    size_t max_steps, 
    size_t num_shells) const {
    
    // Simply create and return a new BasicPlayer instance
    return std::make_unique<BasicPlayer>(player_index, x, y, max_steps, num_shells);
}