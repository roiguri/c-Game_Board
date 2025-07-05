#pragma once

#include "common/TankAlgorithm.h"
#include "common/ActionRequest.h"
#include "common/BattleInfo.h"

// No using namespace - we'll use full qualified names

namespace TestAlgorithm_AlwaysMoveForward_098765432_123456789 {

/**
 * @brief Tank algorithm that always moves forward
 */
class AlwaysMoveForwardTankAlgorithm : public TankAlgorithm {
public:
    AlwaysMoveForwardTankAlgorithm(int playerId, int tankIndex);
    ~AlwaysMoveForwardTankAlgorithm() override;
    
    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;

private:
    int m_playerId;
    int m_tankIndex;
};

} // namespace TestAlgorithm_AlwaysMoveForward_098765432_123456789