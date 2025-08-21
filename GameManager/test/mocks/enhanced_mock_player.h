#pragma once

#include <gmock/gmock.h>
#include <functional>
#include <stdexcept>

#include "common/Player.h"
#include "common/TankAlgorithm.h"
#include "common/SatelliteView.h"

namespace GameManager_318835816_211314471 {

/**
 * @brief Enhanced mock player with configurable behavior for comprehensive testing
 */
class ConfigurableMockPlayer : public Player {
public:
    ConfigurableMockPlayer() = default;
    virtual ~ConfigurableMockPlayer() = default;

    // GMock method declaration
    MOCK_METHOD(void, updateTankWithBattleInfo, (TankAlgorithm&, SatelliteView&), (override));

    /**
     * @brief Configure custom behavior for updateTankWithBattleInfo
     * @param behavior Custom function to execute when method is called
     */
    void setUpdateBehavior(std::function<void(TankAlgorithm&, SatelliteView&)> behavior) {
        m_customBehavior = behavior;
        ON_CALL(*this, updateTankWithBattleInfo(::testing::_, ::testing::_))
            .WillByDefault([this](TankAlgorithm& algo, SatelliteView& view) {
                if (m_customBehavior) {
                    m_customBehavior(algo, view);
                }
                m_updateCallCount++;
            });
    }



    /**
     * @brief Set up default mock behavior (no-op)
     */
    void setDefaultBehavior() {
        setUpdateBehavior(nullptr);
    }

    /**
     * @brief Get number of times updateTankWithBattleInfo was called
     */
    int getUpdateCallCount() const { return m_updateCallCount; }

    /**
     * @brief Reset call counters
     */
    void resetCounters() { m_updateCallCount = 0; }

private:
    std::function<void(TankAlgorithm&, SatelliteView&)> m_customBehavior;
    int m_updateCallCount = 0;
};

/**
 * @brief Simple mock player that just increments call count
 */
class SimpleMockPlayer : public Player {
public:
    SimpleMockPlayer() = default;
    virtual ~SimpleMockPlayer() = default;

    void updateTankWithBattleInfo(TankAlgorithm& /*tank*/, SatelliteView& /*satellite_view*/) override {
        m_callCount++;
    }

    int getCallCount() const { return m_callCount; }
    void resetCallCount() { m_callCount = 0; }

private:
    int m_callCount = 0;
};



} // namespace GameManager_318835816_211314471 