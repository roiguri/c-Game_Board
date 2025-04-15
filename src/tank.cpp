#include "tank.h"

Tank::Tank(int playerId, const Point& position, Direction direction)
    : m_playerId(playerId),
      m_position(position),
      m_direction(direction),
      m_remainingShells(INITIAL_SHELLS),
      m_destroyed(false) {
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