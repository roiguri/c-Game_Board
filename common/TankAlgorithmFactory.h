#pragma once

#include "TankAlgorithm.h"
#include <memory>

/**
 * @class TankAlgorithmFactory
 * @brief Factory interface for creating TankAlgorithm objects
 * 
 * Provides an abstract factory method to create TankAlgorithm instances.
 */
class TankAlgorithmFactory {  
public:  
    /**
     * @brief Virtual destructor
     */
    virtual ~TankAlgorithmFactory() {}  

    /**
     * @brief Creates a new TankAlgorithm instance
     * 
     * @param player_index The ID of the player (1 or 2) this algorithm is for
     * @param tank_index The index of the tank this algorithm will control
     * @return std::unique_ptr<TankAlgorithm> A smart pointer to the created algorithm
     */   
    virtual std::unique_ptr<TankAlgorithm> create(int player_index, 
                                                  int tank_index) const = 0;  
};