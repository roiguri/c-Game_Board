#pragma once

#include <memory>
#include <functional>
#include <map>
#include <stdexcept>

#include "common/TankAlgorithm.h"
#include "common/AbstractGameManager.h"
#include "enhanced_mock_algorithm.h"

namespace GameManager_098765432_123456789 {

/**
 * @brief Global registry for tracking factory calls and configured algorithms
 */
class MockFactoryRegistry {
public:
    static MockFactoryRegistry& getInstance() {
        static MockFactoryRegistry instance;
        return instance;
    }

    void recordFactoryCall(const std::string& factoryName, int playerIndex, int tankIndex) {
        m_factoryCalls[factoryName].push_back({playerIndex, tankIndex});
    }

    void setAlgorithmForFactory(const std::string& factoryName, int playerIndex, int tankIndex, 
                               std::shared_ptr<EnhancedMockAlgorithm> algorithm) {
        m_configuredAlgorithms[factoryName][{playerIndex, tankIndex}] = algorithm;
    }

    std::shared_ptr<EnhancedMockAlgorithm> getAlgorithmForFactory(const std::string& factoryName, 
                                                                  int playerIndex, int tankIndex) {
        auto factoryIt = m_configuredAlgorithms.find(factoryName);
        if (factoryIt != m_configuredAlgorithms.end()) {
            auto algoIt = factoryIt->second.find({playerIndex, tankIndex});
            if (algoIt != factoryIt->second.end()) {
                return algoIt->second;
            }
        }
        return nullptr;
    }

    const std::vector<std::pair<int, int>>& getFactoryCalls(const std::string& factoryName) const {
        static const std::vector<std::pair<int, int>> empty;
        auto it = m_factoryCalls.find(factoryName);
        return (it != m_factoryCalls.end()) ? it->second : empty;
    }

    void reset() {
        m_factoryCalls.clear();
        m_configuredAlgorithms.clear();
    }

private:
    std::map<std::string, std::vector<std::pair<int, int>>> m_factoryCalls;
    std::map<std::string, std::map<std::pair<int, int>, std::shared_ptr<EnhancedMockAlgorithm>>> m_configuredAlgorithms;
};

// Mock factory function that creates simple DoNothing algorithms
inline std::unique_ptr<TankAlgorithm> mockFactoryDoNothing(int playerIndex, int tankIndex) {
    MockFactoryRegistry::getInstance().recordFactoryCall("mockFactoryDoNothing", playerIndex, tankIndex);
    
    // Check if we have a pre-configured algorithm
    auto configured = MockFactoryRegistry::getInstance().getAlgorithmForFactory("mockFactoryDoNothing", playerIndex, tankIndex);
    if (configured) {
        // Create a copy since we need unique ownership
        return std::make_unique<EnhancedMockAlgorithm>(*configured);
    }
    
    return std::make_unique<EnhancedMockAlgorithm>(ActionRequest::DoNothing);
}

// Mock factory function that creates shooting algorithms
inline std::unique_ptr<TankAlgorithm> mockFactoryShooter(int playerIndex, int tankIndex) {
    MockFactoryRegistry::getInstance().recordFactoryCall("mockFactoryShooter", playerIndex, tankIndex);
    
    auto configured = MockFactoryRegistry::getInstance().getAlgorithmForFactory("mockFactoryShooter", playerIndex, tankIndex);
    if (configured) {
        return std::make_unique<EnhancedMockAlgorithm>(*configured);
    }
    
    return std::make_unique<EnhancedMockAlgorithm>(ActionRequest::Shoot);
}



/**
 * @brief Create a custom mock factory that returns algorithms with a specific action sequence
 * @param factoryName Unique name for this factory (for tracking)
 * @param sequence Action sequence the algorithms should execute
 * @param loop Whether the sequence should loop (default: true)
 * @return Factory function that creates algorithms with the specified sequence
 */
inline TankAlgorithmFactory createMockFactoryWithSequence(
    const std::string& factoryName,
    const std::vector<ActionRequest>& sequence,
    bool loop = true) {
    
    return [factoryName, sequence, loop](int playerIndex, int tankIndex) -> std::unique_ptr<TankAlgorithm> {
        MockFactoryRegistry::getInstance().recordFactoryCall(factoryName, playerIndex, tankIndex);
        
        // Check if we have a pre-configured algorithm
        auto configured = MockFactoryRegistry::getInstance().getAlgorithmForFactory(factoryName, playerIndex, tankIndex);
        if (configured) {
            return std::make_unique<EnhancedMockAlgorithm>(*configured);
        }
        
        // Create algorithm with the specified sequence
        auto algorithm = std::make_unique<EnhancedMockAlgorithm>(sequence);
        // Note: EnhancedMockAlgorithm constructor with sequence sets looping by default
        // If we need to control looping, we'd need to add a method to set it
        return algorithm;
    };
}

/**
 * @brief Create a custom mock factory that returns algorithms with a single constant action
 * @param factoryName Unique name for this factory (for tracking)
 * @param action Action the algorithms should always return
 * @return Factory function that creates algorithms with the specified constant action
 */
inline TankAlgorithmFactory createMockFactoryWithAction(
    const std::string& factoryName,
    ActionRequest action) {
    
    return [factoryName, action](int playerIndex, int tankIndex) -> std::unique_ptr<TankAlgorithm> {
        MockFactoryRegistry::getInstance().recordFactoryCall(factoryName, playerIndex, tankIndex);
        
        // Check if we have a pre-configured algorithm
        auto configured = MockFactoryRegistry::getInstance().getAlgorithmForFactory(factoryName, playerIndex, tankIndex);
        if (configured) {
            return std::make_unique<EnhancedMockAlgorithm>(*configured);
        }
        
        // Create algorithm with the specified constant action
        return std::make_unique<EnhancedMockAlgorithm>(action);
    };
}

/**
 * @brief Helper class for configuring mock factories before tests
 */
class MockFactoryConfigurer {
public:
    static void setAlgorithmForFactory(const std::string& factoryName, int playerIndex, int tankIndex,
                                       std::shared_ptr<EnhancedMockAlgorithm> algorithm) {
        MockFactoryRegistry::getInstance().setAlgorithmForFactory(factoryName, playerIndex, tankIndex, algorithm);
    }
    
    static void resetAll() {
        MockFactoryRegistry::getInstance().reset();
    }
    
    static const std::vector<std::pair<int, int>>& getFactoryCalls(const std::string& factoryName) {
        return MockFactoryRegistry::getInstance().getFactoryCalls(factoryName);
    }
};

} // namespace GameManager_098765432_123456789 