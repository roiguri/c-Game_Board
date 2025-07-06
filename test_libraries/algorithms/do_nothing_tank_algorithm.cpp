#include "do_nothing_tank_algorithm.h"
#include "common/TankAlgorithmRegistration.h"

namespace TestAlgorithm_DoNothing_098765432_123456789 {

DoNothingTankAlgorithm::DoNothingTankAlgorithm(int playerId, int tankIndex)
    : m_playerId(playerId), m_tankIndex(tankIndex) {
}

DoNothingTankAlgorithm::~DoNothingTankAlgorithm() = default;

ActionRequest DoNothingTankAlgorithm::getAction() {
    // Always request battle info (do nothing)
    return ActionRequest::GetBattleInfo;
}

void DoNothingTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    // Do nothing with the battle info
    (void)info;
}

} // namespace TestAlgorithm_DoNothing_098765432_123456789

// Registration at global scope
using namespace TestAlgorithm_DoNothing_098765432_123456789;
REGISTER_TANK_ALGORITHM(DoNothingTankAlgorithm);