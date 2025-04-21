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
  
  std::filesystem::path inputPath(inputFilePath);
  std::string directory = inputPath.parent_path().string();
  std::string filename = inputPath.filename().string();
  
  // Create output path: [input file path]/output_[input file name].txt
  std::string outputFilePath;
  if (directory.empty()) {
      outputFilePath = "output_" + filename;
  } else {
      outputFilePath = directory + "/" + "output_" + filename;
  }
   
  gameManager.saveResults(outputFilePath);

  std::cout << "Game completed successfully. Results saved to " << outputFilePath << "." << std::endl;
  return 0;
}