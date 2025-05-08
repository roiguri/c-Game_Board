#include "algo/tank_algorithm_factory_impl.h"
#include <memory>
#include <algo/chase_algorithm.h>
#include <algo/defensive_algorithm.h>

std::unique_ptr<TankAlgorithm> TankAlgorithmFactoryImpl::create(int player_index, int tank_index) const {
    // Use parameters to avoid unused warnings
    (void)player_index;
    (void)tank_index;
    // TODO: Implement logic to return a specific TankAlgorithm instance
    return nullptr;
} 