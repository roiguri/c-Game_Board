#pragma once

/** 
* @class BattleInfo
* @brief Base class for battle information
* 
* This class serves as a base for all battle-related information
* that can be passed to tank algorithms. It can be extended to
* include specific details about the battle state.
*/
class BattleInfo {  
public:  
    /**
     * @brief Virtual destructor
     */
    virtual ~BattleInfo() {}  
};