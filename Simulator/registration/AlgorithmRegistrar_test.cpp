#include <gtest/gtest.h>
#include "AlgorithmRegistrar.h"
#include "common/Player.h"
#include "common/TankAlgorithm.h"
#include "test/mocks/MockFactories.h"

class AlgorithmRegistrarTest : public ::testing::Test {
protected:
    void SetUp() override {
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    }
    
    void TearDown() override {
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    }  
};

TEST_F(AlgorithmRegistrarTest, GetSingleton) {
    auto& registrar1 = AlgorithmRegistrar::getAlgorithmRegistrar();
    auto& registrar2 = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    EXPECT_EQ(&registrar1, &registrar2);
}

TEST_F(AlgorithmRegistrarTest, CreateAlgorithmFactoryEntry) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    EXPECT_EQ(registrar.size(), 0);
    
    registrar.createAlgorithmFactoryEntry("test_algorithm");
    EXPECT_EQ(registrar.size(), 1);
}

TEST_F(AlgorithmRegistrarTest, AddPlayerFactoryToLastEntry) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    registrar.createAlgorithmFactoryEntry("test_algorithm");
    
    PlayerFactory playerFactory = MockFactories::createNullPlayer;
    registrar.addPlayerFactoryToLastEntry(std::move(playerFactory));
    
    EXPECT_EQ(registrar.size(), 1);
}

TEST_F(AlgorithmRegistrarTest, AddTankAlgorithmFactoryToLastEntry) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    registrar.createAlgorithmFactoryEntry("test_algorithm");
    
    TankAlgorithmFactory tankFactory = MockFactories::createNullTankAlgorithm;
    registrar.addTankAlgorithmFactoryToLastEntry(std::move(tankFactory));
    
    EXPECT_EQ(registrar.size(), 1);
}

TEST_F(AlgorithmRegistrarTest, ValidateLastRegistration_Complete) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    registrar.createAlgorithmFactoryEntry("test_algorithm");
    
    PlayerFactory playerFactory = MockFactories::createNullPlayer;
    TankAlgorithmFactory tankFactory = MockFactories::createNullTankAlgorithm;
    
    registrar.addPlayerFactoryToLastEntry(std::move(playerFactory));
    registrar.addTankAlgorithmFactoryToLastEntry(std::move(tankFactory));
    
    EXPECT_NO_THROW(registrar.validateLastRegistration());
    EXPECT_EQ(registrar.size(), 1);
}

TEST_F(AlgorithmRegistrarTest, ValidateLastRegistration_Incomplete_ThrowsException) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    registrar.createAlgorithmFactoryEntry("test_algorithm");
    
    PlayerFactory playerFactory = MockFactories::createNullPlayer;
    registrar.addPlayerFactoryToLastEntry(std::move(playerFactory));
    
    EXPECT_THROW(registrar.validateLastRegistration(), BadRegistrationException);
    EXPECT_EQ(registrar.size(), 0); // Should be removed after failed validation
}

TEST_F(AlgorithmRegistrarTest, ValidateLastRegistration_NoEntry_ThrowsException) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    EXPECT_THROW(registrar.validateLastRegistration(), BadRegistrationException);
}

TEST_F(AlgorithmRegistrarTest, AddFactoryToEmptyRegistrar_ThrowsException) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    PlayerFactory playerFactory = MockFactories::createNullPlayer;
    EXPECT_THROW(registrar.addPlayerFactoryToLastEntry(std::move(playerFactory)), BadRegistrationException);
    
    TankAlgorithmFactory tankFactory = MockFactories::createNullTankAlgorithm;
    EXPECT_THROW(registrar.addTankAlgorithmFactoryToLastEntry(std::move(tankFactory)), BadRegistrationException);
}

TEST_F(AlgorithmRegistrarTest, RemoveLast) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    registrar.createAlgorithmFactoryEntry("test_algorithm1");
    registrar.createAlgorithmFactoryEntry("test_algorithm2");
    EXPECT_EQ(registrar.size(), 2);
    
    registrar.removeLast();
    EXPECT_EQ(registrar.size(), 1);
    
    registrar.removeLast();
    EXPECT_EQ(registrar.size(), 0);
    
    // Should not throw on empty registrar
    EXPECT_NO_THROW(registrar.removeLast());
}

TEST_F(AlgorithmRegistrarTest, IteratorSupport) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    registrar.createAlgorithmFactoryEntry("algorithm1");
    PlayerFactory playerFactory1 = MockFactories::createNullPlayer;
    TankAlgorithmFactory tankFactory1 = MockFactories::createNullTankAlgorithm;
    registrar.addPlayerFactoryToLastEntry(std::move(playerFactory1));
    registrar.addTankAlgorithmFactoryToLastEntry(std::move(tankFactory1));
    registrar.validateLastRegistration();
    
    registrar.createAlgorithmFactoryEntry("algorithm2");
    PlayerFactory playerFactory2 = MockFactories::createNullPlayer;
    TankAlgorithmFactory tankFactory2 = MockFactories::createNullTankAlgorithm;
    registrar.addPlayerFactoryToLastEntry(std::move(playerFactory2));
    registrar.addTankAlgorithmFactoryToLastEntry(std::move(tankFactory2));
    registrar.validateLastRegistration();
    
    int count = 0;
    for ([[maybe_unused]] const auto& entry : registrar) {
        count++;
    }
    
    EXPECT_EQ(count, 2);
    EXPECT_EQ(registrar.size(), 2);
    EXPECT_FALSE(registrar.empty());
}