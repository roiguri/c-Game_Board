#include <iostream>
#include "game_manager.h"
#include <iostream>
#include <string>

class MockAlgorithm : public Algorithm {
  public:
      MockAlgorithm(int someting) : m_nextAction(Action::Shoot) {
        std::cout << "MockAlgorithm constructor" << std::endl;
      }
      
      Action getNextAction(
          const GameBoard& gameBoard,
          const Tank& myTank,
          const Tank& enemyTank,
          const std::vector<Shell>& shells
      ) override {
          std::cout << "reached MockAlgorithm::getNextAction" << std::endl;
          return m_nextAction;
      }
      
      void setNextAction(Action action) {
          m_nextAction = action;
      }
      
  private:
      Action m_nextAction;
};

int main() {
  // Path to the board configuration file
  const std::string boardFilePath = "board.txt";
  const std::string outputFilePath = "game_results.txt";

  // Create game manager
  GameManager gameManager;
  Algorithm* algo1 = new MockAlgorithm(1);
  Algorithm* algo2 = new MockAlgorithm(2);
  
  // Initialize the game with the board file
  if (!gameManager.initialize(boardFilePath, nullptr, nullptr)) {
    std::cerr << "Failed to initialize game" << std::endl;
    return 1;
  }
  gameManager.createAlgorithms(algo1, algo2);
  std::cout << "player next action:" << gameManager.getPlayerAction(1) << std::endl;
  std::cout << "player next action:" << gameManager.getPlayerAction(2) << std::endl;

  
  std::cout << "Game simulation completed. Results saved to " << outputFilePath << std::endl;
  return 0;
}