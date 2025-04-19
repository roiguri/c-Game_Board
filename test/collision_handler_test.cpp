#include "gtest/gtest.h"
#include "collision_handler.h"
#include "tank.h"
#include "shell.h"
#include "game_board.h"
#include "utils/direction.h"

class CollisionHandlerTest : public ::testing::Test {
protected:
    CollisionHandler handler;
    GameBoard dummyBoard{10, 10};

    const std::vector<std::pair<float, float>>& getPathExplosions() const {
        return handler.m_pathExplosions;
    }

    const std::vector<Point>& getPositionExplosions() const {
      return handler.m_positionExplosions;
    }

    void markPathExplosion(const Point& from, const Point& to) {
      handler.markPathExplosionAt(from, to);
    }

    void markPositionExplosion(const Point& pos) {
        handler.markPositionExplosionAt(pos);
    }

    bool applyPathExplosions(std::vector<Tank>& tanks, std::vector<Shell>& shells) {
        return handler.applyPathExplosions(tanks, shells);
    }

    bool applyPositionExplosions(std::vector<Tank>& tanks, std::vector<Shell>& shells, GameBoard& board) {
        return handler.applyPositionExplosions(tanks, shells, board);
    }

    void detectPositionCollisions(std::vector<Tank>& tanks, std::vector<Shell>& shells) {
      handler.detectPositionCollisions(tanks, shells);
    }

    void checkShellWallCollisions(std::vector<Shell>& shells) {
      handler.checkShellWallCollisions(shells, dummyBoard);
    }
    
    void checkTankMineCollisions(std::vector<Tank>& tanks) {
        handler.checkTankMineCollisions(tanks, dummyBoard);
    }

    bool resolveAll(std::vector<Tank>& tanks, std::vector<Shell>& shells) {
      return handler.resolveAllCollisions(tanks, shells, board);
    }
};

// Test: Two shells swap positions → explosion at midpoint
TEST_F(CollisionHandlerTest, DetectPathCrossings_ShellsSwap_ExplosionLogged) {
    Shell s1(0, Point(2, 2), Direction::DownRight);
    Shell s2(1, Point(1, 1), Direction::UpLeft);

    s1.setPosition(Point(1, 1));
    s2.setPosition(Point(2, 2));

    std::vector<Shell> shells{s1, s2};
    std::vector<Tank> tanks;

    handler.resolveAllCollisions(tanks, shells, dummyBoard);

    ASSERT_EQ(getPathExplosions().size(), 1);
    EXPECT_FLOAT_EQ(getPathExplosions()[0].first, 1.5f);
    EXPECT_FLOAT_EQ(getPathExplosions()[0].second, 1.5f);
}

// Test: A tank and shell do not intersect → no explosion
TEST_F(CollisionHandlerTest, DetectPathCrossings_NoIntersection_NoExplosion) {
    Tank t1(0, Point(4, 3), Direction::Right);
    Shell s1(1, Point(0, 0), Direction::Left);

    t1.setPosition(Point(4, 4));

    std::vector<Tank> tanks{t1};
    std::vector<Shell> shells{s1};

    handler.resolveAllCollisions(tanks, shells, dummyBoard);

    EXPECT_TRUE(getPathExplosions().empty());

    // Ensure nothing is destroyed
    EXPECT_FALSE(tanks[0].isDestroyed());
    EXPECT_FALSE(shells[0].isDestroyed());
}

// Test: Tank-tank crossing → explosion logged at midpoint
TEST_F(CollisionHandlerTest, DetectPathCrossings_TanksSwap_ExplosionLogged) {
    Tank t1(0, Point(4, 4), Direction::Down);
    Tank t2(1, Point(3, 3), Direction::Up);

    t1.setPosition(Point(3, 3));
    t2.setPosition(Point(4, 4));

    std::vector<Tank> tanks{t1, t2};
    std::vector<Shell> shells;

    handler.resolveAllCollisions(tanks, shells, dummyBoard);

    ASSERT_EQ(getPathExplosions().size(), 1);
    EXPECT_FLOAT_EQ(getPathExplosions()[0].first, 3.5f);
    EXPECT_FLOAT_EQ(getPathExplosions()[0].second, 3.5f);
}

TEST_F(CollisionHandlerTest, ApplyPathExplosion_DestroysCrossingShell) {
  Shell shell(0, Point(2, 2), Direction::UpLeft);
  shell.setPosition(Point(1, 1)); // sets prev internally

  std::vector<Shell> shells{shell};
  std::vector<Tank> tanks;

  markPathExplosion(Point(1, 1), Point(2, 2));

  EXPECT_FALSE(shells[0].isDestroyed());

  bool result = applyPathExplosions(tanks, shells);

  EXPECT_TRUE(shells[0].isDestroyed());
  EXPECT_FALSE(result);  // No tanks destroyed
}

TEST_F(CollisionHandlerTest, ApplyPathExplosion_DestroysCrossingTank) {
  Tank tank(0, Point(4, 4), Direction::UpLeft);
  tank.setPosition(Point(3, 3));  // now previous is (4,4), current is (3,3)

  std::vector<Tank> tanks{tank};
  std::vector<Shell> shells;

  markPathExplosion(Point(3, 3), Point(4, 4));

  EXPECT_FALSE(tanks[0].isDestroyed());

  bool result = applyPathExplosions(tanks, shells);

  EXPECT_TRUE(tanks[0].isDestroyed());
  EXPECT_TRUE(result);  // A tank was destroyed
}

TEST_F(CollisionHandlerTest, ApplyPositionExplosion_DestroysTankAndRemovesMine) {
  Point minePos(2, 2);

  dummyBoard.setCellType(minePos, GameBoard::CellType::Mine);
  EXPECT_EQ(dummyBoard.getCellType(minePos), GameBoard::CellType::Mine);

  Tank tank(0, minePos, Direction::Up);
  std::vector<Tank> tanks{tank};
  std::vector<Shell> shells;

  markPositionExplosion(minePos);

  bool result = applyPositionExplosions(tanks, shells, dummyBoard);

  EXPECT_TRUE(tanks[0].isDestroyed());
  EXPECT_EQ(dummyBoard.getCellType(minePos), GameBoard::CellType::Empty);
  EXPECT_TRUE(result);
}

TEST_F(CollisionHandlerTest, ApplyPositionExplosion_DestroysShell) {
  Point p(1, 1);

  Shell shell(0, p, Direction::Down);
  std::vector<Shell> shells{shell};
  std::vector<Tank> tanks;

  markPositionExplosion(p);

  bool result = applyPositionExplosions(tanks, shells, dummyBoard);

  EXPECT_TRUE(shells[0].isDestroyed());
  EXPECT_FALSE(result);  // No tank destroyed
}

TEST_F(CollisionHandlerTest, DetectPositionCollision_TankTankCollision_ExplosionLogged) {
  Tank t1(0, Point(1, 1), Direction::Down);
  Tank t2(1, Point(0, 0), Direction::Left);

  t1.setPosition(Point(2, 2));
  t2.setPosition(Point(2, 2));

  std::vector<Tank> tanks{t1, t2};
  std::vector<Shell> shells;

  detectPositionCollisions(tanks, shells);

  ASSERT_EQ(getPositionExplosions().size(), 1);
  EXPECT_EQ(getPositionExplosions()[0], Point(2, 2));
}

TEST_F(CollisionHandlerTest, DetectPositionCollision_TankShellCollision_ExplosionLogged) {
  Tank t1(0, Point(4, 4), Direction::Right);
  Shell s1(1, Point(0, 0), Direction::Left);

  t1.setPosition(Point(7, 7));
  s1.setPosition(Point(7, 7));

  std::vector<Tank> tanks{t1};
  std::vector<Shell> shells{s1};

  detectPositionCollisions(tanks, shells);

  ASSERT_EQ(getPositionExplosions().size(), 1);
  EXPECT_EQ(getPositionExplosions()[0], Point(7, 7));
}

TEST_F(CollisionHandlerTest, DetectPositionCollision_NoCollision_NoExplosionLogged) {
  Tank t1(0, Point(1, 1), Direction::Down);
  Shell s1(1, Point(2, 2), Direction::Left);
  Shell s2(1, Point(3, 3), Direction::Up);

  t1.setPosition(Point(4, 4));
  s1.setPosition(Point(5, 5));
  s2.setPosition(Point(6, 6));

  std::vector<Tank> tanks{t1};
  std::vector<Shell> shells{s1, s2};

  detectPositionCollisions(tanks, shells);

  EXPECT_TRUE(getPositionExplosions().empty());
}

TEST_F(CollisionHandlerTest, ShellWallCollision_OneHit_ExplosionLoggedWallSurvives) {
  Point wallPos(3, 3);
  dummyBoard.setCellType(wallPos, GameBoard::CellType::Wall);

  Shell shell(0, Point(0, 0), Direction::Right);
  shell.setPosition(wallPos);

  std::vector<Shell> shells{shell};

  checkShellWallCollisions(shells);

  EXPECT_EQ(dummyBoard.getCellType(wallPos), GameBoard::CellType::Wall);
  ASSERT_EQ(getPositionExplosions().size(), 1);
  EXPECT_EQ(getPositionExplosions()[0], wallPos);
}

TEST_F(CollisionHandlerTest, ShellWallCollision_TwoHits_WallDestroyed) {
  Point wallPos(4, 4);
  dummyBoard.setCellType(wallPos, GameBoard::CellType::Wall);

  Shell s1(0, Point(1, 1), Direction::Down);
  Shell s2(1, Point(2, 2), Direction::Down);

  s1.setPosition(wallPos);
  s2.setPosition(wallPos);

  std::vector<Shell> shells{s1, s2};

  checkShellWallCollisions(shells);

  EXPECT_EQ(dummyBoard.getCellType(wallPos), GameBoard::CellType::Empty);

  ASSERT_EQ(getPositionExplosions().size(), 2);
  EXPECT_EQ(getPositionExplosions()[0], wallPos);
  EXPECT_EQ(getPositionExplosions()[1], wallPos);
}

TEST_F(CollisionHandlerTest, TankMineCollision_DestroyedAndMineRemoved) {
  Point minePos(5, 5);
  dummyBoard.setCellType(minePos, GameBoard::CellType::Mine);

  Tank tank(0, Point(0, 0), Direction::Down);
  tank.setPosition(minePos);

  std::vector<Tank> tanks{tank};

  checkTankMineCollisions(tanks);

  EXPECT_TRUE(tanks[0].isDestroyed());
  EXPECT_EQ(dummyBoard.getCellType(minePos), GameBoard::CellType::Empty);
  ASSERT_EQ(getPositionExplosions().size(), 1);
  EXPECT_EQ(getPositionExplosions()[0], minePos);
}

TEST_F(CollisionHandlerTest, TankMineCollision_NoMine_NoExplosion) {
  Point safePos(6, 6);
  dummyBoard.setCellType(safePos, GameBoard::CellType::Empty);

  Tank tank(0, Point(0, 0), Direction::Right);
  tank.setPosition(safePos);

  std::vector<Tank> tanks{tank};

  checkTankMineCollisions(tanks);

  EXPECT_FALSE(tanks[0].isDestroyed());
  EXPECT_EQ(dummyBoard.getCellType(safePos), GameBoard::CellType::Empty);
  EXPECT_TRUE(getPositionExplosions().empty());
}