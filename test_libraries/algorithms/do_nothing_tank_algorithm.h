#pragma once

#include "common/TankAlgorithm.h"
#include "common/ActionRequest.h"
#include "common/BattleInfo.h"

// No using namespace - we'll use full qualified names

namespace TestAlgorithm_DoNothing_098765432_123456789 {

/**
 * @brief Tank algorithm that does nothing (always requests battle info)
 */
class DoNothingTankAlgorithm : public TankAlgorithm {
public:
    DoNothingTankAlgorithm(int playerId, int tankIndex);
    ~DoNothingTankAlgorithm() override;
    
    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;

private:
    int m_playerId;
    int m_tankIndex;
};

} // namespace TestAlgorithm_DoNothing_098765432_123456789