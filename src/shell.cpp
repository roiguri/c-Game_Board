#include "shell.h"

Shell::Shell(int playerId, const Point& position, Direction direction)
    : m_playerId(playerId), 
      m_position(position), 
      m_direction(direction),
      m_destroyed(false) {
}

Shell::~Shell() {
    // No dynamic resources to clean up
}

int Shell::getPlayerId() const {
  return m_playerId;
}

Point Shell::getPosition() const {
  return m_position;
}

Direction Shell::getDirection() const {
  return m_direction;
}

bool Shell::isDestroyed() const {
  return m_destroyed;
}


void Shell::setPosition(const Point& position) {
  m_position = position;
}

void Shell::destroy() {
  m_destroyed = true;
}

Point Shell::getNextPosition() const {
  return m_position + getDirectionDelta(m_direction);
}