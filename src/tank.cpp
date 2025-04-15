#include "tank.h"

Tank::Tank(int playerId, const Point& position, Direction direction)
    : m_playerId(playerId),
      m_position(position),
      m_direction(direction) {
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

void Tank::setPosition(const Point& position) {
    m_position = position;
}

void Tank::setDirection(Direction direction) {
    m_direction = direction;
}