#include <gtest/gtest.h>
#include "common/TankAlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"
#include "test/mocks/MockFactories.h"
#include "test/mocks/MockTankAlgorithm.h"

class TankAlgorithmRegistrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    }
    
    void TearDown() override {
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    }
    
};

TEST_F(TankAlgorithmRegistrationTest, TankAlgorithmRegistration_AddsFactoryToRegistrar) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    registrar.createAlgorithmFactoryEntry("test_algorithm");
    
    TankAlgorithmFactory factory = MockFactories::createNullTankAlgorithm;
    TankAlgorithmRegistration registration(std::move(factory));
    
    EXPECT_EQ(registrar.size(), 1);
}

TEST_F(TankAlgorithmRegistrationTest, TankAlgorithmRegistration_NoEntry_ThrowsException) {
    TankAlgorithmFactory factory = MockFactories::createNullTankAlgorithm;
    
    EXPECT_THROW(TankAlgorithmRegistration registration(std::move(factory)), BadRegistrationException);
}

TEST_F(TankAlgorithmRegistrationTest, RegisterTankAlgorithmMacro_CreatesRegistration) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    registrar.createAlgorithmFactoryEntry("test_algorithm");
    
    // Simulate what the macro would do using centralized mock
    [[maybe_unused]] TankAlgorithmRegistration register_me_MockTankAlgorithm(
        MockFactories::createMockTankAlgorithm
    );
    
    EXPECT_EQ(registrar.size(), 1);
}