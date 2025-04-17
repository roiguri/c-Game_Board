#include "algo/algorithm.h"

static const Direction ALL_DIRECTIONS[] = {
  Direction::Up, Direction::UpRight, Direction::Right, Direction::DownRight,
  Direction::Down, Direction::DownLeft, Direction::Left, Direction::UpLeft
};
const int NUM_DIRECTIONS = sizeof(ALL_DIRECTIONS) / sizeof(ALL_DIRECTIONS[0]);

// TODO: not implemented
Algorithm::Algorithm() {
}

Algorithm::~Algorithm() {
}

Algorithm* Algorithm::createAlgorithm(const std::string& type) {
    if (type == "chase" || type == "defensive") {
        return nullptr;
    }
    
    return nullptr;
}

bool Algorithm::hasDirectLineOfSight(
  const GameBoard& gameBoard,
  const Point& from,
  const Point& to) const
{
  if (from == to) {
      return true;
  }

  for (int i = 0; i < NUM_DIRECTIONS; ++i) {
      Direction current_dir = ALL_DIRECTIONS[i];
      Point stepDelta = getDirectionDelta(current_dir);

      Point currentPoint = from;
      while (true) {
          currentPoint = currentPoint + stepDelta; 
          currentPoint = gameBoard.wrapPosition(currentPoint);

          if (currentPoint == to) {
              return true;
          }
          if (gameBoard.isWall(currentPoint)) {
              break;
          }
          
          if (currentPoint == from) {
               break;
          }
      }
  }
  return false;
}

