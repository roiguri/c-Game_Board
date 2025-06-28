#pragma once

#include "AbstractGameManager.h"

// TODO: basic stub - requires full implementation.
struct GameManagerRegistration {
    GameManagerRegistration(GameManagerFactory);
};

#define REGISTER_GAME_MANAGER(class_name) \
GameManagerRegistration register_me_##class_name \
    ( [] (bool verbose) { return std::make_unique<class_name>(verbose); } );
