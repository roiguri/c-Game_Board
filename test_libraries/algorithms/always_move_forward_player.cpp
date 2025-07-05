#include "always_move_forward_player.h"
#include "always_move_forward_tank_algorithm.h"
#include "common/PlayerRegistration.h"


namespace TestAlgorithm_AlwaysMoveForward_098765432_123456789 {

AlwaysMoveForwardPlayer::AlwaysMoveForwardPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
    : m_playerIndex(playerIndex), m_boardWidth(x), m_boardHeight(y), 
      m_maxSteps(maxSteps), m_numShells(numShells) {
}

AlwaysMoveForwardPlayer::~AlwaysMoveForwardPlayer() = default;

void AlwaysMoveForwardPlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satelliteView) {
    // Simple implementation - tanks will move forward on their own
    (void)tank;
    (void)satelliteView;
}

} // namespace TestAlgorithm_AlwaysMoveForward_098765432_123456789

// Registration at global scope
using namespace TestAlgorithm_AlwaysMoveForward_098765432_123456789;
REGISTER_PLAYER(AlwaysMoveForwardPlayer);