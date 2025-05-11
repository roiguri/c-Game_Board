#include "algo/basic_tank_algorithm_factory.h"
#include "algo/basic_tank_algorithm.h"

std::unique_ptr<TankAlgorithm> BasicTankAlgorithmFactory::create(
    int player_index, 
    int tank_index) const {
    
    // Create and return a new BasicTankAlgorithm instance
    return std::make_unique<BasicTankAlgorithm>(player_index, tank_index);
}