#include "always_move_forward_tank_algorithm.h"
#include "common/TankAlgorithmRegistration.h"


namespace TestAlgorithm_AlwaysMoveForward_098765432_123456789 {

AlwaysMoveForwardTankAlgorithm::AlwaysMoveForwardTankAlgorithm(int playerId, int tankIndex)
    : m_playerId(playerId), m_tankIndex(tankIndex) {
}

AlwaysMoveForwardTankAlgorithm::~AlwaysMoveForwardTankAlgorithm() = default;

ActionRequest AlwaysMoveForwardTankAlgorithm::getAction() {
    // Always move forward
    return ActionRequest::MoveForward;
}

void AlwaysMoveForwardTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    // Don't need battle info for this simple behavior
    (void)info;
}

} // namespace TestAlgorithm_AlwaysMoveForward_098765432_123456789

// Registration at global scope
using namespace TestAlgorithm_AlwaysMoveForward_098765432_123456789;
REGISTER_TANK_ALGORITHM(AlwaysMoveForwardTankAlgorithm);