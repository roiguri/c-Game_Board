#include "objects/tank.h"

int Tank::INITIAL_SHELLS = 16;
void Tank::setInitialShells(int shells) {
    INITIAL_SHELLS = shells;
}

Tank::Tank(int playerId, const Point& position, Direction direction)
    : GameObject(playerId, position, direction),
      m_remainingShells(INITIAL_SHELLS),
      m_shootCooldown(0),
      m_backwardCounter(0),
      m_continuousBackward(false) {
}

int Tank::getRemainingShells() const {
  return m_remainingShells;
}

bool Tank::canShoot() const {
  return !m_destroyed && 
         m_remainingShells > 0 && 
         m_shootCooldown == 0;
}

bool Tank::isMovingBackward() const {
  return m_backwardCounter > 0;
}

int Tank::getBackwardCounter() const {
  return m_backwardCounter;
}

bool Tank::isContinuousBackward() const {
  return m_continuousBackward;
}

void Tank::decrementShells() {
  if (m_remainingShells > 0) {
      m_remainingShells--;
  }
}

void Tank::updateCooldowns() {
  if (m_shootCooldown > 0) {
      m_shootCooldown--;
  }
}

void Tank::resetBackwardMovement() {
  m_backwardCounter = 0;
  m_continuousBackward = false;
}

bool Tank::moveForward(const Point& newPosition) {
  if (m_backwardCounter > 0) {
    resetBackwardMovement();
    return true;
  }
  
  setPosition(newPosition);

  m_continuousBackward = false;
  return true;
}

bool Tank::requestMoveBackward(const Point& newPosition) {
  if (isInBackwardMovement()) {
      return false;
  }
  m_backwardPosition = newPosition;
  if (m_continuousBackward) {
      moveBackward();
      return true;
  }
  m_backwardCounter = 1;
  return true;
}

void Tank::moveBackward() {
  setPosition(m_backwardPosition);
  m_backwardCounter = 0;
  m_continuousBackward = true;
}

bool Tank::isInBackwardMovement() {
  if (m_backwardCounter > 0) {
    m_backwardCounter++;
    
    if (m_backwardCounter > BACKWARD_DELAY) {
      moveBackward();          
      return true;
    }
    return true;
  }
  return false;
}

bool Tank::rotateLeft(bool quarterTurn) {
  m_previousPosition = m_position;
  if (isInBackwardMovement()) {
    return false;
  }
  
  m_direction = ::rotateLeft(m_direction, quarterTurn);
  m_continuousBackward = false;
  return true;
}

bool Tank::rotateRight(bool quarterTurn) {
  m_previousPosition = m_position;
  if (isInBackwardMovement()) {
    return false;
  }
  
  m_direction = ::rotateRight(m_direction, quarterTurn);
  m_continuousBackward = false;
  return true;
}

bool Tank::shoot() {
  m_previousPosition = m_position;
  if (isInBackwardMovement()) {
    return false;
  }

  if (!canShoot()) {
      return false;
  }
  
  decrementShells();
  m_shootCooldown = SHOOT_COOLDOWN + 1;
  m_continuousBackward = false;
  return true;
}

Point Tank::getNextForwardPosition() const {
  return m_position + getDirectionDelta(m_direction);
}

Point Tank::getNextBackwardPosition() const {
  return m_position - getDirectionDelta(m_direction);
}

void Tank::doNothing() {
  m_previousPosition = m_position;
  isInBackwardMovement();
}