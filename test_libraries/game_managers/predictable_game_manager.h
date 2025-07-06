#pragma once

#include "common/AbstractGameManager.h"
#include "common/GameResult.h"
#include "common/Player.h"
#include "common/SatelliteView.h"
#include "common/TankAlgorithm.h"

// No using namespace - we'll use full qualified names

namespace TestGameManager_Predictable_098765432_123456789 {

/**
 * @brief Predictable GameManager for testing that always returns player 1 wins
 */
class PredictableGameManager : public AbstractGameManager {
public:
    PredictableGameManager(bool verbose = false);
    ~PredictableGameManager() override;
    
    GameResult run(
        size_t map_width, size_t map_height,
        const SatelliteView& map,
        string map_name,
        size_t max_steps, size_t num_shells,
        Player& player1, string name1,
        Player& player2, string name2,
        TankAlgorithmFactory player1_tank_algo_factory,
        TankAlgorithmFactory player2_tank_algo_factory) override;

private:
    bool m_verbose;
};

} // namespace TestGameManager_Predictable_098765432_123456789