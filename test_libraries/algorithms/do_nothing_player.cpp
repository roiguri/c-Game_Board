#include "do_nothing_player.h"
#include "do_nothing_tank_algorithm.h"
#include "common/PlayerRegistration.h"

namespace TestAlgorithm_DoNothing_098765432_123456789 {

DoNothingPlayer::DoNothingPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
    : m_playerIndex(playerIndex), m_boardWidth(x), m_boardHeight(y), 
      m_maxSteps(maxSteps), m_numShells(numShells) {
}

DoNothingPlayer::~DoNothingPlayer() = default;

void DoNothingPlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satelliteView) {
    // Do nothing - tanks will continue with their default behavior (do nothing)
    (void)tank;
    (void)satelliteView;
}

} // namespace TestAlgorithm_DoNothing_098765432_123456789

// Registration at global scope
using namespace TestAlgorithm_DoNothing_098765432_123456789;
REGISTER_PLAYER(DoNothingPlayer);