#include "gtest/gtest.h"
#include "collision_handler.h"
#include "game_board.h"
#include "shell.h"
#include "tank.h"
#include <vector>

// Test fixture for CollisionHandler tests
class CollisionHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a default 5x5 board for testing
        board = GameBoard(5, 5);
        
        // Add some walls to the board
        board.setCellType(1, 1, GameBoard::CellType::Wall);
        board.setCellType(3, 3, GameBoard::CellType::Wall);
    }
    
    GameBoard board;
};

// Test shell-wall collision when there is a wall
TEST_F(CollisionHandlerTest, ShellWallCollision_HitsWall) {
    // Create a shell at a wall position
    Point wallPosition(1, 1);
    Shell shell(1, wallPosition, Direction::Right);
    
    // Verify there's a wall at the position and shell is active
    EXPECT_TRUE(board.isWall(wallPosition));
    EXPECT_FALSE(shell.isDestroyed());
    
    // Check the initial wall health
    EXPECT_EQ(board.getWallHealth(wallPosition), GameBoard::WALL_STARTING_HEALTH);
    
    // Execute the collision check
    bool wallDestroyed = CollisionHandler::checkShellWallCollision(board, shell);
    
    // Verify the shell was destroyed
    EXPECT_TRUE(shell.isDestroyed());
    
    // Verify the wall was damaged but not destroyed (assuming starting health > 1)
    EXPECT_FALSE(wallDestroyed);
    EXPECT_EQ(board.getWallHealth(wallPosition), GameBoard::WALL_STARTING_HEALTH - 1);
    EXPECT_TRUE(board.isWall(wallPosition));
}

// Test shell-wall collision when there is no wall
TEST_F(CollisionHandlerTest, ShellWallCollision_NoWall) {
    // Create a shell at a non-wall position
    Point emptyPosition(2, 2);
    Shell shell(1, emptyPosition, Direction::Right);
    
    // Verify there's no wall at the position and shell is active
    EXPECT_FALSE(board.isWall(emptyPosition));
    EXPECT_FALSE(shell.isDestroyed());
    
    // Execute the collision check
    bool wallDestroyed = CollisionHandler::checkShellWallCollision(board, shell);
    
    // Verify the shell wasn't destroyed
    EXPECT_FALSE(shell.isDestroyed());
    
    // Verify no wall was affected
    EXPECT_FALSE(wallDestroyed);
}

// Test shell-wall collision for an already destroyed shell
TEST_F(CollisionHandlerTest, ShellWallCollision_ShellAlreadyDestroyed) {
    // Create a shell at a wall position but mark it as already destroyed
    Point wallPosition(1, 1);
    Shell shell(1, wallPosition, Direction::Right);
    shell.destroy();
    
    // Verify there's a wall at the position but shell is inactive
    EXPECT_TRUE(board.isWall(wallPosition));
    EXPECT_TRUE(shell.isDestroyed());
    
    // Check the initial wall health
    int initialHealth = board.getWallHealth(wallPosition);
    
    // Execute the collision check
    bool wallDestroyed = CollisionHandler::checkShellWallCollision(board, shell);
    
    // Verify the wall wasn't damaged
    EXPECT_FALSE(wallDestroyed);
    EXPECT_EQ(board.getWallHealth(wallPosition), initialHealth);
}

// Test shell-wall collision that destroys the wall
TEST_F(CollisionHandlerTest, ShellWallCollision_WallDestroyed) {
    // Create a shell at a wall position
    Point wallPosition(1, 1);
    Shell shell(1, wallPosition, Direction::Right);
    
    // Damage the wall to leave it with 1 health
    board.damageWall(wallPosition);
    
    // Verify there's a wall at the position with 1 health
    EXPECT_TRUE(board.isWall(wallPosition));
    EXPECT_EQ(board.getWallHealth(wallPosition), 1);
    
    // Execute the collision check
    bool wallDestroyed = CollisionHandler::checkShellWallCollision(board, shell);
    
    // Verify the shell was destroyed
    EXPECT_TRUE(shell.isDestroyed());
    
    // Verify the wall was destroyed
    EXPECT_TRUE(wallDestroyed);
    EXPECT_FALSE(board.isWall(wallPosition));
    EXPECT_EQ(board.getCellType(wallPosition), GameBoard::CellType::Empty);
}


// Test shell-shell collision when shells are at the same position
TEST_F(CollisionHandlerTest, ShellShellCollision_SamePosition) {
    // Shell1 lives outside the vector
    Shell shell1(1, Point(2, 2), Direction::Right);

    // Shell2 lives inside the vector, like gameplay
    std::vector<Shell> shells;
    shells.emplace_back(2, Point(2, 2), Direction::Left);
    Shell& shell2 = shells.back();  // reference to actual Shell in vector

    std::vector<Point> explosionPositions;

    bool tankDestroyed = CollisionHandler::checkShellShellCollision(shell1, shells, explosionPositions);

    EXPECT_TRUE(shell1.isDestroyed());
    EXPECT_TRUE(shell2.isDestroyed());  // ✅ this works now!
    EXPECT_FALSE(tankDestroyed);

    EXPECT_EQ(explosionPositions.size(), 1);
    EXPECT_EQ(explosionPositions[0], Point(2, 2));
}

// Test shell-shell collision when shells are at different positions
TEST_F(CollisionHandlerTest, ShellShellCollision_DifferentPositions) {
    // Create two shells at different positions
    Shell shell1(1, Point(2, 2), Direction::Right);
    Shell shell2(2, Point(3, 3), Direction::Left);
    
    std::vector<Shell> shells = {shell2};
    std::vector<Point> explosionPositions;
    
    // Check for collision
    bool tankDestroyed = CollisionHandler::checkShellShellCollision(shell1, shells, explosionPositions);
    
    // Verify no shells were destroyed
    EXPECT_FALSE(shell1.isDestroyed());
    EXPECT_FALSE(shell2.isDestroyed());
    EXPECT_FALSE(tankDestroyed);
    
    // Verify no explosion positions were tracked
    EXPECT_TRUE(explosionPositions.empty());
}


// Test shell-tank collision when shell and tank are at the same position
TEST_F(CollisionHandlerTest, ShellTankCollision_SamePosition) {
    Shell shell(1, Point(2, 2), Direction::Right);
    Tank tank(2, Point(2, 2), Direction::Left);

    std::vector<Tank> tanks = {tank};
    std::vector<Point> explosionPositions;

    bool result = CollisionHandler::checkShellTankCollision(shell, tanks, explosionPositions);

    EXPECT_TRUE(shell.isDestroyed());
    EXPECT_TRUE(tanks[0].isDestroyed());
    EXPECT_TRUE(result);

    ASSERT_EQ(explosionPositions.size(), 1);
    EXPECT_EQ(explosionPositions[0], Point(2, 2));
}

// Test shell-tank collision when shell and tank are at different positions
TEST_F(CollisionHandlerTest, ShellTankCollision_DifferentPosition) {
    Shell shell(1, Point(2, 2), Direction::Right);
    Tank tank(2, Point(3, 3), Direction::Left);

    std::vector<Tank> tanks = {tank};
    std::vector<Point> explosionPositions;

    bool result = CollisionHandler::checkShellTankCollision(shell, tanks, explosionPositions);

    EXPECT_FALSE(shell.isDestroyed());
    EXPECT_FALSE(tanks[0].isDestroyed());
    EXPECT_FALSE(result);
    EXPECT_TRUE(explosionPositions.empty());
}

// Test shell-tank collision when shell is already destroyed
TEST_F(CollisionHandlerTest, ShellTankCollision_ShellAlreadyDestroyed) {
    Shell shell(1, Point(2, 2), Direction::Right);
    shell.destroy();

    Tank tank(2, Point(2, 2), Direction::Left);
    std::vector<Tank> tanks = {tank};
    std::vector<Point> explosionPositions;

    bool result = CollisionHandler::checkShellTankCollision(shell, tanks, explosionPositions);

    EXPECT_TRUE(shell.isDestroyed());
    EXPECT_FALSE(tanks[0].isDestroyed());  // no collision happened
    EXPECT_FALSE(result);
    EXPECT_TRUE(explosionPositions.empty());
}

// Test shell-tank collision with multiple tanks at same position
TEST_F(CollisionHandlerTest, ShellTankCollision_MultipleTanksSamePosition) {
    Shell shell(1, Point(2, 2), Direction::Right);
    Tank tank1(2, Point(2, 2), Direction::Left);
    Tank tank2(2, Point(2, 2), Direction::Down);

    std::vector<Tank> tanks = {tank1, tank2};
    std::vector<Point> explosionPositions;

    bool result = CollisionHandler::checkShellTankCollision(shell, tanks, explosionPositions);

    EXPECT_TRUE(shell.isDestroyed());
    EXPECT_TRUE(tanks[0].isDestroyed());
    EXPECT_TRUE(tanks[1].isDestroyed());
    EXPECT_TRUE(result);
    ASSERT_EQ(explosionPositions.size(), 1);
    EXPECT_EQ(explosionPositions[0], Point(2, 2));
}

TEST_F(CollisionHandlerTest, ResolveCollisions_ExplosionAtSamePositionKillsAll) {
    Shell s1(1, Point(2, 2), Direction::Right);
    Shell s2(2, Point(2, 2), Direction::Left);
    Tank t1(1, Point(2, 2), Direction::Up);

    std::vector<Shell> shells = {s1, s2};
    std::vector<Tank> tanks = {t1};

    bool result = CollisionHandler::resolveAllCollisions(board, shells, tanks);

    EXPECT_TRUE(shells[0].isDestroyed());
    EXPECT_TRUE(shells[1].isDestroyed());
    EXPECT_TRUE(tanks[0].isDestroyed());
    EXPECT_TRUE(result);
}

TEST_F(CollisionHandlerTest, ResolveCollisions_ShellKillsTankDirectly) {
    Shell s1(1, Point(1, 1), Direction::Right);
    Tank t1(2, Point(1, 1), Direction::Up);

    std::vector<Shell> shells = {s1};
    std::vector<Tank> tanks = {t1};

    bool result = CollisionHandler::resolveAllCollisions(board, shells, tanks);

    EXPECT_TRUE(shells[0].isDestroyed());
    EXPECT_TRUE(tanks[0].isDestroyed());
    EXPECT_TRUE(result);
}

TEST_F(CollisionHandlerTest, ResolveCollisions_ExplosionDestroysMine) {
    Point pos(3, 3);
    board.setCellType(pos, GameBoard::CellType::Mine);

    Shell s1(1, pos, Direction::Right);
    Shell s2(2, pos, Direction::Left);

    std::vector<Shell> shells = {s1, s2};
    std::vector<Tank> tanks = {};

    bool result = CollisionHandler::resolveAllCollisions(board, shells, tanks);

    EXPECT_TRUE(shells[0].isDestroyed());
    EXPECT_TRUE(shells[1].isDestroyed());
    EXPECT_FALSE(result);
    EXPECT_EQ(board.getCellType(pos), GameBoard::CellType::Empty);  // mine removed
}

TEST_F(CollisionHandlerTest, ResolveCollisions_NoOverlap_NoDestruction) {
    Shell s1(1, Point(0, 0), Direction::Right);
    Shell s2(2, Point(4, 4), Direction::Left);
    Tank t1(1, Point(1, 2), Direction::Up);
    Tank t2(2, Point(3, 3), Direction::Down);

    std::vector<Shell> shells = {s1, s2};
    std::vector<Tank> tanks = {t1, t2};

    bool result = CollisionHandler::resolveAllCollisions(board, shells, tanks);

    EXPECT_FALSE(shells[0].isDestroyed());
    EXPECT_FALSE(shells[1].isDestroyed());
    EXPECT_FALSE(tanks[0].isDestroyed());
    EXPECT_FALSE(tanks[1].isDestroyed());
    EXPECT_FALSE(result);
}

TEST_F(CollisionHandlerTest, ResolveCollisions_MixedDestructionAtOneTile) {
    Point chaos = Point(2, 2);
    board.setCellType(chaos, GameBoard::CellType::Mine);

    Shell s1(1, chaos, Direction::Right);
    Shell s2(2, chaos, Direction::Left);
    Tank t1(1, chaos, Direction::Up);
    Tank t2(2, chaos, Direction::Down);

    std::vector<Shell> shells = {s1, s2};
    std::vector<Tank> tanks = {t1, t2};

    bool result = CollisionHandler::resolveAllCollisions(board, shells, tanks);

    EXPECT_TRUE(shells[0].isDestroyed());
    EXPECT_TRUE(shells[1].isDestroyed());
    EXPECT_TRUE(tanks[0].isDestroyed());
    EXPECT_TRUE(tanks[1].isDestroyed());
    EXPECT_EQ(board.getCellType(chaos), GameBoard::CellType::Empty);  // mine destroyed
    EXPECT_TRUE(result);
}

TEST_F(CollisionHandlerTest, ResolveCollisions_TankTankCollision_BothDestroyed) {
    Tank t1(1, Point(2, 2), Direction::Up);
    Tank t2(2, Point(2, 2), Direction::Down);

    std::vector<Shell> shells = {};  // No shells
    std::vector<Tank> tanks = {t1, t2};

    bool result = CollisionHandler::resolveAllCollisions(board, shells, tanks);

    EXPECT_TRUE(tanks[0].isDestroyed());
    EXPECT_TRUE(tanks[1].isDestroyed());
    EXPECT_TRUE(result);
}

TEST_F(CollisionHandlerTest, ResolveCollisions_ThreeTankCollision_AllDestroyed) {
    Tank t1(1, Point(1, 1), Direction::Up);
    Tank t2(2, Point(1, 1), Direction::Down);
    Tank t3(1, Point(1, 1), Direction::Left);

    std::vector<Shell> shells = {};
    std::vector<Tank> tanks = {t1, t2, t3};

    bool result = CollisionHandler::resolveAllCollisions(board, shells, tanks);

    for (const auto& tank : tanks) {
        EXPECT_TRUE(tank.isDestroyed());
    }

    EXPECT_TRUE(result);
}

TEST_F(CollisionHandlerTest, ResolveCollisions_TankStepsOnMine) {
    Point pos = Point(3, 3);
    board.setCellType(pos, GameBoard::CellType::Mine);

    Tank t1(1, pos, Direction::Up);
    std::vector<Shell> shells = {};
    std::vector<Tank> tanks = {t1};

    bool result = CollisionHandler::resolveAllCollisions(board, shells, tanks);

    EXPECT_TRUE(tanks[0].isDestroyed());
    EXPECT_EQ(board.getCellType(pos), GameBoard::CellType::Empty);  // mine gone
    EXPECT_TRUE(result);
}

TEST_F(CollisionHandlerTest, ResolveCollisions_TankNotOnMine_Survives) {
    Point minePos = Point(1, 1);
    Point tankPos = Point(2, 2);
    board.setCellType(minePos, GameBoard::CellType::Mine);

    Tank t1(1, tankPos, Direction::Right);
    std::vector<Shell> shells = {};
    std::vector<Tank> tanks = {t1};

    bool result = CollisionHandler::resolveAllCollisions(board, shells, tanks);

    EXPECT_FALSE(tanks[0].isDestroyed());
    EXPECT_EQ(board.getCellType(minePos), GameBoard::CellType::Mine);
    EXPECT_FALSE(result);
}

TEST_F(CollisionHandlerTest, ResolveCollisions_AllTypesInOneStep) {
    // Shell-shell + tank-tank + shell-tank + tank-mine + mine in explosion area
    board.setCellType(Point(1, 1), GameBoard::CellType::Mine);       // tank-mine
    board.setCellType(Point(2, 2), GameBoard::CellType::Mine);       // explosion target

    std::vector<Shell> shells = {
        Shell(1, Point(0, 0), Direction::Right),     // part of explosion
        Shell(2, Point(3, 3), Direction::Left),      // part of explosion
        Shell(1, Point(2, 2), Direction::Up),        // part of explosion
        Shell(2, Point(2, 2), Direction::Down)       // part of explosion
    };

    std::vector<Tank> tanks = {
        Tank(1, Point(0, 0), Direction::Down),       // part of explosion
        Tank(1, Point(1, 1), Direction::Up),         // dies to mine
        Tank(2, Point(1, 1), Direction::Down),       // dies to tank-mine
        Tank(1, Point(2, 2), Direction::Up),         // dies to explosion
        Tank(2, Point(3, 3), Direction::Down),       // dies to explosion
        Tank(1, Point(4, 4), Direction::Left),       // 
        Tank(2, Point(4, 4), Direction::Right)       // tank-tank collision
    };

    bool result = CollisionHandler::resolveAllCollisions(board, shells, tanks);

    // Shells
    EXPECT_TRUE(shells[0].isDestroyed());
    EXPECT_TRUE(shells[1].isDestroyed());
    EXPECT_TRUE(shells[2].isDestroyed());
    EXPECT_TRUE(shells[3].isDestroyed());

    // Tanks
    EXPECT_TRUE(tanks[0].isDestroyed());
    EXPECT_TRUE(tanks[1].isDestroyed());  // tank on mine
    EXPECT_TRUE(tanks[2].isDestroyed());  // tank on mine
    EXPECT_TRUE(tanks[3].isDestroyed());  // exploded
    EXPECT_TRUE(tanks[4].isDestroyed());
    EXPECT_TRUE(tanks[5].isDestroyed());  // tank-tank
    EXPECT_TRUE(tanks[6].isDestroyed());  // tank-tank

    // Mines destroyed
    EXPECT_EQ(board.getCellType(Point(1, 1)), GameBoard::CellType::Empty);
    EXPECT_EQ(board.getCellType(Point(2, 2)), GameBoard::CellType::Empty);

    EXPECT_TRUE(result);  // some tank was destroyed
}

TEST_F(CollisionHandlerTest, ResolveCollisions_EverythingSurvives) {
    board.setCellType(Point(3, 3), GameBoard::CellType::Mine); // not touched

    std::vector<Shell> shells = {
        Shell(1, Point(0, 0), Direction::Right),
        Shell(2, Point(1, 0), Direction::Left)
    };

    std::vector<Tank> tanks = {
        Tank(1, Point(4, 4), Direction::Up),
        Tank(2, Point(2, 3), Direction::Down)
    };

    bool result = CollisionHandler::resolveAllCollisions(board, shells, tanks);

    // Nothing should be destroyed
    EXPECT_FALSE(shells[0].isDestroyed());
    EXPECT_FALSE(shells[1].isDestroyed());
    EXPECT_FALSE(tanks[0].isDestroyed());
    EXPECT_FALSE(tanks[1].isDestroyed());

    EXPECT_EQ(board.getCellType(Point(3, 3)), GameBoard::CellType::Mine);
    EXPECT_FALSE(result);
}

TEST_F(CollisionHandlerTest, ResolveCollisions_ExplosionChainDestroysMineAndTank) {
    Point explosionCenter = Point(2, 2);
    board.setCellType(explosionCenter, GameBoard::CellType::Mine);

    std::vector<Shell> shells = {
        Shell(1, explosionCenter, Direction::Up),
        Shell(2, explosionCenter, Direction::Down)
    };

    std::vector<Tank> tanks = {
        Tank(1, explosionCenter, Direction::Left)  // tank + mine + shell-shell
    };

    bool result = CollisionHandler::resolveAllCollisions(board, shells, tanks);

    EXPECT_TRUE(shells[0].isDestroyed());
    EXPECT_TRUE(shells[1].isDestroyed());
    EXPECT_TRUE(tanks[0].isDestroyed());
    EXPECT_EQ(board.getCellType(explosionCenter), GameBoard::CellType::Empty);
    EXPECT_TRUE(result);
}

TEST_F(CollisionHandlerTest, ResolveCollisions_MixedSurvivorsAndCasualties) {
    board.setCellType(Point(2, 2), GameBoard::CellType::Mine);  // Will explode
    board.setCellType(Point(4, 4), GameBoard::CellType::Mine);  // Should remain

    std::vector<Shell> shells = {
        Shell(1, Point(0, 0), Direction::Right),     // survives
        Shell(2, Point(2, 2), Direction::Left),      // part of explosion
        Shell(1, Point(3, 3), Direction::Up)         // survives
    };

    std::vector<Tank> tanks = {
        Tank(1, Point(2, 2), Direction::Down),       // on mine — dies
        Tank(2, Point(2, 2), Direction::Up),         // tank-tank with one above — dies
        Tank(1, Point(1, 1), Direction::Right),      // survives
        Tank(2, Point(4, 4), Direction::Left)        // dies
    };

    bool result = CollisionHandler::resolveAllCollisions(board, shells, tanks);

    EXPECT_FALSE(shells[0].isDestroyed());
    EXPECT_FALSE(shells[2].isDestroyed());
    EXPECT_FALSE(tanks[2].isDestroyed());
    
    EXPECT_TRUE(shells[1].isDestroyed());
    EXPECT_TRUE(tanks[0].isDestroyed());
    EXPECT_TRUE(tanks[1].isDestroyed());
    EXPECT_TRUE(tanks[3].isDestroyed());

    EXPECT_EQ(board.getCellType(Point(2, 2)), GameBoard::CellType::Empty);
    EXPECT_EQ(board.getCellType(Point(4, 4)), GameBoard::CellType::Empty);

    EXPECT_TRUE(result);
}








