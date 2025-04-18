#include "algo/algorithm.h"
#include <iostream>
#include "algo/chase_algorithm.h"
#include "algo/defensive_algorithm.h"

const int NUM_DIRECTIONS = sizeof(ALL_DIRECTIONS) / sizeof(ALL_DIRECTIONS[0]);

// TODO: not implemented
Algorithm::Algorithm() {
}

Algorithm::~Algorithm() {
}

Algorithm* Algorithm::createAlgorithm(const std::string& type) {
    if (type == "chase") {
      return new ChaseAlgorithm();
    } else if (type == "defensive") {
      return new DefensiveAlgorithm();
    }
    return nullptr;
}

// TODO: consider changing to return boolean
Direction* Algorithm::hasDirectLineOfSight(
  const GameBoard& gameBoard,
  const Point& from,
  const Point& to) const
{
  if (from == to) {
      return new Direction(Direction::Up); // Any direction is fine when points are the same
  }

  // TODO: replace with enum iteration if possible
  for (int i = 0; i < NUM_DIRECTIONS; ++i) {
      Direction current_dir = ALL_DIRECTIONS[i];
      if (hasLineOfSightInDirection(gameBoard, from, to, current_dir)) {
          return new Direction(current_dir);
      }
  }
  return nullptr;
}

bool Algorithm::hasLineOfSightInDirection(
  const GameBoard& gameBoard,
  const Point& from,
  const Point& to,
  Direction direction) const
{
  if (from == to) {
      return true;
  }

  Point stepDelta = getDirectionDelta(direction);
  Point currentPoint = from;
  
  while (true) {
      currentPoint = currentPoint + stepDelta;
      currentPoint = gameBoard.wrapPosition(currentPoint);
      
      if (currentPoint == to) {
          return true;
      }
      if (gameBoard.isWall(currentPoint)) {
          return false;
      }
      
      if (currentPoint == from) {
          // Back to the start without hitting a wall or reaching the target
          return false;
      }
  }
}

bool Algorithm::isPositionInDangerFromShells(
  const GameBoard& gameBoard,
  const Point& position,
  const std::vector<Shell>& shells,
  int stepsToCheck) const
{
  if (shells.empty()) {
      return false;
  }

  // TODO: consider moving to next shell if shell belongs to the same tank
  for (const Shell& shell : shells) {
      if (shell.isDestroyed()) {
          continue;
      }

      if (shell.getPosition() == position) {
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

              if (shellPosition == position) {
                  return true;
              }
          }
      }
      next_shell:;
  }

  return false;
}

// TODO: should we change stepsToCheck to const imediate threat
// TODO: possible improvement: if tank facing the tank - shoot
Action Algorithm::findSafeAction(
  const GameBoard& gameBoard,
  const Tank& tank,
  const std::vector<Shell>& shells,
  int stepsToCheck
) const {
  if (tank.isDestroyed() || shells.empty()) {
      return Action::None;
  }
  
  if (tank.isMovingBackward()) {
      Point backwardPos = tank.getNextBackwardPosition();
      if (!isPositionInDangerFromShells(gameBoard, backwardPos, shells, stepsToCheck)) {
          return Action::None; // TODO: check if need to move backward again.
      }
  }
  
  if (!isPositionInDangerFromShells(gameBoard, tank.getPosition(), shells, stepsToCheck)) {
      return Action::None;
  }
  
  Point forwardPos = tank.getNextForwardPosition();
  if (gameBoard.canMoveTo(forwardPos) && 
      !isPositionInDangerFromShells(gameBoard, forwardPos, shells, stepsToCheck)) {
      return Action::MoveForward;
  }
  // TODO: Do we have enought steps to move backward?
  Point backwardPos = tank.getNextBackwardPosition();
  if (gameBoard.canMoveTo(backwardPos) && 
      !isPositionInDangerFromShells(gameBoard, backwardPos, shells, stepsToCheck)) {
      return Action::MoveBackward;
  }
  // TODO: is this the best way to handle this? check if tank has enough steps.
  for (int i = 0; i < NUM_DIRECTIONS; ++i) {
      Direction targetDir = ALL_DIRECTIONS[i];
      if (targetDir == tank.getDirection()) {
          continue;
      }
      
      Point targetPos = tank.getPosition() + getDirectionDelta(targetDir);
      targetPos = gameBoard.wrapPosition(targetPos);
      if (!gameBoard.canMoveTo(targetPos) || 
          isPositionInDangerFromShells(gameBoard, targetPos, shells, stepsToCheck)) {
          continue;
      }
      
      return getFirstRotationAction(tank.getDirection(), targetDir);
  }
  std::cout << "No safe action found" << std::endl;
  return Action::None;
}

Action Algorithm::getFirstRotationAction(Direction currentDir, Direction targetDir) const {
  std::cout << "Finding first rotation action from " << static_cast<int>(currentDir) 
            << " to " << static_cast<int>(targetDir) << std::endl;
  if (currentDir == targetDir) {
      return Action::None;
  }
  
  // One step rotations
  if (targetDir == rotateRight(currentDir, false)) {
      return Action::RotateRightEighth;
  }
  if (targetDir == rotateLeft(currentDir, false)) {
      return Action::RotateLeftEighth;
  }
  if (targetDir == rotateRight(currentDir, true)) {
      return Action::RotateRightQuarter;
  }
  if (targetDir == rotateLeft(currentDir, true)) {
      return Action::RotateLeftQuarter;
  }
  
  // Two step rotations
  int stepsClockwise = 0;
  int stepsCounterClockwise = 0;
  
  // Count steps in each direction
  Direction tempDir = currentDir;
  while (tempDir != targetDir && stepsClockwise < 4) {
      tempDir = rotateRight(tempDir, false);
      stepsClockwise++;
  }
  
  tempDir = currentDir;
  while (tempDir != targetDir && stepsCounterClockwise < 4) {
      tempDir = rotateLeft(tempDir, false);
      stepsCounterClockwise++;
  }
  
  // Choose the direction with fewer steps
  bool rotateClockwise = (stepsClockwise <= stepsCounterClockwise);
  
  // Return the appropriate first rotation
  if (rotateClockwise) {
      return (stepsClockwise >= 2 && stepsClockwise <= 3) ? 
             Action::RotateRightQuarter : Action::RotateRightEighth;
  } else {
      return (stepsCounterClockwise >= 2 && stepsCounterClockwise <= 3) ? 
             Action::RotateLeftQuarter : Action::RotateLeftEighth;
  }
}

bool Algorithm::canShootEnemy(
  const GameBoard& gameBoard,
  const Tank& myTank,
  const Tank& enemyTank
) const {
  if (enemyTank.isDestroyed() || !myTank.canShoot()) {
      return false;
  }
  
  return hasLineOfSightInDirection(
    gameBoard,
    myTank.getPosition(),
    enemyTank.getPosition(),
    myTank.getDirection()
  );
}
