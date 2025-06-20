#pragma once
#include <vector>

struct GameResult {
    int winner; // 0 = tie
    enum Reason { ALL_TANKS_DEAD, MAX_STEPS, ZERO_SHELLS };
    Reason reason;
    std::vector<size_t> remaining_tanks; // index 0 = player 1, etc.
};