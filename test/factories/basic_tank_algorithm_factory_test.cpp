#include "gtest/gtest.h"
#include "factories/basic_tank_algorithm_factory.h"
#include "algo/basic_tank_algorithm.h"
#include "algo/offensive_tank_algorithm.h"
#include <memory>

TEST(BasicTankAlgorithmFactoryTest, CreatesBasicTankAlgorithm) {
    BasicTankAlgorithmFactory factory;
    int playerIndex = 1;
    int tankIndex = 2;
    std::unique_ptr<TankAlgorithm> algo = factory.create(playerIndex, tankIndex);
    ASSERT_NE(algo, nullptr);
    // Check type via dynamic_cast
    auto* basicAlgo = dynamic_cast<BasicTankAlgorithm*>(algo.get());
    ASSERT_NE(basicAlgo, nullptr);
} 

TEST(BasicTankAlgorithmFactoryTest, CreatesOffensiveTankAlgorithm) {
    BasicTankAlgorithmFactory factory;
    int playerIndex = 2;
    int tankIndex = 2;
    std::unique_ptr<TankAlgorithm> algo = factory.create(playerIndex, tankIndex);
    ASSERT_NE(algo, nullptr);
    // Check type via dynamic_cast
    auto* offensiveAlgo = dynamic_cast<OffensiveTankAlgorithm*>(algo.get());
    ASSERT_NE(offensiveAlgo, nullptr);
} 