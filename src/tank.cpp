#include "tank.h"

Tank::Tank(int playerId, const Point& position, Direction direction)
    : m_playerId(playerId),
      m_position(position),
      m_previousPosition(position),
      m_direction(direction),
      m_remainingShells(INITIAL_SHELLS),
      m_destroyed(false),
      m_shootCooldown(0),
      m_backwardCounter(0),
      m_continuousBackward(false) {
}

Tank::~Tank() {
    // Nothing to clean up
}

int Tank::getPlayerId() const {
    return m_playerId;
}

Point Tank::getPosition() const {
    return m_position;
}

Point Tank::getPreviousPosition() const {
  return m_previousPosition;
}

void Tank::updatePreviousPosition() {
  m_previousPosition = m_position;
}

Direction Tank::getDirection() const {
    return m_direction;
}

int Tank::getRemainingShells() const {
  return m_remainingShells;
}

bool Tank::isDestroyed() const {
  return m_destroyed;
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

void Tank::setPosition(const Point& position) {
    m_previousPosition = m_position;
    m_position = position;
}

void Tank::setDirection(Direction direction) {
    m_direction = direction;
}

void Tank::decrementShells() {
  if (m_remainingShells > 0) {
      m_remainingShells--;
  }
}

void Tank::destroy() {
  m_destroyed = true;
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
      return true;
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
  if (isInBackwardMovement()) {
    return false;
  }
  
  m_direction = ::rotateLeft(m_direction, quarterTurn);
  m_continuousBackward = false;
  return true;
}

bool Tank::rotateRight(bool quarterTurn) {
  if (isInBackwardMovement()) {
    return false;
  }
  
  m_direction = ::rotateRight(m_direction, quarterTurn);
  m_continuousBackward = false; // TODO: is this needed?
  return true;
}

bool Tank::shoot() {
  if (isInBackwardMovement()) {
    return false;
  }

  if (!canShoot()) {
      return false;
  }
  
  decrementShells();
  m_shootCooldown = SHOOT_COOLDOWN;
  m_continuousBackward = false;
  return true;
}

Point Tank::getNextForwardPosition() const {
  return m_position + getDirectionDelta(m_direction);
}

Point Tank::getNextBackwardPosition() const {
  return m_position - getDirectionDelta(m_direction);
}