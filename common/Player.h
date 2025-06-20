#pragma once

#include "TankAlgorithm.h"
#include "SatelliteView.h"
#include <memory>
#include <functional>

// TODO: update cpp files accordingly.
class Player {  
public:
    virtual ~Player() {}  
    virtual void updateTankWithBattleInfo(TankAlgorithm& tank,
                                          SatelliteView& satellite_view) = 0;  
};

using PlayerFactory = 
    std::function<std::unique_ptr<Player>
    (int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)>;