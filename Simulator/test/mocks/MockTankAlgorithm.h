#pragma once

#include "common/TankAlgorithm.h"
#include "common/ActionRequest.h"
#include "common/BattleInfo.h"

// Mock TankAlgorithm for testing
class MockTankAlgorithm : public TankAlgorithm {
public:
    MockTankAlgorithm(int /*player_index*/, int /*tank_index*/) {}
    
    ActionRequest getAction() override {
        return ActionRequest{};
    }
    
    void updateBattleInfo(BattleInfo& /*info*/) override {
        // Mock implementation
    }
};