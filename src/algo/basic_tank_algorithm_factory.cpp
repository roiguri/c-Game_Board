#include "algo/basic_tank_algorithm_factory.h"
#include "algo/basic_tank_algorithm.h"
#include "algo/offensive_tank_algorithm.h"

std::unique_ptr<TankAlgorithm> BasicTankAlgorithmFactory::create(
    int playerIndex, 
    int tankIndex) const {
    if (playerIndex == 1) {
        return std::make_unique<BasicTankAlgorithm>(playerIndex, tankIndex);
    } else {
        return std::make_unique<OffensiveTankAlgorithm>(playerIndex, tankIndex);
    }
}