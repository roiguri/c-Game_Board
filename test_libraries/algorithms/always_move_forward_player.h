#pragma once

#include <memory>
#include "common/Player.h"
#include "common/SatelliteView.h"
#include "common/TankAlgorithm.h"

// No using namespace - we'll use full qualified names

namespace TestAlgorithm_AlwaysMoveForward_098765432_123456789 {

/**
 * @brief Player that makes tanks always move forward
 */
class AlwaysMoveForwardPlayer : public Player {
public:
    AlwaysMoveForwardPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells);
    ~AlwaysMoveForwardPlayer() override;
    
    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satelliteView) override;

private:
    int m_playerIndex;
    size_t m_boardWidth;
    size_t m_boardHeight;
    size_t m_maxSteps;
    size_t m_numShells;
};

} // namespace TestAlgorithm_AlwaysMoveForward_098765432_123456789