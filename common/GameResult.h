#pragma once

#include <vector>
#include <memory>

#include "SatelliteView.h"

struct GameResult {
    int winner; // 0 = tie
    enum Reason { ALL_TANKS_DEAD, MAX_STEPS, ZERO_SHELLS };
    Reason reason;
    std::vector<size_t> remaining_tanks; // index 0 = player 1, etc.
    std::unique_ptr<SatelliteView> gameState; // at end of game
    size_t rounds; // total number of rounds
};