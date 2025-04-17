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

bool Algorithm::isInDangerFromShells(
  const GameBoard& gameBoard,
  const Tank& tank,
  const std::vector<Shell>& shells,
  int stepsToCheck) const
{
  if (tank.isDestroyed() || shells.empty()) {
      return false;
  }

  Point tankPosition = tank.getPosition();
  // TODO: consider moving to next shell if shell belongs to the same tank
  for (const Shell& shell : shells) {
      if (shell.isDestroyed()) {
          continue;
      }

      if (shell.getPosition() == tankPosition) {
          return true;
      }

      Point shellPosition = shell.getPosition();
      Direction shellDirection = shell.getDirection();
      Point shellDelta = getDirectionDelta(shellDirection);

      for (int step = 0; step < stepsToCheck; ++step) {
          for (int pace = 0; pace < 2; ++pace) {
              shellPosition = shellPosition + shellDelta;
              shellPosition = gameBoard.wrapPosition(shellPosition);

              // Check if shell hits a wall
              if (gameBoard.isWall(shellPosition)) {
                  // Shell would be destroyed by the wall
                  goto next_shell;
              }

              // Check if shell hits the tank
              if (shellPosition == tankPosition) {
                  return true;
              }
          }
      }
      next_shell:;
  }

  return false;
}

