#pragma once

#include "Player.h"

struct PlayerRegistration {
    PlayerRegistration(PlayerFactory);
};

#define REGISTER_PLAYER(class_name) \
PlayerRegistration register_me_##class_name \
    ( [] (int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) { \
        return std::make_unique<class_name>(player_index, x, y, max_steps, num_shells); \
    } );
