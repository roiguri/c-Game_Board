#include <iostream>
#include "game_manager.h"
#include <iostream>
#include <string>

int main() {
  // Path to the board configuration file
  const std::string boardFilePath = "board.txt";
  const std::string outputFilePath = "game_results.txt";
  GameManager gameManager;

  if (!gameManager.initialize(boardFilePath)) {
      std::cerr << "Failed to initialize the game." << std::endl;
      return 1;
  }
  gameManager.runGame();
  gameManager.saveResults(outputFilePath);
  std::cout << "Game completed successfully. Results saved to " << outputFilePath << "." << std::endl;
  return 0;
}