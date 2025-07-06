#include "always_shoot_tank_algorithm.h"
#include "common/TankAlgorithmRegistration.h"


namespace TestAlgorithm_AlwaysShoot_098765432_123456789 {

AlwaysShootTankAlgorithm::AlwaysShootTankAlgorithm(int playerId, int tankIndex)
    : m_playerId(playerId), m_tankIndex(tankIndex) {
}

AlwaysShootTankAlgorithm::~AlwaysShootTankAlgorithm() = default;

ActionRequest AlwaysShootTankAlgorithm::getAction() {
    // Always shoot
    return ActionRequest::Shoot;
}

void AlwaysShootTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    // Don't need battle info for this simple behavior
    (void)info;
}

} // namespace TestAlgorithm_AlwaysShoot_098765432_123456789

// Registration at global scope
using namespace TestAlgorithm_AlwaysShoot_098765432_123456789;
REGISTER_TANK_ALGORITHM(AlwaysShootTankAlgorithm);