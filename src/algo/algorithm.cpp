#include "algo/algorithm.h"
#include <iostream>
#include <algorithm>
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

bool Algorithm::isInDanger(const GameBoard& gameBoard, const Tank& tank, 
                         const std::vector<Shell>& shells, int lookAheadSteps) const {
    return isInDanger(gameBoard, tank.getPosition(), shells, lookAheadSteps);
}

bool Algorithm::isInDanger(const GameBoard& gameBoard, const Point& position, 
                         const std::vector<Shell>& shells, int lookAheadSteps) const {
    for (const Shell& shell : shells) {
        if (shell.isDestroyed()) {
            continue;
        }
        
        if (shell.getPosition() == position) {
            return true;
        }
        
        // Predict shell trajectory
        Point shellPos = shell.getPosition();
        Direction shellDir = shell.getDirection();
        // TODO: consider using a constant for the number of steps
        for (int step = 0; step < lookAheadSteps * 2; ++step) {
            shellPos = gameBoard.wrapPosition(shellPos + getDirectionDelta(shellDir));
            if (gameBoard.isWall(shellPos)) {
                break;
            }
            if (shellPos == position) {
                return true;
            }
        }
    }
    return false;
}

Action Algorithm::findOptimalSafeMove(
  const GameBoard& gameBoard, 
  const Tank& tank,                
  const Tank& enemyTank, 
  const std::vector<Shell>& shells, 
  bool avoidEnemySight
) const {
    bool currentPositionSafe = !isInDanger(gameBoard, tank, shells);
      
    bool exposedToEnemy = avoidEnemySight && 
                         isExposedToEnemy(gameBoard, tank.getPosition(), enemyTank);

    if (currentPositionSafe && (!avoidEnemySight || !exposedToEnemy)) {
        return Action::None;
    }
    
    std::vector<SafeMoveOption> options = 
      getSafeMoveOptions(gameBoard, tank, enemyTank,shells, avoidEnemySight);
    
    if (options.empty()) {
        return Action::None;
    }
    
    std::sort(options.begin(), options.end());
    return options[0].action;
}

// TODO: to many options considered
// TODO: add mine to checks
std::vector<Algorithm::SafeMoveOption> Algorithm::getSafeMoveOptions(
    const GameBoard& gameBoard, 
    const Tank& tank,
    const Tank& enemyTank,
    const std::vector<Shell>& shells,
    bool avoidEnemySight
) const {
    std::vector<SafeMoveOption> safeOptions;
    Direction currentDir = tank.getDirection();
    
    // Check forward movement
    Point forwardPos = tank.getNextForwardPosition();
    forwardPos = gameBoard.wrapPosition(forwardPos);
    
    // TODO: extract to helper function
    bool canMove = gameBoard.canMoveTo(forwardPos) && forwardPos != enemyTank.getPosition();
    bool safeFromShells = !isInDanger(gameBoard, forwardPos, shells);
    bool safeFromEnemy = !avoidEnemySight || !isExposedToEnemy(gameBoard, forwardPos, enemyTank);
    
    if (canMove && safeFromShells && safeFromEnemy) {
        safeOptions.push_back({
            forwardPos, 
            Action::MoveForward, 
            1
        });
    }
    
    // Check all other directions
    for (const Direction& dir : ALL_DIRECTIONS) {
        if (dir == currentDir) continue;  // Already checked forward
        
        Point newPos = gameBoard.wrapPosition(tank.getPosition() + getDirectionDelta(dir));
        
        canMove = gameBoard.canMoveTo(newPos) && newPos != enemyTank.getPosition();
        safeFromShells = !isInDanger(gameBoard, newPos, shells);
        safeFromEnemy = !avoidEnemySight || !isExposedToEnemy(gameBoard, newPos, enemyTank);
        
        if (canMove && safeFromShells && safeFromEnemy) {            
            int cost = calculateMoveCost(tank, newPos, dir);
            safeOptions.push_back({
                newPos,
                getRotationToDirection(currentDir, dir),  // Return first rotation action
                cost
            });
        }
    }
    
    // Check backward movement
    Point backwardPos = tank.getNextBackwardPosition();
    backwardPos = gameBoard.wrapPosition(backwardPos);
    
    canMove = gameBoard.canMoveTo(backwardPos) && backwardPos != enemyTank.getPosition();
    safeFromShells = !isInDanger(gameBoard, backwardPos, shells);
    safeFromEnemy = !avoidEnemySight || !isExposedToEnemy(gameBoard, backwardPos, enemyTank);
    
    if (canMove && safeFromShells && safeFromEnemy) {
        int cost = tank.isContinuousBackward() ? 1 : 3;  // Initial backward takes 3 steps        
        safeOptions.push_back({
            backwardPos,
            Action::MoveBackward,
            cost
        });
    }
    
    return safeOptions;
}

bool Algorithm::isExposedToEnemy(
  const GameBoard& gameBoard, 
  const Point& position,
  const Tank& enemyTank) const {
    if (enemyTank.isDestroyed()) {
        return false;
    }
    
    auto dirOpt = getLineOfSightDirection(gameBoard, enemyTank.getPosition(), position);
    return dirOpt.has_value();
}

int Algorithm::calculateMoveCost(const Tank& tank, const Point& targetPos, Direction targetDir) const {
    Direction currentDir = tank.getDirection();
    
    // If already facing the right direction, just need 1 step to move
    if (currentDir == targetDir) {
        return 1;
    }
    
    // Calculate minimum rotation steps
    int leftSteps = 0;
    int rightSteps = 0;
    Direction tempDir = currentDir;
    
    // Count left rotation steps
    while (tempDir != targetDir && leftSteps < 8) {
        tempDir = rotateLeft(tempDir, false);
        leftSteps++;
    }
    
    // Count right rotation steps
    tempDir = currentDir;
    while (tempDir != targetDir && rightSteps < 8) {
        tempDir = rotateRight(tempDir, false);
        rightSteps++;
    }
    
    // Use the minimum number of rotation steps
    int rotationSteps = std::min(leftSteps, rightSteps);
    
    // Can optimize with quarter rotations (worth 2 eighth rotations)
    if (rotationSteps >= 2) {
        rotationSteps = (rotationSteps + 1) / 2;  // Round up for quarter turns
    }
    
    // Total cost: rotation steps + 1 step to move
    return rotationSteps + 1;
}

bool Algorithm::canHitTarget(const GameBoard& gameBoard, const Tank& shooter, 
                           const Point& targetPos) const {
    if (!shooter.canShoot()) {
        return false;
    }
    
    return checkLineOfSightInDirection(
        gameBoard, 
        shooter.getPosition(),
        targetPos,
        shooter.getDirection()
    );
}

std::optional<Direction> Algorithm::getLineOfSightDirection(const GameBoard& gameBoard, 
                                                         const Point& from, 
                                                         const Point& to) const {
    // Check all 8 possible directions
    for (const Direction& dir : ALL_DIRECTIONS) {
        if (checkLineOfSightInDirection(gameBoard, from, to, dir)) {
            return dir;
        }
    }
    return std::nullopt;  // No line of sight found
}

bool Algorithm::checkLineOfSightInDirection(const GameBoard& gameBoard, const Point& from, 
                                         const Point& to, Direction direction) const {
    if (from == to) {
        return true;
    }
    
    Point current = from;
    Point delta = getDirectionDelta(direction);
    
    // Count steps to avoid infinite loops in case of wrapping
    int maxSteps = gameBoard.getWidth() + gameBoard.getHeight();
    int steps = 0;
    
    while (steps < maxSteps) {
        current = gameBoard.wrapPosition(current + delta);
        steps++;
        
        if (current == to) {
            return true;
        }
        
        if (gameBoard.isWall(current)) {
            return false;
        }
    }
    
    return false;
}

Action Algorithm::getRotationToDirection(Direction current, Direction target) const {
  if (current == target) {
      return Action::None;
  }
  
  // One step rotations
  if (target == rotateRight(current, false)) {
      return Action::RotateRightEighth;
  }
  if (target == rotateLeft(current, false)) {
      return Action::RotateLeftEighth;
  }
  if (target == rotateRight(current, true)) {
      return Action::RotateRightQuarter;
  }
  if (target == rotateLeft(current, true)) {
      return Action::RotateLeftQuarter;
  }
  
  // Two step rotations
  int stepsClockwise = 0;
  int stepsCounterClockwise = 0;
  
  // Count steps in each direction
  Direction tempDir = current;
  while (tempDir != target && stepsClockwise < 4) {
      tempDir = rotateRight(tempDir, false);
      stepsClockwise++;
  }
  
  tempDir = current;
  while (tempDir != target && stepsCounterClockwise < 4) {
      tempDir = rotateLeft(tempDir, false);
      stepsCounterClockwise++;
  }
  
  // Choose the direction with fewer steps
  bool rotateClockwise = (stepsClockwise <= stepsCounterClockwise);
  
  // Return the appropriate first rotation
  if (stepsClockwise <= stepsCounterClockwise) {
    // Rotate clockwise, always use quarter turn for multi-step rotations
    return Action::RotateRightQuarter;
  } else {
      // Rotate counter-clockwise, always use quarter turn for multi-step rotations
      return Action::RotateLeftQuarter;
  }
}

Action Algorithm::evaluateOffensiveOptions(const GameBoard& gameBoard, const Tank& myTank, 
                                         const Tank& enemyTank) const {
    if (canHitTarget(gameBoard, myTank, enemyTank.getPosition())) {
        return Action::Shoot;
    }
    
    // Check if we have line of sight but need to rotate
    auto lineOfSightDir = getLineOfSightDirection(gameBoard, myTank.getPosition(), enemyTank.getPosition());
    if (lineOfSightDir.has_value()) {
        return getRotationToDirection(myTank.getDirection(), lineOfSightDir.value());
    }
    
    return Action::None;
}

// TODO: old methods - remove after refactor
// TODO: old methods - remove after refactor
// TODO: old methods - remove after refactor
// TODO: old methods - remove after refactor
// TODO: old methods - remove after refactor
// TODO: old methods - remove after refactor
std::optional<Direction> Algorithm::hasDirectLineOfSight(
  const GameBoard& gameBoard,
  const Point& from,
  const Point& to) const
{
  if (from == to) {
      return Direction::Up; // Any direction is fine when points are the same
  }

  // TODO: replace with enum iteration if possible
  for (int i = 0; i < NUM_DIRECTIONS; ++i) {
      Direction current_dir = ALL_DIRECTIONS[i];
      if (hasLineOfSightInDirection(gameBoard, from, to, current_dir)) {
          return current_dir;
      }
  }
  return std::nullopt;
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

  bool skipShell;

  // TODO: consider moving to next shell if shell belongs to the same tank
  // TODO: remove use of goto
  for (const Shell& shell : shells) {
      skipShell = false;
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
                  skipShell = true;
                  break;
              }

              if (shellPosition == position) {
                  return true;
              }
          }
          if (skipShell) {
              break;
          }
      }
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
  return Action::None;
}

Action Algorithm::getFirstRotationAction(Direction currentDir, Direction targetDir) const {
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

// TODO: consider doing this more elegantly
Action Algorithm::getTryToShootAction(GameBoard gameBoard, Tank myTank, Tank enemyTank) const{
  if (myTank.canShoot()) {
    if (canShootEnemy(gameBoard, myTank, enemyTank)) {
        return Action::Shoot;
    }
    std::optional<Direction> dir = hasDirectLineOfSight(gameBoard, myTank.getPosition(), enemyTank.getPosition());
    if (dir.has_value()) {
        Direction targetDir = dir.value();
        std::cout << "line of sight direction: " << targetDir << std::endl;
        if (myTank.getDirection() == targetDir) {
            return Action::Shoot;
        } else {
            Action rotationAction = getFirstRotationAction(myTank.getDirection(), targetDir);
            if (rotationAction != Action::None) {
                return rotationAction;
            }
        }
    }
  }
  return Action::None;
}

Action Algorithm::getTryToAvoidShellsAction(GameBoard gameBoard, Tank myTank, std::vector<Shell> shells) const {
  if (isPositionInDangerFromShells(gameBoard, myTank.getPosition(), shells)) {
    Action safeAction = findSafeAction(gameBoard, myTank, shells);
    if (safeAction != Action::None) {
        return safeAction;
    }
  }
  return Action::None;
}
