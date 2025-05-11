#pragma once

#include "TankAlgorithmFactory.h"

/**
 * @class BasicTankAlgorithmFactory
 * @brief Factory for creating BasicTankAlgorithm instances
 * 
 * Implements the TankAlgorithmFactory interface to create BasicTankAlgorithm
 * objects for controlling tanks in the game.
 */
class BasicTankAlgorithmFactory : public TankAlgorithmFactory {
public:
    /**
     * @brief Constructor
     */
    BasicTankAlgorithmFactory() = default;
    
    /**
     * @brief Destructor
     */
    ~BasicTankAlgorithmFactory() override = default;
    
    /**
     * @brief Creates a new TankAlgorithm instance
     * 
     * @param player_index The ID of the player (1 or 2) this algorithm is for
     * @param tank_index The index of the tank this algorithm will control
     * @return A unique pointer to the created TankAlgorithm
     */
    std::unique_ptr<TankAlgorithm> create(
        int player_index, 
        int tank_index) const override;
};