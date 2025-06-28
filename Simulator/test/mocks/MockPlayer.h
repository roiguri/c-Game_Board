#pragma once

#include "common/Player.h"
#include "common/TankAlgorithm.h"
#include "common/SatelliteView.h"

// Mock Player for testing
class MockPlayer : public Player {
public:
    MockPlayer(int /*player_index*/, size_t /*x*/, size_t /*y*/, size_t /*max_steps*/, size_t /*num_shells*/) {}
    
    void updateTankWithBattleInfo(TankAlgorithm& /*tank*/, SatelliteView& /*view*/) override {
        // Mock implementation
    }
};