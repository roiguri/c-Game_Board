#include "shell.h"

Shell::Shell(int playerId, const Point& position, Direction direction)
    : GameObject(playerId, position, direction) {
}

Point Shell::getNextPosition() const {
  return m_position + getDirectionDelta(m_direction);
}