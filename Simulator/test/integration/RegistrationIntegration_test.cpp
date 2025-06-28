#include <gtest/gtest.h>
#include "registration/AlgorithmRegistrar.h"
#include "common/PlayerRegistration.h"
#include "common/TankAlgorithmRegistration.h"
#include "test/mocks/MockFactories.h"

class RegistrationIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    }
    
    void TearDown() override {
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    }
    
    void simulateSuccessfulRegistration(const std::string& name) {
        auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        registrar.createAlgorithmFactoryEntry(name);
        
        // Create mock factories
        PlayerFactory playerFactory = MockFactories::createNullPlayer;
        TankAlgorithmFactory tankFactory = MockFactories::createNullTankAlgorithm;
        
        // Simulate registration through the registration structs
        PlayerRegistration playerReg(std::move(playerFactory));
        TankAlgorithmRegistration tankReg(std::move(tankFactory));
        
        registrar.validateLastRegistration();
    }
    
    void simulateFailedRegistration(const std::string& name) {
        auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        registrar.createAlgorithmFactoryEntry(name);
        
        // Only register player, not tank algorithm
        PlayerFactory playerFactory = MockFactories::createNullPlayer;
        
        PlayerRegistration playerReg(std::move(playerFactory));
        
        // This should throw and remove the incomplete registration
        EXPECT_THROW(registrar.validateLastRegistration(), BadRegistrationException);
    }
};

TEST_F(RegistrationIntegrationTest, SuccessfulRegistration) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    EXPECT_NO_THROW(simulateSuccessfulRegistration("algorithm1"));
    EXPECT_EQ(registrar.size(), 1);
}

TEST_F(RegistrationIntegrationTest, FailedRegistration_IncompleteEntry) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    EXPECT_NO_THROW(simulateFailedRegistration("incomplete_algorithm"));
    EXPECT_EQ(registrar.size(), 0); // Should be cleaned up after failed validation
}

TEST_F(RegistrationIntegrationTest, MultipleSuccessfulRegistrations) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    simulateSuccessfulRegistration("algorithm1");
    simulateSuccessfulRegistration("algorithm2");
    simulateSuccessfulRegistration("algorithm3");
    
    EXPECT_EQ(registrar.size(), 3);
}

TEST_F(RegistrationIntegrationTest, MixedRegistrations_SuccessAndFailure) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    simulateSuccessfulRegistration("algorithm1");
    EXPECT_EQ(registrar.size(), 1);
    
    simulateFailedRegistration("incomplete_algorithm");
    EXPECT_EQ(registrar.size(), 1); // Should still have the successful one
    
    simulateSuccessfulRegistration("algorithm2");
    EXPECT_EQ(registrar.size(), 2);
}

TEST_F(RegistrationIntegrationTest, RegistrationOrder_PlayerFirst) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    registrar.createAlgorithmFactoryEntry("test_algorithm");
    
    // Register player first
    PlayerFactory playerFactory = MockFactories::createNullPlayer;
    PlayerRegistration playerReg(std::move(playerFactory));
    
    // Then register tank algorithm
    TankAlgorithmFactory tankFactory = MockFactories::createNullTankAlgorithm;
    TankAlgorithmRegistration tankReg(std::move(tankFactory));
    
    EXPECT_NO_THROW(registrar.validateLastRegistration());
    EXPECT_EQ(registrar.size(), 1);
}

TEST_F(RegistrationIntegrationTest, RegistrationOrder_TankAlgorithmFirst) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    registrar.createAlgorithmFactoryEntry("test_algorithm");
    
    // Register tank algorithm first
    TankAlgorithmFactory tankFactory = MockFactories::createNullTankAlgorithm;
    TankAlgorithmRegistration tankReg(std::move(tankFactory));
    
    // Then register player
    PlayerFactory playerFactory = MockFactories::createNullPlayer;
    PlayerRegistration playerReg(std::move(playerFactory));
    
    EXPECT_NO_THROW(registrar.validateLastRegistration());
    EXPECT_EQ(registrar.size(), 1);
}