#include "algo/algorithm.h"
#include <iostream>
#include <algorithm>
#include "algo/chase_algorithm.h"
#include "algo/defensive_algorithm.h"

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