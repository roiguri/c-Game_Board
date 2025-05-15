#include "algo/algorithm.h"
#include <iostream>
#include <algorithm>
#include "algo/chase_algorithm.h"
#include "algo/defensive_algorithm.h"

Algorithm::Algorithm() {
}

Algorithm::~Algorithm() {
}

bool Algorithm::isInDangerFromShells(
  const GameBoard& gameBoard, 
  const Tank& tank, 
  const std::vector<Shell>& shells, 
  int lookAheadSteps
) const {
    return isInDangerFromShells(gameBoard, tank.getPosition(), shells, lookAheadSteps);
}

bool Algorithm::isInDangerFromShells(
  const GameBoard& gameBoard, 
  const Point& position, 
  const std::vector<Shell>& shells, 
  int lookAheadSteps
) const {
    if (gameBoard.isMine(position)) {
        return true;
    }
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
        for (int step = 0; step < lookAheadSteps * 2; ++step) {
            shellPos = 
              gameBoard.wrapPosition(shellPos + getDirectionDelta(shellDir));
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
    bool currentPositionSafe = !isInDangerFromShells(gameBoard, tank, shells);
      
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

std::vector<Algorithm::SafeMoveOption> Algorithm::getSafeMoveOptions(
    const GameBoard& gameBoard, 
    const Tank& tank,
    const Tank& enemyTank,
    const std::vector<Shell>& shells,
    bool avoidEnemySight
) const {
    std::vector<SafeMoveOption> safeOptions;
    Direction currentDir = tank.getDirection();
    
    for (const Direction& dir : ALL_DIRECTIONS) {        
        Point newPos = 
          gameBoard.wrapPosition(tank.getPosition() + getDirectionDelta(dir));
        
        if (isPositionSafe(gameBoard, newPos, enemyTank, 
                           shells, avoidEnemySight)) {            
          Action tankAction = getRotationToDirection(currentDir, dir);
          tankAction = (tankAction == Action::None) ? 
                        Action::MoveForward : tankAction;
          int cost = calculateMoveCost(tank, dir);
            safeOptions.push_back({
                newPos,
                tankAction,
                cost
            });
        }
    }
    
    Point backwardPos = tank.getNextBackwardPosition();
    backwardPos = gameBoard.wrapPosition(backwardPos);
    
    if (isPositionSafe(gameBoard, backwardPos, enemyTank, 
                       shells, avoidEnemySight)) {
        int cost = tank.isContinuousBackward() ? 1 : 3;        
        safeOptions.push_back({
            backwardPos,
            Action::MoveBackward,
            cost
        });
    }
    
    return safeOptions;
}

bool Algorithm::isPositionSafe(
  const GameBoard& gameBoard, 
  const Point& position,
  const Tank& enemyTank,
  const std::vector<Shell>& shells,
  bool avoidEnemySight
) const {
  bool canMove = gameBoard.canMoveTo(position) &&
                 position != enemyTank.getPosition() && 
                 !gameBoard.isMine(position);
  bool safeFromShells = !isInDangerFromShells(gameBoard, position, shells);
  bool safeFromEnemy = 
    !avoidEnemySight || !isExposedToEnemy(gameBoard, position, enemyTank);

  return canMove && safeFromShells && safeFromEnemy;
}

bool Algorithm::isExposedToEnemy(
  const GameBoard& gameBoard, 
  const Point& position,
  const Tank& enemyTank) const {
    if (enemyTank.isDestroyed()) {
        return false;
    }
    
    auto dirOpt = 
      getLineOfSightDirection(gameBoard, enemyTank.getPosition(), position);
    return dirOpt.has_value();
}

int Algorithm::calculateMoveCost(const Tank& tank, Direction targetDir) const {
    Direction currentDir = tank.getDirection();
    
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
    
    int rotationSteps = std::min(leftSteps, rightSteps);
    
    // Check if can optimize rotations with quarter rotations
    if (rotationSteps >= 2) {
        rotationSteps = (rotationSteps + 1) / 2;
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

std::optional<Direction> Algorithm::getLineOfSightDirection(
  const GameBoard& gameBoard, 
  const Point& from, 
  const Point& to
) const {
    for (const Direction& dir : ALL_DIRECTIONS) {
        if (checkLineOfSightInDirection(gameBoard, from, to, dir)) {
            return dir;
        }
    }
    return std::nullopt;  // No line of sight found
}

bool Algorithm::checkLineOfSightInDirection(
  const GameBoard& gameBoard, 
  const Point& from, 
  const Point& to,
  Direction direction
) const {
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

Action Algorithm::getRotationToDirection(
  Direction current, 
  Direction target
) const {
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
  
  if (rotateClockwise) {
    return Action::RotateRightQuarter;
  } else {
      return Action::RotateLeftQuarter;
  }
}

Action Algorithm::evaluateOffensiveOptions(
  const GameBoard& gameBoard, 
  const Tank& myTank, 
  const Tank& enemyTank
) const {
    if (canHitTarget(gameBoard, myTank, enemyTank.getPosition())) {
        return Action::Shoot;
    }
    
    auto lineOfSightDir = getLineOfSightDirection(
      gameBoard, myTank.getPosition(), 
      enemyTank.getPosition());
    if (lineOfSightDir.has_value()) {
        return getRotationToDirection(
          myTank.getDirection(), lineOfSightDir.value());
    }
    
    return Action::None;
}