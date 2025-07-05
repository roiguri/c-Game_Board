#pragma once

#include "common/TankAlgorithm.h"
#include "common/ActionRequest.h"
#include "common/BattleInfo.h"

// No using namespace - we'll use full qualified names

namespace TestAlgorithm_AlwaysShoot_098765432_123456789 {

/**
 * @brief Tank algorithm that always shoots shells
 */
class AlwaysShootTankAlgorithm : public TankAlgorithm {
public:
    AlwaysShootTankAlgorithm(int playerId, int tankIndex);
    ~AlwaysShootTankAlgorithm() override;
    
    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;

private:
    int m_playerId;
    int m_tankIndex;
};

} // namespace TestAlgorithm_AlwaysShoot_098765432_123456789