#pragma once

#include <vector>
#include <stdexcept>

#include "common/TankAlgorithm.h"
#include "common/ActionRequest.h"
#include "common/BattleInfo.h"

namespace GameManager_098765432_123456789 {

/**
 * @brief Enhanced mock algorithm with comprehensive state tracking and configurable behavior
 */
class EnhancedMockAlgorithm : public TankAlgorithm {
public:
    EnhancedMockAlgorithm() 
        : m_constantAction(ActionRequest::DoNothing)
        , m_useSequence(false)
        , m_currentIndex(0)
        , m_getActionCallCount(0)
        , m_updateBattleInfoCallCount(0)
        , m_lastBattleInfo(nullptr) {}

    explicit EnhancedMockAlgorithm(ActionRequest action) 
        : m_constantAction(action)
        , m_useSequence(false)
        , m_currentIndex(0)
        , m_getActionCallCount(0)
        , m_updateBattleInfoCallCount(0)
        , m_lastBattleInfo(nullptr) {}

    explicit EnhancedMockAlgorithm(const std::vector<ActionRequest>& sequence) 
        : m_constantAction(ActionRequest::DoNothing)
        , m_actionSequence(sequence)
        , m_useSequence(true)
        , m_currentIndex(0)
        , m_getActionCallCount(0)
        , m_updateBattleInfoCallCount(0)
        , m_lastBattleInfo(nullptr) {}

    virtual ~EnhancedMockAlgorithm() = default;

    // Implement TankAlgorithm interface
    ActionRequest getAction() override {
        m_getActionCallCount++;

        // Return action based on configuration
        if (!m_useSequence) {
            return m_constantAction;
        }
        
        if (m_actionSequence.empty()) {
            return ActionRequest::DoNothing;
        }
        
        if (m_currentIndex >= m_actionSequence.size()) {
            if (m_loopSequence) {
                m_currentIndex = 0; // Loop back to beginning
            } else {
                return ActionRequest::DoNothing; // Stay at end
            }
        }
        
        return m_actionSequence[m_currentIndex++];
    }

    void updateBattleInfo(BattleInfo& info) override {
        m_updateBattleInfoCallCount++;
        m_lastBattleInfo = &info;
        
        // Store battle info for later inspection if needed
        // Note: We store pointer, caller must ensure lifetime
    }

    // Configuration methods
    void setConstantAction(ActionRequest action) {
        m_constantAction = action;
        m_useSequence = false;
    }

    void setActionSequence(const std::vector<ActionRequest>& sequence, bool loop = true) {
        m_actionSequence = sequence;
        m_useSequence = true;
        m_currentIndex = 0;
        m_loopSequence = loop;
    }



    // State tracking methods
    int getActionCallCount() const { return m_getActionCallCount; }
    int updateBattleInfoCallCount() const { return m_updateBattleInfoCallCount; }
    const BattleInfo* getLastBattleInfo() const { return m_lastBattleInfo; }

    // Reset state
    void resetCounters() {
        m_getActionCallCount = 0;
        m_updateBattleInfoCallCount = 0;
        m_lastBattleInfo = nullptr;
    }

    void resetSequence() {
        m_currentIndex = 0;
    }

    // Inspection methods
    size_t getCurrentSequenceIndex() const { return m_currentIndex; }
    bool isUsingSequence() const { return m_useSequence; }
    ActionRequest getCurrentConstantAction() const { return m_constantAction; }
    const std::vector<ActionRequest>& getActionSequence() const { return m_actionSequence; }

private:
    ActionRequest m_constantAction;
    std::vector<ActionRequest> m_actionSequence;
    bool m_useSequence;
    size_t m_currentIndex;
    bool m_loopSequence = true;

    // State tracking
    int m_getActionCallCount;
    int m_updateBattleInfoCallCount;
    const BattleInfo* m_lastBattleInfo;
};

/**
 * @brief Simple mock algorithm that always returns the same action
 */
class SimpleMockAlgorithm : public TankAlgorithm {
public:
    explicit SimpleMockAlgorithm(ActionRequest action = ActionRequest::DoNothing) 
        : m_action(action), m_callCount(0) {}

    ActionRequest getAction() override {
        m_callCount++;
        return m_action;
    }

    void updateBattleInfo(BattleInfo& /*info*/) override {
        // No-op
    }

    void setAction(ActionRequest action) { m_action = action; }
    int getCallCount() const { return m_callCount; }
    void resetCallCount() { m_callCount = 0; }

private:
    ActionRequest m_action;
    int m_callCount;
};

} // namespace GameManager_098765432_123456789 