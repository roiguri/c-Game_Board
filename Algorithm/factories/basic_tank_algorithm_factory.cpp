#include "factories/basic_tank_algorithm_factory.h"
#include "players/basic/basic_tank_algorithm.h"
#include "players/offensive/offensive_tank_algorithm.h"

std::unique_ptr<TankAlgorithm> BasicTankAlgorithmFactory::create(
    int playerIndex, 
    int tankIndex) const {
    if (playerIndex % 2 == 1) {
        // Odd players (1,3,5,7,9) get BasicTankAlgorithm
        return std::make_unique<BasicTankAlgorithm>(playerIndex, tankIndex);
    } else {
        // Even players (2,4,6,8) get OffensiveTankAlgorithm
        return std::make_unique<OffensiveTankAlgorithm>(playerIndex, tankIndex);
    }
}