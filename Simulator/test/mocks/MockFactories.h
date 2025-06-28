#pragma once

#include "common/Player.h"
#include "common/TankAlgorithm.h"
#include "MockPlayer.h"
#include "MockTankAlgorithm.h"
#include <memory>

// Centralized mock factory functions for testing
namespace MockFactories {
    
    // Mock factory for creating Player instances
    inline std::unique_ptr<Player> createMockPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) {
        return std::make_unique<MockPlayer>(player_index, x, y, max_steps, num_shells);
    }
    
    // Mock factory for creating TankAlgorithm instances
    inline std::unique_ptr<TankAlgorithm> createMockTankAlgorithm(int player_index, int tank_index) {
        return std::make_unique<MockTankAlgorithm>(player_index, tank_index);
    }
    
    // Null factory for Player (returns nullptr for testing)
    inline std::unique_ptr<Player> createNullPlayer(int /*player_index*/, size_t /*x*/, size_t /*y*/, size_t /*max_steps*/, size_t /*num_shells*/) {
        return nullptr;
    }
    
    // Null factory for TankAlgorithm (returns nullptr for testing)
    inline std::unique_ptr<TankAlgorithm> createNullTankAlgorithm(int /*player_index*/, int /*tank_index*/) {
        return nullptr;
    }
    
}  // namespace MockFactories