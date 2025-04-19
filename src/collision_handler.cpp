#include "collision_handler.h"
#include <map>

bool CollisionHandler::resolveAllCollisions(
  std::vector<Tank>& tanks,
  std::vector<Shell>& shells,
  GameBoard& board
) {
  m_explosionPositions.clear();

  // Step 1: Detect crossing paths
  detectPathCrossings(tanks, shells);

  // Step 2: Detect same-position overlaps
  detectPositionCollisions(tanks, shells);

  // Step 3: Special collisions
  checkShellWallCollisions(shells, board);
  checkTankMineCollisions(tanks, board);

  // Step 4: Apply effects
  return applyExplosions(tanks, shells, board);
}


// TODO: implement stubs
void CollisionHandler::detectPathCrossings(
  std::vector<Tank>& tanks,
  std::vector<Shell>& shells
) {}

void CollisionHandler::detectPositionCollisions(
  std::vector<Tank>& tanks,
  std::vector<Shell>& shells
) {}

void CollisionHandler::checkShellWallCollisions(
  std::vector<Shell>& shells,
  GameBoard& board
) {}

void CollisionHandler::checkTankMineCollisions(
  std::vector<Tank>& tanks,
  GameBoard& board
) {}

bool CollisionHandler::applyExplosions(
  std::vector<Tank>& tanks,
  std::vector<Shell>& shells,
  GameBoard& board
) {
  return false;
}

void CollisionHandler::markExplosionAt(const Point& position) {
  m_explosionPositions.push_back(position);
}

