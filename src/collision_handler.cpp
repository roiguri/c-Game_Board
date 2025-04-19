#include "collision_handler.h"
#include <map>

bool CollisionHandler::resolveAllCollisions(
  std::vector<Tank>& tanks,
  std::vector<Shell>& shells,
  GameBoard& board
) {
  m_positionExplosions.clear();
  m_pathExplosions.clear();

  // Step 1: Detect crossing paths
  detectPathCrossings(tanks, shells);
  bool tankDestroyed = applyPathExplosions(tanks, shells);

  // Step 2: Detect same-position overlaps
  detectPositionCollisions(tanks, shells);
  tankDestroyed |= applyPositionExplosions(tanks, shells, board);

  // Step 3: Special collisions
  checkShellWallCollisions(shells, board);
  checkTankMineCollisions(tanks, board);

  // Step 4: Apply effects
  return tankDestroyed;
}


void CollisionHandler::detectPathCrossings(
  std::vector<Tank>& tanks,
  std::vector<Shell>& shells
) {
  // Combine all objects into one list of pointers to a base interface
  struct MovingObject {
      Point prev;
      Point curr;
  };

  std::vector<MovingObject> objects;

  for (Tank& tank : tanks) {
      if (!tank.isDestroyed()) {
          objects.push_back({
              tank.getPreviousPosition(),
              tank.getPosition(),
          });
      }
  }

  for (Shell& shell : shells) {
      if (!shell.isDestroyed()) {
          objects.push_back({
              shell.getPreviousPosition(),
              shell.getPosition(),
          });
      }
  }

  // Check for position swaps (X→Y and Y→X)
  const size_t n = objects.size();
  for (size_t i = 0; i < n; ++i) {
      for (size_t j = i + 1; j < n; ++j) {
          if (objects[i].prev == objects[j].curr &&
              objects[i].curr == objects[j].prev) {

              // Log explosion at the crossing point
              markPathExplosionAt(objects[i].prev, objects[i].curr);
          }
      }
  }
}

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

bool CollisionHandler::applyPathExplosions(std::vector<Tank>& tanks,
                                           std::vector<Shell>& shells) {
    bool tankDestroyed = false;

    for (Tank& tank : tanks) {
        if (!tank.isDestroyed()) {
            for (const auto& mid : m_pathExplosions) {
                if (pathCrossedMidpoint(tank.getPreviousPosition(), tank.getPosition(), mid.first, mid.second)) {
                    tank.destroy();
                    tankDestroyed = true;
                    break;
                }
            }
        }
    }

    for (Shell& shell : shells) {
        if (!shell.isDestroyed()) {
            for (const auto& mid : m_pathExplosions) {
                if (pathCrossedMidpoint(shell.getPreviousPosition(), shell.getPosition(), mid.first, mid.second)) {
                    shell.destroy();
                    break;
                }
            }
        }
    }

    return tankDestroyed;
}

bool CollisionHandler::applyPositionExplosions(std::vector<Tank>& tanks,
                                               std::vector<Shell>& shells,
                                               GameBoard& board) {
    bool tankDestroyed = false;

    for (Tank& tank : tanks) {
        if (!tank.isDestroyed()) {
            for (const Point& exp : m_positionExplosions) {
                if (tank.getPosition() == exp) {
                    tank.destroy();
                    tankDestroyed = true;
                    break;
                }
            }
        }
    }

    for (Shell& shell : shells) {
        if (!shell.isDestroyed()) {
            for (const Point& exp : m_positionExplosions) {
                if (shell.getPosition() == exp) {
                    shell.destroy();
                    break;
                }
            }
        }
    }

    for (const Point& pos : m_positionExplosions) {
      if (board.getCellType(pos) == GameBoard::CellType::Mine) {
        board.setCellType(pos, GameBoard::CellType::Empty);
      }
    }

    return tankDestroyed;
}

void CollisionHandler::markPositionExplosionAt(const Point& pos) {
  m_positionExplosions.push_back(pos);
}

void CollisionHandler::markPathExplosionAt(const Point& from, const Point& to) {
  float mx = (from.x + to.x) / 2.0f;
  float my = (from.y + to.y) / 2.0f;
  m_pathExplosions.emplace_back(mx, my);
}

// TODO: replace with a floating point struct to hold midpoint
bool CollisionHandler::pathCrossedMidpoint(const Point& prev, const Point& curr, float mx, float my) {
  float px = static_cast<float>(prev.x);
  float py = static_cast<float>(prev.y);
  float cx = static_cast<float>(curr.x);
  float cy = static_cast<float>(curr.y);

  float midx = (px + cx) / 2.0f;
  float midy = (py + cy) / 2.0f;

  return std::abs(midx - mx) < 1e-5f && std::abs(midy - my) < 1e-5f;
}

