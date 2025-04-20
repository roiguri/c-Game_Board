#include "game_manager.h"
#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <game_board_input_file>" << std::endl;
    return 1;
  }

  const std::string inputFilePath = argv[1];
  GameManager gameManager;

  if (!gameManager.initialize(inputFilePath)) {
      return 1;
  }
  
  gameManager.runGame();
  
  // TODO: do we need a specific file name for the output?
  const std::string outputFilePath = "game_results.txt";
  gameManager.saveResults(outputFilePath);

  std::cout << "Game completed successfully. Results saved to " << outputFilePath << "." << std::endl;
  return 0;
}