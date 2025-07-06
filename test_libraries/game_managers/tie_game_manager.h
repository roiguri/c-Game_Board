#pragma once

#include "common/AbstractGameManager.h"
#include "common/GameResult.h"
#include "common/Player.h"
#include "common/SatelliteView.h"
#include "common/TankAlgorithm.h"

// No using namespace - we'll use full qualified names

namespace TestGameManager_AlwaysTie_098765432_123456789 {

/**
 * @brief GameManager for testing that always results in a tie
 */
class TieGameManager : public AbstractGameManager {
public:
    TieGameManager(bool verbose = false);
    ~TieGameManager() override;
    
    GameResult run(
        size_t map_width, size_t map_height,
        const SatelliteView& map,
        size_t max_steps, size_t num_shells,
        Player& player1, Player& player2,
        TankAlgorithmFactory player1_tank_algo_factory,
        TankAlgorithmFactory player2_tank_algo_factory) override;

private:
    bool m_verbose;
};

} // namespace TestGameManager_AlwaysTie_098765432_123456789