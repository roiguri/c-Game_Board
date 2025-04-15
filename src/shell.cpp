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