#pragma once

#include "registration/AlgorithmRegistrar.h"
#include "registration/GameManagerRegistrar.h"
#include "test/mocks/MockFactories.h"
#include "test/mocks/MockGameManager.h"
#include <memory>

/**
 * @brief Helper utilities for setting up and managing registrars in tests
 */
class RegistrarTestHelpers {
public:
    /**
     * @brief Clear all registrars for test isolation
     * Should be called in test SetUp/TearDown methods
     */
    static void clearAllRegistrars() {
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
        GameManagerRegistrar::getGameManagerRegistrar().clear();
    }
    
    /**
     * @brief Setup mock registrars with complete test data
     * Sets up one GameManager and two complete algorithms for testing
     */
    static void setupMockRegistrars() {
        auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
        auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        
        // Register mock GameManager
        gameManagerRegistrar.createGameManagerEntry("TestGameManager");
        gameManagerRegistrar.addGameManagerFactoryToLastEntry(
            [](bool verbose) { return std::make_unique<MockGameManager>(verbose); }
        );
        gameManagerRegistrar.validateLastRegistration();
        
        // Register first algorithm
        algorithmRegistrar.createAlgorithmFactoryEntry("TestAlgo1");
        algorithmRegistrar.addPlayerFactoryToLastEntry(MockFactories::createMockPlayer);
        algorithmRegistrar.addTankAlgorithmFactoryToLastEntry(MockFactories::createMockTankAlgorithm);
        algorithmRegistrar.validateLastRegistration();
        
        // Register second algorithm
        algorithmRegistrar.createAlgorithmFactoryEntry("TestAlgo2");
        algorithmRegistrar.addPlayerFactoryToLastEntry(MockFactories::createMockPlayer);
        algorithmRegistrar.addTankAlgorithmFactoryToLastEntry(MockFactories::createMockTankAlgorithm);
        algorithmRegistrar.validateLastRegistration();
    }
    
    /**
     * @brief Setup incomplete algorithm for error testing
     * Creates an algorithm entry with only Player factory (missing TankAlgorithm)
     */
    static void setupIncompleteAlgorithm() {
        auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        
        algorithmRegistrar.createAlgorithmFactoryEntry("IncompleteAlgo");
        algorithmRegistrar.addPlayerFactoryToLastEntry(MockFactories::createMockPlayer);
        // Missing: addTankAlgorithmFactoryToLastEntry
        // Don't call validateLastRegistration() - keep it incomplete
    }
    
    /**
     * @brief Setup GameManager registrar only
     * @param name Name for the GameManager entry
     */
    static void setupGameManagerRegistrar(const std::string& name = "TestGameManager") {
        auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
        
        gameManagerRegistrar.createGameManagerEntry(name);
        gameManagerRegistrar.addGameManagerFactoryToLastEntry(
            [](bool verbose) { return std::make_unique<MockGameManager>(verbose); }
        );
        gameManagerRegistrar.validateLastRegistration();
    }
    
    /**
     * @brief Setup complete algorithm registrar entry
     * @param name Name for the algorithm entry
     */
    static void setupCompleteAlgorithm(const std::string& name) {
        auto& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        
        algorithmRegistrar.createAlgorithmFactoryEntry(name);
        algorithmRegistrar.addPlayerFactoryToLastEntry(MockFactories::createMockPlayer);
        algorithmRegistrar.addTankAlgorithmFactoryToLastEntry(MockFactories::createMockTankAlgorithm);
        algorithmRegistrar.validateLastRegistration();
    }
};