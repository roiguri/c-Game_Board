#include "shell.h"

namespace UserCommon_098765432_123456789 {

Shell::Shell(int playerId, const Point& position, Direction direction)
    : GameObject(playerId, position, direction) {
}

Point Shell::getNextPosition() const {
  return m_position + getDirectionDelta(m_direction);
}

} // namespace UserCommon_098765432_123456789