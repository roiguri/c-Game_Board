#include <gtest/gtest.h>
#include "common/PlayerRegistration.h"
#include "AlgorithmRegistrar.h"
#include "test/mocks/MockFactories.h"
#include "test/mocks/MockPlayer.h"

class PlayerRegistrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    }
    
    void TearDown() override {
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    }
    
};

TEST_F(PlayerRegistrationTest, PlayerRegistration_AddsFactoryToRegistrar) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    registrar.createAlgorithmFactoryEntry("test_algorithm");
    
    PlayerFactory factory = MockFactories::createNullPlayer;
    PlayerRegistration registration(std::move(factory));
    
    EXPECT_EQ(registrar.size(), 1);
}

TEST_F(PlayerRegistrationTest, PlayerRegistration_NoEntry_ThrowsException) {
    PlayerFactory factory = MockFactories::createNullPlayer;
    
    EXPECT_THROW(PlayerRegistration registration(std::move(factory)), BadRegistrationException);
}

TEST_F(PlayerRegistrationTest, RegisterPlayerMacro_CreatesRegistration) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    registrar.createAlgorithmFactoryEntry("test_algorithm");
    
    // Simulate what the macro would do using centralized mock
    [[maybe_unused]] PlayerRegistration register_me_MockPlayer(
        PlayerFactory(MockFactories::createMockPlayer)
    );
    
    EXPECT_EQ(registrar.size(), 1);
}