#include "TankAlgorithm.h"
#include <vector>

class MockAlgorithm : public TankAlgorithm {
  public:
      MockAlgorithm() : m_constantAction(ActionRequest::DoNothing), m_useSequence(false), m_currentIndex(0) {}
      explicit MockAlgorithm(ActionRequest action) : m_constantAction(action), m_useSequence(false), m_currentIndex(0) {}
      explicit MockAlgorithm(const std::vector<ActionRequest>& sequence) 
          : m_constantAction(ActionRequest::DoNothing), m_actionSequence(sequence), m_useSequence(true), m_currentIndex(0) {}
      ~MockAlgorithm() override {}
  
      // Implement TankAlgorithm interface
      ActionRequest getAction() override {
          if (!m_useSequence) {
              return m_constantAction;
          }
          if (m_actionSequence.empty()) {
              return ActionRequest::DoNothing;
          }
          if (m_currentIndex >= m_actionSequence.size()) {
              m_currentIndex = 0; // Loop back to beginning
          }
          return m_actionSequence[m_currentIndex++];
      }
  
      void updateBattleInfo(BattleInfo& /*info*/) override {
          // No-op for mock
      }
  
      // set the actions
      void setConstantAction(ActionRequest action) {
        m_constantAction = action;
        m_useSequence = false;
      }
  
      void setActionSequence(const std::vector<ActionRequest>& sequence) {
        m_actionSequence = sequence;
        m_useSequence = true;
        m_currentIndex = 0;
      }
    private:
      ActionRequest m_constantAction;
      std::vector<ActionRequest> m_actionSequence;
      bool m_useSequence;
      size_t m_currentIndex;
  };

// Factory for creating MockAlgorithm instances
#include "TankAlgorithmFactory.h"
class MockAlgorithmFactory : public TankAlgorithmFactory {
public:
    std::unique_ptr<TankAlgorithm> create(int /*player_index*/, int /*tank_index*/) const override {
        return std::make_unique<MockAlgorithm>();
    }
};