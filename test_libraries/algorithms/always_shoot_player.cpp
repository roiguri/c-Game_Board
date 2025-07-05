#include "always_shoot_player.h"
#include "always_shoot_tank_algorithm.h"
#include "common/PlayerRegistration.h"


namespace TestAlgorithm_AlwaysShoot_098765432_123456789 {

AlwaysShootPlayer::AlwaysShootPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
    : m_playerIndex(playerIndex), m_boardWidth(x), m_boardHeight(y), 
      m_maxSteps(maxSteps), m_numShells(numShells) {
}

AlwaysShootPlayer::~AlwaysShootPlayer() = default;

void AlwaysShootPlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satelliteView) {
    // Simple implementation - tanks will shoot on their own
    (void)tank;
    (void)satelliteView;
}

} // namespace TestAlgorithm_AlwaysShoot_098765432_123456789

// Registration at global scope
using namespace TestAlgorithm_AlwaysShoot_098765432_123456789;
REGISTER_PLAYER(AlwaysShootPlayer);