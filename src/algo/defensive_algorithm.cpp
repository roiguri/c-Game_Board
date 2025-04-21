#include "algo/defensive_algorithm.h"

DefensiveAlgorithm::DefensiveAlgorithm() {
    // Nothing to initialize
}

DefensiveAlgorithm::~DefensiveAlgorithm() {
    // Nothing to clean up
}

Action DefensiveAlgorithm::getNextAction(
    const GameBoard& gameBoard,
    const Tank& myTank,
    const Tank& enemyTank,
    const std::vector<Shell>& shells
) {
    Action action = Action::None;
    
    action = getTryToAvoidShellsAction(gameBoard, myTank, shells);
    if (action != Action::None) {
        return action;
    }
    
    action = getTryToShootAction(gameBoard, myTank, enemyTank);
    if (action != Action::None) {
        return action;
    }

    action = moveOutOfLineOfSight(gameBoard, myTank, enemyTank, shells);
    if (action != Action::None) {
        return action;
    }
    
    return action;
}

// TODO: simplify
Action DefensiveAlgorithm::moveOutOfLineOfSight(
  const GameBoard& gameBoard,
  const Tank& myTank,
  const Tank& enemyTank,
  const std::vector<Shell>& shells
) const {  
  // If not in line of sight, no need to move
  if (!hasDirectLineOfSight(gameBoard, enemyTank.getPosition(), myTank.getPosition())) {
      return Action::None;
  }
    
  // Try moving forward first
  Point forwardPos = myTank.getPosition() + getDirectionDelta(myTank.getDirection());
  forwardPos = gameBoard.wrapPosition(forwardPos);
  
  if (gameBoard.canMoveTo(forwardPos) && 
      !isPositionInDangerFromShells(gameBoard, forwardPos, shells) &&
      !hasDirectLineOfSight(gameBoard, enemyTank.getPosition(), forwardPos)) {
      return Action::MoveForward;
  }
  
  // Try 1/8 turn left
  Direction leftDir = rotateLeft(myTank.getDirection(), false);
  Point leftPos = myTank.getPosition() + getDirectionDelta(leftDir);
  leftPos = gameBoard.wrapPosition(leftPos);
  
  if (gameBoard.canMoveTo(leftPos) && 
      !isPositionInDangerFromShells(gameBoard, leftPos, shells) &&
      !hasDirectLineOfSight(gameBoard, enemyTank.getPosition(), leftPos)) {
      return Action::RotateLeftEighth;
  }
  
  // Try 1/8 turn right
  Direction rightDir = rotateRight(myTank.getDirection(), false);
  Point rightPos = myTank.getPosition() + getDirectionDelta(rightDir);
  rightPos = gameBoard.wrapPosition(rightPos);
  
  if (gameBoard.canMoveTo(rightPos) && 
      !isPositionInDangerFromShells(gameBoard, rightPos, shells) &&
      !hasDirectLineOfSight(gameBoard, enemyTank.getPosition(), rightPos)) {
      return Action::RotateRightEighth;
  }
  
  // Try 1/4 turn left
  Direction leftQuarterDir = rotateLeft(myTank.getDirection(), true);
  Point leftQuarterPos = myTank.getPosition() + getDirectionDelta(leftQuarterDir);
  leftQuarterPos = gameBoard.wrapPosition(leftQuarterPos);
  
  if (gameBoard.canMoveTo(leftQuarterPos) && 
      !isPositionInDangerFromShells(gameBoard, leftQuarterPos, shells) &&
      !hasDirectLineOfSight(gameBoard, enemyTank.getPosition(), leftQuarterPos)) {
      return Action::RotateLeftQuarter;
  }
  
  // Try 1/4 turn right
  Direction rightQuarterDir = rotateRight(myTank.getDirection(), true);
  Point rightQuarterPos = myTank.getPosition() + getDirectionDelta(rightQuarterDir);
  rightQuarterPos = gameBoard.wrapPosition(rightQuarterPos);
  
  if (gameBoard.canMoveTo(rightQuarterPos) && 
      !isPositionInDangerFromShells(gameBoard, rightQuarterPos, shells) &&
      !hasDirectLineOfSight(gameBoard, enemyTank.getPosition(), rightQuarterPos)) {
      return Action::RotateRightQuarter;
  }
  
  // If no good move found
  return Action::None;
}