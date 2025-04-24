#include "objects/game_object.h"

GameObject::GameObject(int playerId, const Point& position, Direction direction)
    : m_playerId(playerId),
      m_position(position),
      m_previousPosition(position),
      m_direction(direction),
      m_destroyed(false) {
}

int GameObject::getPlayerId() const {
    return m_playerId;
}

Point GameObject::getPosition() const {
    return m_position;
}

Point GameObject::getPreviousPosition() const {
    return m_previousPosition;
}

Direction GameObject::getDirection() const {
    return m_direction;
}

bool GameObject::isDestroyed() const {
    return m_destroyed;
}

void GameObject::setPosition(const Point& position) {
    m_previousPosition = m_position;
    m_position = position;
}

void GameObject::setDirection(Direction direction) {
    m_direction = direction;
}

void GameObject::destroy() {
    m_destroyed = true;
}

void GameObject::updatePreviousPosition() {
    m_previousPosition = m_position;
}