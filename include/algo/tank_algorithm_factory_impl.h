#pragma once

#include "TankAlgorithmFactory.h"
#include <memory>

/**
 * @class TankAlgorithmFactoryImpl
 * @brief Concrete factory for creating TankAlgorithm objects
 *
 * Implements the TankAlgorithmFactory interface to provide
 * specific TankAlgorithm instances.
 */
class TankAlgorithmFactoryImpl : public TankAlgorithmFactory {
public:
    /**
     * @brief Creates a new TankAlgorithm instance
     *
     * @param player_index The ID of the player (1 or 2) this algorithm is for
     * @param tank_index The index of the tank this algorithm will control
     * @return std::unique_ptr<TankAlgorithm> A smart pointer to the created algorithm
     */
    std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override;
};
