#include "collision_handler.h"
#include <map>

bool CollisionHandler::resolveAllCollisions(
  std::vector<Tank>& tanks,
  std::vector<Shell>& shells,
  GameBoard& board
) {
  m_positionExplosions.clear();
  m_pathExplosions.clear();
  m_boardWidth = board.getWidth();
  m_boardHeight = board.getHeight();

  detectPathCollisions(tanks, shells);
  bool tankDestroyed = applyPathExplosions(tanks, shells);

  checkShellWallCollisions(shells, board);
  
  detectPositionCollisions(tanks, shells);
  tankDestroyed |= applyPositionExplosions(tanks, shells, board);

  tankDestroyed |= checkTankMineCollisions(tanks, board);

  return tankDestroyed;
}


void CollisionHandler::detectPathCollisions(
  std::vector<Tank>& tanks,
  std::vector<Shell>& shells
) {
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

              markPathExplosionAt(objects[i].prev, objects[i].curr);
          }
      }
  }
}

void CollisionHandler::detectPositionCollisions(std::vector<Tank>& tanks,
                                                std::vector<Shell>& shells) {
    std::map<Point, int> positionCounts;

    for (const Tank& tank : tanks) {
        if (!tank.isDestroyed()) {
            positionCounts[tank.getPosition()]++;
        }
    }

    for (const Shell& shell : shells) {
        if (!shell.isDestroyed()) {
            positionCounts[shell.getPosition()]++;
        }
    }

    // Any position with 2+ things is a collision
    for (const auto& [pos, count] : positionCounts) {
        if (count >= 2) {
            markPositionExplosionAt(pos);
        }
    }
}

void CollisionHandler::checkShellWallCollisions(
    std::vector<Shell>& shells,
    GameBoard& board
  ) {
  for (Shell& shell : shells) {
      if (shell.isDestroyed()) continue;

      Point pos = shell.getPosition();
      if (board.isWall(pos)) {
          board.damageWall(pos);
          markPositionExplosionAt(pos);
      }
  }
}

bool CollisionHandler::checkTankMineCollisions(
    std::vector<Tank>& tanks, 
    GameBoard& board
  ) {
  bool tankDestroyed = false;
  for (Tank& tank : tanks) {
      if (tank.isDestroyed()) continue;

      Point pos = tank.getPosition();
      if (board.getCellType(pos) == GameBoard::CellType::Mine) {
          tank.destroy();
          tankDestroyed = true;
          board.setCellType(pos, GameBoard::CellType::Empty);
          markPositionExplosionAt(pos);
      }
  }
  return tankDestroyed;
}

bool CollisionHandler::applyPathExplosions(std::vector<Tank>& tanks,
                                           std::vector<Shell>& shells) {
    bool tankDestroyed = false;

    for (Tank& tank : tanks) {
        if (!tank.isDestroyed()) {
            for (const auto& mid : m_pathExplosions) {
                if (pathCrossedMidpoint(
                  tank.getPreviousPosition(), 
                  tank.getPosition(), 
                  mid.first, 
                  mid.second)
                ) {
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
                if (pathCrossedMidpoint(
                  shell.getPreviousPosition(), 
                  shell.getPosition(), 
                  mid.first, 
                  mid.second)
                ) {
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
  auto [mx, my] = computeMidpoint(from, to);
  m_pathExplosions.emplace_back(mx, my);
}

bool CollisionHandler::pathCrossedMidpoint(
  const Point& prev, 
  const Point& curr, 
  float mx, float my
) const {
    auto [computed_mx, computed_my] = computeMidpoint(prev, curr);
    return std::abs(computed_mx - mx) < 1e-5f 
           && std::abs(computed_my - my) < 1e-5f;
}

std::pair<float, float> CollisionHandler::computeMidpoint(
  const Point& a, 
  const Point& b
) const {
  int dx = b.getX() - a.getX();
  int dy = b.getY() - a.getY();

  float mx, my;

  // X axis midpoint
  if (dx == 1 || dx == -1) {
      mx = (a.getX() + b.getX()) / 2.0f;
  } else if ((a.getX() == 0 && b.getX() == m_boardWidth - 1) || 
             (b.getX() == 0 && a.getX() == m_boardWidth - 1)) {
      // Wraparound on X
      mx = (m_boardWidth - 0.5f);
  } else {
      mx = (a.getX() + b.getX()) / 2.0f;
  }

  // Y axis midpoint
  if (dy == 1 || dy == -1) {
      my = (a.getY() + b.getY()) / 2.0f;
  } else if ((a.getY() == 0 && b.getY() == m_boardHeight - 1) || 
             (b.getY() == 0 && a.getY() == m_boardHeight - 1)) {
      // Wraparound on Y
      my = (m_boardHeight - 0.5f);
  } else {
      my = (a.getY() + b.getY()) / 2.0f;
  }

  return {mx, my};
}

