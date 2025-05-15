#include "algo/basic_tank_algorithm_factory.h"
#include "algo/basic_tank_algorithm.h"
#include "algo/offensive_tank_algorithm.h"

std::unique_ptr<TankAlgorithm> BasicTankAlgorithmFactory::create(
    int player_index, 
    int tank_index) const {
    if (player_index == 1) {
        return std::make_unique<BasicTankAlgorithm>(player_index, tank_index);
    } else {
        return std::make_unique<OffensiveTankAlgorithm>(player_index, tank_index);
    }
}