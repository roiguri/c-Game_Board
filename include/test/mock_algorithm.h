#include "algo/algorithm.h"

class MockAlgorithm : public Algorithm {
  public:
      MockAlgorithm() : m_constantAction(Action::None), m_useSequence(false), m_currentIndex(0) {}
      explicit MockAlgorithm(Action action) : m_constantAction(action), m_useSequence(false), m_currentIndex(0) {}
      explicit MockAlgorithm(const std::vector<Action>& sequence) 
          : m_constantAction(Action::None), m_actionSequence(sequence), m_useSequence(true), m_currentIndex(0) {}
      ~MockAlgorithm() override {}
  
      // Mocked methods
      Action getNextAction(
        const GameBoard& /*gameBoard*/,
        const Tank& /*myTank*/,
        const Tank& /*enemyTank*/,
        const std::vector<Shell>& /*shells*/
      ) override {
          if (!m_useSequence) {
              return m_constantAction;
          }
          
          if (m_actionSequence.empty()) {
              return Action::None;
          }
          
          if (m_currentIndex >= m_actionSequence.size()) {
              m_currentIndex = 0; // Loop back to beginning
          }
          
          return m_actionSequence[m_currentIndex++];
      }
  
      // set the actions
      void setConstantAction(Action action) {
        m_constantAction = action;
        m_useSequence = false;
      }
  
      void setActionSequence(const std::vector<Action>& sequence) {
        m_actionSequence = sequence;
        m_useSequence = true;
        m_currentIndex = 0;
      }
    private:
      Action m_constantAction;
      std::vector<Action> m_actionSequence;
      bool m_useSequence;
      size_t m_currentIndex;
  };