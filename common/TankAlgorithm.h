#pragma once

#include "ActionRequest.h"
#include "BattleInfo.h"

/**
 * @class TankAlgorithm
 * @brief Interface for tank control algorithms
 * 
 * Defines the interface that all tank algorithms must implement.
 * Algorithms determine tank behavior through action requests and
 * receive information about the game state through battle info objects.
 */
class TankAlgorithm {  
public:  
    /**
     * @brief Virtual destructor
     */
    virtual ~TankAlgorithm() {}  

    /**
     * @brief Gets the next action for the tank
     * 
     * @return ActionRequest The action the tank should perform
     */
    virtual ActionRequest getAction() = 0;  

    /**
     * @brief Processes battle information update
     * 
     * Called when the tank receives battle information,
     * allowing the algorithm to update its internal state.
     * 
     * @param info Reference to battle information object
     */
    virtual void updateBattleInfo(BattleInfo& info) = 0;  
};