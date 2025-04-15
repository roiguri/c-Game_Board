#include "tank.h"

Tank::Tank(int playerId, const Point& position, Direction direction)
    : m_playerId(playerId),
      m_position(position),
      m_direction(direction),
      m_remainingShells(INITIAL_SHELLS),
      m_destroyed(false),
      m_shootCooldown(0) {
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

void Tank::setPosition(const Point& position) {
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

bool Tank::moveForward(const Point& newPosition) {
  m_position = newPosition;
  return true;
}

bool Tank::rotateLeft(bool quarterTurn) {
  m_direction = ::rotateLeft(m_direction, quarterTurn);
  return true;
}

bool Tank::rotateRight(bool quarterTurn) {
  m_direction = ::rotateRight(m_direction, quarterTurn);
  return true;
}

bool Tank::shoot() {
  if (!canShoot()) {
      return false;
  }
  
  decrementShells();
  m_shootCooldown = SHOOT_COOLDOWN;
  return true;
}

Point Tank::getNextForwardPosition() const {
  return m_position + getDirectionDelta(m_direction);
}

Point Tank::getNextBackwardPosition() const {
  // Backward movement is in the opposite direction of the tank's cannon
  return m_position - getDirectionDelta(m_direction);
}