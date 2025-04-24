#include "gtest/gtest.h"
#include "collision_handler.h"
#include "objects/tank.h"
#include "objects/shell.h"
#include "game_board.h"
#include "utils/direction.h"

class CollisionHandlerTest : public ::testing::Test {
protected:
    CollisionHandler handler;
    GameBoard board{10, 10};

    const std::vector<MidPoint>& getPathExplosions() const {
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
      handler.checkShellWallCollisions(shells, board);
    }
    
    void checkTankMineCollisions(std::vector<Tank>& tanks) {
        handler.checkTankMineCollisions(tanks, board);
    }

    bool resolveAll(std::vector<Tank>& tanks, std::vector<Shell>& shells) {
      return handler.resolveAllCollisions(tanks, shells, board);
    }
};

// Test: Two shells swap positions → explosion at midpoint
TEST_F(CollisionHandlerTest, DetectPathCollisions_ShellsSwap_ExplosionLogged) {
    Shell s1(0, Point(2, 2), Direction::DownRight);
    Shell s2(1, Point(1, 1), Direction::UpLeft);

    s1.setPosition(Point(1, 1));
    s2.setPosition(Point(2, 2));

    std::vector<Shell> shells{s1, s2};
    std::vector<Tank> tanks;

    handler.resolveAllCollisions(tanks, shells, board);

    ASSERT_EQ(getPathExplosions().size(), 1);
    MidPoint mp = getPathExplosions()[0];
    EXPECT_EQ(mp.getX(), 1);
    EXPECT_EQ(mp.getY(), 1);
    EXPECT_TRUE(mp.isHalfX());
    EXPECT_TRUE(mp.isHalfY());
}

// Test: A tank and shell do not intersect → no explosion
TEST_F(CollisionHandlerTest, DetectPathCollisions_NoIntersection_NoExplosion) {
    Tank t1(0, Point(4, 3), Direction::Right);
    Shell s1(1, Point(0, 0), Direction::Left);

    t1.setPosition(Point(4, 4));

    std::vector<Tank> tanks{t1};
    std::vector<Shell> shells{s1};

    handler.resolveAllCollisions(tanks, shells, board);

    EXPECT_TRUE(getPathExplosions().empty());

    // Ensure nothing is destroyed
    EXPECT_FALSE(tanks[0].isDestroyed());
    EXPECT_FALSE(shells[0].isDestroyed());
}

// Test: Tank-tank crossing → explosion logged at midpoint
TEST_F(CollisionHandlerTest, DetectPathCollisions_TanksSwap_ExplosionLogged) {
    Tank t1(0, Point(4, 4), Direction::Down);
    Tank t2(1, Point(3, 3), Direction::Up);

    t1.setPosition(Point(3, 3));
    t2.setPosition(Point(4, 4));

    std::vector<Tank> tanks{t1, t2};
    std::vector<Shell> shells;

    handler.resolveAllCollisions(tanks, shells, board);

    ASSERT_EQ(getPathExplosions().size(), 1);
    MidPoint mp = getPathExplosions()[0];
    EXPECT_EQ(mp.getX(), 3);
    EXPECT_EQ(mp.getY(), 3);
    EXPECT_TRUE(mp.isHalfX());
    EXPECT_TRUE(mp.isHalfY());
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

  board.setCellType(minePos, GameBoard::CellType::Mine);
  EXPECT_EQ(board.getCellType(minePos), GameBoard::CellType::Mine);

  Tank tank(0, minePos, Direction::Up);
  std::vector<Tank> tanks{tank};
  std::vector<Shell> shells;

  markPositionExplosion(minePos);

  bool result = applyPositionExplosions(tanks, shells, board);

  EXPECT_TRUE(tanks[0].isDestroyed());
  EXPECT_EQ(board.getCellType(minePos), GameBoard::CellType::Empty);
  EXPECT_TRUE(result);
}

TEST_F(CollisionHandlerTest, ApplyPositionExplosion_DestroysShell) {
  Point p(1, 1);

  Shell shell(0, p, Direction::Down);
  std::vector<Shell> shells{shell};
  std::vector<Tank> tanks;

  markPositionExplosion(p);

  bool result = applyPositionExplosions(tanks, shells, board);

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
  board.setCellType(wallPos, GameBoard::CellType::Wall);

  Shell shell(0, Point(0, 0), Direction::Right);
  shell.setPosition(wallPos);

  std::vector<Shell> shells{shell};

  checkShellWallCollisions(shells);

  EXPECT_EQ(board.getCellType(wallPos), GameBoard::CellType::Wall);
  ASSERT_EQ(getPositionExplosions().size(), 1);
  EXPECT_EQ(getPositionExplosions()[0], wallPos);
}

TEST_F(CollisionHandlerTest, ShellWallCollision_TwoHits_WallDestroyed) {
  Point wallPos(4, 4);
  board.setCellType(wallPos, GameBoard::CellType::Wall);

  Shell s1(0, Point(1, 1), Direction::Down);
  Shell s2(1, Point(2, 2), Direction::Down);

  s1.setPosition(wallPos);
  s2.setPosition(wallPos);

  std::vector<Shell> shells{s1, s2};

  checkShellWallCollisions(shells);

  EXPECT_EQ(board.getCellType(wallPos), GameBoard::CellType::Empty);

  ASSERT_EQ(getPositionExplosions().size(), 2);
  EXPECT_EQ(getPositionExplosions()[0], wallPos);
  EXPECT_EQ(getPositionExplosions()[1], wallPos);
}

TEST_F(CollisionHandlerTest, TankMineCollision_DestroyedAndMineRemoved) {
  Point minePos(5, 5);
  board.setCellType(minePos, GameBoard::CellType::Mine);

  Tank tank(0, Point(0, 0), Direction::Down);
  tank.setPosition(minePos);

  std::vector<Tank> tanks{tank};

  checkTankMineCollisions(tanks);

  EXPECT_TRUE(tanks[0].isDestroyed());
  EXPECT_EQ(board.getCellType(minePos), GameBoard::CellType::Empty);
  ASSERT_EQ(getPositionExplosions().size(), 1);
  EXPECT_EQ(getPositionExplosions()[0], minePos);
}

TEST_F(CollisionHandlerTest, TankMineCollision_NoMine_NoExplosion) {
  Point safePos(6, 6);
  board.setCellType(safePos, GameBoard::CellType::Empty);

  Tank tank(0, Point(0, 0), Direction::Right);
  tank.setPosition(safePos);

  std::vector<Tank> tanks{tank};

  checkTankMineCollisions(tanks);

  EXPECT_FALSE(tanks[0].isDestroyed());
  EXPECT_EQ(board.getCellType(safePos), GameBoard::CellType::Empty);
  EXPECT_TRUE(getPositionExplosions().empty());
}

TEST_F(CollisionHandlerTest, Resolve_ShellShellPositionCollision_BothDestroyed) {
  Shell s1(0, Point(1, 1), Direction::Down);
  Shell s2(1, Point(2, 2), Direction::Up);

  s1.setPosition(Point(3, 3));
  s2.setPosition(Point(3, 3));

  std::vector<Shell> shells{s1, s2};
  std::vector<Tank> tanks;

  bool result = resolveAll(tanks, shells);

  EXPECT_TRUE(shells[0].isDestroyed());
  EXPECT_TRUE(shells[1].isDestroyed());
  EXPECT_FALSE(result);
  ASSERT_EQ(getPositionExplosions().size(), 1);
  EXPECT_EQ(getPositionExplosions()[0], Point(3, 3));
}

TEST_F(CollisionHandlerTest, Resolve_TankTankPositionCollision_BothDestroyed) {
  Tank t1(0, Point(0, 0), Direction::Right);
  Tank t2(1, Point(4, 4), Direction::Left);

  t1.setPosition(Point(2, 2));
  t2.setPosition(Point(2, 2));

  std::vector<Tank> tanks{t1, t2};
  std::vector<Shell> shells;

  bool result = resolveAll(tanks, shells);

  EXPECT_TRUE(tanks[0].isDestroyed());
  EXPECT_TRUE(tanks[1].isDestroyed());
  EXPECT_TRUE(result);
  ASSERT_EQ(getPositionExplosions().size(), 1);
  EXPECT_EQ(getPositionExplosions()[0], Point(2, 2));
}

TEST_F(CollisionHandlerTest, Resolve_ShellWallOneHit_WallSurvives) {
  Point wallPos(3, 3);
  board.setCellType(wallPos, GameBoard::CellType::Wall);

  Shell s1(0, Point(0, 0), Direction::DownRight);
  s1.setPosition(wallPos);

  std::vector<Shell> shells{s1};
  std::vector<Tank> tanks;

  bool result = resolveAll(tanks, shells);

  EXPECT_TRUE(shells[0].isDestroyed());
  EXPECT_EQ(board.getCellType(wallPos), GameBoard::CellType::Wall);
  ASSERT_EQ(getPositionExplosions().size(), 1);
  EXPECT_EQ(getPositionExplosions()[0], wallPos);
  EXPECT_FALSE(result);
}

TEST_F(CollisionHandlerTest, Resolve_ShellWallThreeHits_WallDestroyed) {
  Point wallPos(3, 3);
  board.setCellType(wallPos, GameBoard::CellType::Wall);

  Shell s1(0, Point(0, 0), Direction::Down);
  Shell s2(1, Point(1, 1), Direction::Down);
  Shell s3(2, Point(2, 2), Direction::Down);

  s1.setPosition(wallPos);
  s2.setPosition(wallPos);
  s3.setPosition(wallPos);

  std::vector<Shell> shells{s1, s2, s3};
  std::vector<Tank> tanks;

  bool result = resolveAll(tanks, shells);

  for (const Shell& s : shells) {
      EXPECT_TRUE(s.isDestroyed());
  }

  EXPECT_EQ(board.getCellType(wallPos), GameBoard::CellType::Empty);
  EXPECT_FALSE(result);
  ASSERT_EQ(getPositionExplosions().size(), 3);
  for (const Point& p : getPositionExplosions()) {
      EXPECT_EQ(p, wallPos);
  }
}

TEST_F(CollisionHandlerTest, Resolve_ShellsCrossMidpoint_AllDestroyed) {
  Shell s1(0, Point(0, 0), Direction::DownRight);
  Shell s2(1, Point(1, 1), Direction::UpLeft);
  Shell s3(2, Point(1, 0), Direction::UpRight);

  s1.setPosition(Point(1, 1));
  s2.setPosition(Point(0, 0));
  s3.setPosition(Point(0, 1));

  std::vector<Shell> shells{s1, s2, s3};
  std::vector<Tank> tanks;

  bool result = resolveAll(tanks, shells);

  for (const Shell& s : shells) {
      EXPECT_TRUE(s.isDestroyed());
  }

  // Midpoints: all are (0.5, 0.5)
  for (MidPoint mp : getPathExplosions()) {
      EXPECT_EQ(mp.getX(), 0);
      EXPECT_EQ(mp.getY(), 0);
      EXPECT_TRUE(mp.isHalfX());
      EXPECT_TRUE(mp.isHalfY());
  }

  EXPECT_FALSE(result);
}

TEST_F(CollisionHandlerTest, Resolve_TankStepsOnMine_DestroyedAndCleared) {
  Point minePos(2, 2);
  board.setCellType(minePos, GameBoard::CellType::Mine);

  Tank tank(0, Point(0, 0), Direction::DownRight);
  tank.setPosition(minePos);

  std::vector<Tank> tanks{tank};
  std::vector<Shell> shells;

  bool result = resolveAll(tanks, shells);

  EXPECT_TRUE(tanks[0].isDestroyed());
  EXPECT_EQ(board.getCellType(minePos), GameBoard::CellType::Empty);
  EXPECT_TRUE(result);
  ASSERT_EQ(getPositionExplosions().size(), 1);
  EXPECT_EQ(getPositionExplosions()[0], minePos);
}

TEST_F(CollisionHandlerTest, Resolve_ShellHitsTank_BothDestroyed) {
  Tank tank(0, Point(0, 0), Direction::Up);
  tank.setPosition(Point(2, 2));

  Shell shell(1, Point(4, 4), Direction::UpLeft);
  shell.setPosition(Point(2, 2));

  std::vector<Tank> tanks{tank};
  std::vector<Shell> shells{shell};

  bool result = resolveAll(tanks, shells);

  EXPECT_TRUE(tanks[0].isDestroyed());
  EXPECT_TRUE(shells[0].isDestroyed());
  EXPECT_TRUE(result);
  ASSERT_EQ(getPositionExplosions().size(), 1);
  EXPECT_EQ(getPositionExplosions()[0], Point(2, 2));
}

TEST_F(CollisionHandlerTest, Resolve_TankShellPathCross_BothDestroyed) {
  Tank tank(0, Point(1, 1), Direction::DownRight);
  Shell shell(1, Point(2, 2), Direction::UpLeft);

  tank.setPosition(Point(2, 2));     // prev: (0,0), curr: (2,2)
  shell.setPosition(Point(1, 1));    // prev: (2,2), curr: (0,0)

  std::vector<Tank> tanks{tank};
  std::vector<Shell> shells{shell};

  bool result = resolveAll(tanks, shells);

  EXPECT_TRUE(tanks[0].isDestroyed());
  EXPECT_TRUE(shells[0].isDestroyed());
  EXPECT_TRUE(result);

  ASSERT_EQ(getPathExplosions().size(), 1);
  MidPoint mp = getPathExplosions()[0];
  EXPECT_EQ(mp.getX(), 1);
  EXPECT_EQ(mp.getY(), 1);
  EXPECT_TRUE(mp.isHalfX());
  EXPECT_TRUE(mp.isHalfY());
}

TEST_F(CollisionHandlerTest, Resolve_NoCollisions_AllSurvive) {
  Tank tank(0, Point(0, 0), Direction::Right);
  Shell shell(1, Point(3, 3), Direction::Left);

  tank.setPosition(Point(1, 0));
  shell.setPosition(Point(2, 3));

  std::vector<Tank> tanks{tank};
  std::vector<Shell> shells{shell};

  bool result = resolveAll(tanks, shells);

  EXPECT_FALSE(tank.isDestroyed());
  EXPECT_FALSE(shell.isDestroyed());
  EXPECT_FALSE(result);

  EXPECT_TRUE(getPathExplosions().empty());
  EXPECT_TRUE(getPositionExplosions().empty());
}

TEST_F(CollisionHandlerTest, Resolve_MixedCollisions_AllHandledCorrectly) {
  // Setup:
  // - s1 + s2: collide at (2,2)
  // - s3 hits wall at (1,1)
  // - t1 hits mine at (3,3)

  Point wallPos(1, 1);
  Point minePos(3, 3);
  board.setCellType(wallPos, GameBoard::CellType::Wall);
  board.setCellType(minePos, GameBoard::CellType::Mine);

  Shell s1(0, Point(0, 0), Direction::DownRight);
  Shell s2(1, Point(4, 4), Direction::UpLeft);
  Shell s3(2, Point(0, 1), Direction::Right);
  s1.setPosition(Point(2, 2));
  s2.setPosition(Point(2, 2));
  s3.setPosition(wallPos);

  Tank t1(0, Point(0, 0), Direction::DownRight);
  t1.setPosition(minePos);

  std::vector<Shell> shells{s1, s2, s3};
  std::vector<Tank> tanks{t1};

  bool result = resolveAll(tanks, shells);

  EXPECT_TRUE(shells[0].isDestroyed());
  EXPECT_TRUE(shells[1].isDestroyed());
  EXPECT_TRUE(shells[2].isDestroyed());

  EXPECT_TRUE(tanks[0].isDestroyed());
  EXPECT_EQ(board.getCellType(wallPos), GameBoard::CellType::Wall);  // only 1 hit
  EXPECT_EQ(board.getCellType(minePos), GameBoard::CellType::Empty);

  EXPECT_TRUE(result);  // tank destroyed
  ASSERT_EQ(getPositionExplosions().size(), 3);
  EXPECT_EQ(getPositionExplosions()[0], wallPos);
  EXPECT_EQ(getPositionExplosions()[1], Point(2, 2));
  EXPECT_EQ(getPositionExplosions()[2], minePos);
}

TEST_F(CollisionHandlerTest, Resolve_ShellHitsMine_ShellSurvivesMineUnaffected) {
  Point minePos(2, 2);
  board.setCellType(minePos, GameBoard::CellType::Mine);

  Shell shell(0, Point(1, 2), Direction::Right);
  shell.setPosition(minePos);

  std::vector<Shell> shells{shell};
  std::vector<Tank> tanks;

  bool result = resolveAll(tanks, shells);

  EXPECT_FALSE(shells[0].isDestroyed());
  EXPECT_EQ(board.getCellType(minePos), GameBoard::CellType::Mine);
  EXPECT_FALSE(result);
  EXPECT_TRUE(getPositionExplosions().empty());
  EXPECT_TRUE(getPathExplosions().empty());
}

TEST_F(CollisionHandlerTest, Resolve_WraparoundShellPathCrossing_BothDestroyed) {
  Shell s1(0, Point(9, 0), Direction::Right);
  Shell s2(1, Point(0, 0), Direction::Left);

  s1.setPosition(Point(0, 0));  // wraparound
  s2.setPosition(Point(9, 0));  // wraparound

  std::vector<Shell> shells{s1, s2};
  std::vector<Tank> tanks;

  bool result = resolveAll(tanks, shells);

  EXPECT_TRUE(shells[0].isDestroyed());
  EXPECT_TRUE(shells[1].isDestroyed());
  EXPECT_FALSE(result);

  ASSERT_EQ(getPathExplosions().size(), 1);
  MidPoint mp = getPathExplosions()[0];
  EXPECT_EQ(mp.getX(), 9);
  EXPECT_EQ(mp.getY(), 0);
  EXPECT_TRUE(mp.isHalfX());
  EXPECT_FALSE(mp.isHalfY());
}

TEST_F(CollisionHandlerTest, Resolve_WraparoundX_CollisionMidpointCorrect) {
  Shell s1(0, Point(9, 5), Direction::Right);
  Shell s2(1, Point(0, 5), Direction::Left);

  s1.setPosition(Point(0, 5));  // wraps
  s2.setPosition(Point(9, 5));  // wraps

  std::vector<Shell> shells{s1, s2};
  std::vector<Tank> tanks;

  bool result = resolveAll(tanks, shells);

  EXPECT_TRUE(shells[0].isDestroyed());
  EXPECT_TRUE(shells[1].isDestroyed());

  ASSERT_EQ(getPathExplosions().size(), 1);
  MidPoint mp = getPathExplosions()[0];
  EXPECT_EQ(mp.getX(), 9);
  EXPECT_EQ(mp.getY(), 5);
  EXPECT_TRUE(mp.isHalfX());
  EXPECT_FALSE(mp.isHalfY());
  EXPECT_FALSE(result);
}

TEST_F(CollisionHandlerTest, Resolve_WraparoundY_CollisionMidpointCorrect) {
  Shell s1(0, Point(5, 9), Direction::Down);
  Shell s2(1, Point(5, 0), Direction::Up);

  s1.setPosition(Point(5, 0));
  s2.setPosition(Point(5, 9));

  std::vector<Shell> shells{s1, s2};
  std::vector<Tank> tanks;

  bool result = resolveAll(tanks, shells);

  EXPECT_TRUE(shells[0].isDestroyed());
  EXPECT_TRUE(shells[1].isDestroyed());

  ASSERT_EQ(getPathExplosions().size(), 1);
  MidPoint mp = getPathExplosions()[0];
  EXPECT_EQ(mp.getX(), 5);
  EXPECT_EQ(mp.getY(), 9);
  EXPECT_FALSE(mp.isHalfX());
  EXPECT_TRUE(mp.isHalfY());
  EXPECT_FALSE(result);
}

TEST_F(CollisionHandlerTest, Resolve_WraparoundCornerDiagonal_CollisionMidpointCorrect) {
  Shell s1(0, Point(9, 9), Direction::DownRight);
  Shell s2(1, Point(0, 0), Direction::UpLeft);

  s1.setPosition(Point(0, 0));
  s2.setPosition(Point(9, 9));

  std::vector<Shell> shells{s1, s2};
  std::vector<Tank> tanks;

  bool result = resolveAll(tanks, shells);

  EXPECT_TRUE(shells[0].isDestroyed());
  EXPECT_TRUE(shells[1].isDestroyed());

  ASSERT_EQ(getPathExplosions().size(), 1);
  MidPoint mp = getPathExplosions()[0];
  EXPECT_EQ(mp.getX(), 9);
  EXPECT_EQ(mp.getY(), 9);
  EXPECT_TRUE(mp.isHalfX());
  EXPECT_TRUE(mp.isHalfY());
  EXPECT_FALSE(result);
}

