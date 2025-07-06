#pragma once

#include <memory>
#include "common/Player.h"
#include "common/SatelliteView.h"
#include "common/TankAlgorithm.h"

// No using namespace - we'll use full qualified names

namespace TestAlgorithm_DoNothing_098765432_123456789 {

/**
 * @brief DoNothing player implementation - tanks do nothing
 */
class DoNothingPlayer : public Player {
public:
    DoNothingPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells);
    ~DoNothingPlayer() override;
    
    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satelliteView) override;

private:
    int m_playerIndex;
    size_t m_boardWidth;
    size_t m_boardHeight;
    size_t m_maxSteps;
    size_t m_numShells;
};

} // namespace TestAlgorithm_DoNothing_098765432_123456789