#include <gtest/gtest.h>
#include "registrar_test_helpers.h"
#include "registration/AlgorithmRegistrar.h"
#include "registration/GameManagerRegistrar.h"

class RegistrarTestHelpersTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegistrarTestHelpers::clearAllRegistrars();
    }
    
    void TearDown() override {
        RegistrarTestHelpers::clearAllRegistrars();
    }
};

TEST_F(RegistrarTestHelpersTest, ClearAllRegistrars) {
    // Arrange - Setup some data first
    RegistrarTestHelpers::setupMockRegistrars();
    
    auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    EXPECT_GT(gameManagerRegistrar.count(), 0);
    EXPECT_GT(algorithmRegistrar.size(), 0);
    
    // Act
    RegistrarTestHelpers::clearAllRegistrars();
    
    // Assert
    EXPECT_EQ(gameManagerRegistrar.count(), 0);
    EXPECT_EQ(algorithmRegistrar.size(), 0);
}

TEST_F(RegistrarTestHelpersTest, SetupMockRegistrars) {
    RegistrarTestHelpers::setupMockRegistrars();
    
    auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    EXPECT_EQ(gameManagerRegistrar.count(), 1);
    EXPECT_EQ(algorithmRegistrar.size(), 2);
    
    // Verify we can find the registered entries
    EXPECT_NE(gameManagerRegistrar.findByName("TestGameManager"), nullptr);
    
    // Verify algorithms are complete
    for (const auto& entry : algorithmRegistrar) {
        EXPECT_TRUE(entry.isComplete());
    }
}

TEST_F(RegistrarTestHelpersTest, SetupIncompleteAlgorithm) {
    RegistrarTestHelpers::setupIncompleteAlgorithm();
    
    auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    EXPECT_EQ(algorithmRegistrar.size(), 1);
    
    // Find the incomplete algorithm
    bool foundIncomplete = false;
    for (const auto& entry : algorithmRegistrar) {
        if (entry.getName() == "IncompleteAlgo") {
            foundIncomplete = true;
            EXPECT_FALSE(entry.isComplete());
        }
    }
    EXPECT_TRUE(foundIncomplete);
}

TEST_F(RegistrarTestHelpersTest, SetupGameManagerRegistrar) {
    RegistrarTestHelpers::setupGameManagerRegistrar("CustomGM");
    
    auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    EXPECT_EQ(gameManagerRegistrar.count(), 1);
    EXPECT_EQ(algorithmRegistrar.size(), 0); // Should not affect algorithm registrar
    
    EXPECT_NE(gameManagerRegistrar.findByName("CustomGM"), nullptr);
}

TEST_F(RegistrarTestHelpersTest, SetupCompleteAlgorithm) {
    RegistrarTestHelpers::setupCompleteAlgorithm("CustomAlgo");
    
    auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    EXPECT_EQ(gameManagerRegistrar.count(), 0); // Should not affect game manager registrar
    EXPECT_EQ(algorithmRegistrar.size(), 1);
    
    // Verify algorithm is complete
    bool foundCustom = false;
    for (const auto& entry : algorithmRegistrar) {
        if (entry.getName() == "CustomAlgo") {
            foundCustom = true;
            EXPECT_TRUE(entry.isComplete());
        }
    }
    EXPECT_TRUE(foundCustom);
}

TEST_F(RegistrarTestHelpersTest, RegistrarsStartEmpty) {
    auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    EXPECT_EQ(gameManagerRegistrar.count(), 0);
    EXPECT_EQ(algorithmRegistrar.size(), 0);
}