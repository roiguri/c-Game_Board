#pragma once
#include "PlayerFactory.h"
#include <memory>

class PlayerFactoryImpl : public PlayerFactory {
public:
    std::unique_ptr<Player> create(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) const override {
        // Use parameters to avoid unused warnings
        (void)player_index;
        (void)x;
        (void)y;
        (void)max_steps;
        (void)num_shells;
        // TODO: Implement actual player creation
        return nullptr;
    }
}; 