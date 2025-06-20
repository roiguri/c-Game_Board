#pragma once
#include <gmock/gmock.h>
#include "Player.h"
#include "PlayerFactory.h"

class MockPlayer : public Player {
public:
    MockPlayer() : Player(1, 5, 5, 100, 5) {} // Use dummy values for base constructor
    MOCK_METHOD(void, updateTankWithBattleInfo, (TankAlgorithm&, SatelliteView&), (override));
    // Add more mocked methods if needed
};

class MockPlayerFactory : public PlayerFactory {
public:
    std::unique_ptr<Player> create(int, size_t, size_t, size_t, size_t) const override {
        return std::make_unique<MockPlayer>();
    }
};
