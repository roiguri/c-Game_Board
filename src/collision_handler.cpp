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
          MidPoint tankPath = MidPoint::calculateMidpoint(
              tank.getPreviousPosition(), 
              tank.getPosition(),
              m_boardWidth,
              m_boardHeight
          );
          
          if (tankPath.getX() != -1 && 
                m_pathExplosions.find(tankPath) != m_pathExplosions.end()) {
              tank.destroy();
              tankDestroyed = true;
          }
        }
    }

    for (Shell& shell : shells) {
        if (!shell.isDestroyed()) {
          MidPoint shellPath = MidPoint::calculateMidpoint(
              shell.getPreviousPosition(), 
              shell.getPosition(),
              m_boardWidth,
              m_boardHeight
          );
          
          if (shellPath.getX() != -1 && 
              m_pathExplosions.find(shellPath) != m_pathExplosions.end()) {
              shell.destroy();
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
          if (m_positionExplosions.find(tank.getPosition()) != 
                                                m_positionExplosions.end()) {
            tank.destroy();
            tankDestroyed = true;
          }
        }
    }

    for (Shell& shell : shells) {
        if (!shell.isDestroyed()) {
          if (m_positionExplosions.find(shell.getPosition()) != 
                                                m_positionExplosions.end()) {
            shell.destroy();
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
  m_positionExplosions.insert(pos);
}

void CollisionHandler::markPathExplosionAt(const Point& from, const Point& to) {
    MidPoint mp = 
          MidPoint::calculateMidpoint(from, to, m_boardWidth, m_boardHeight);
    if (mp.getX() != -1) {
      m_pathExplosions.insert(mp);
    }
}

