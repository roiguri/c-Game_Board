#include "gtest/gtest.h"
#include "collision_handler.h"
#include "game_board.h"
#include "tank.h"
#include "shell.h"

class CollisionHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test board of 5x5
        testBoard = GameBoard(5, 5);
        
        // Initialize with some walls and mines for testing
        std::vector<std::string> boardLines = {
            "#####",
            "#1 2#",
            "#   #",
            "# @ #",
            "#####"
        };
        
        std::vector<std::string> errors;
        testBoard.initialize(boardLines, errors);
        
        // Add tanks to the tanks vector
        tanks.push_back(Tank(1, Point(1, 1), Direction::Right));
        tanks.push_back(Tank(2, Point(3, 1), Direction::Left));
    }
    
    // Test objects
    GameBoard testBoard;
    std::vector<Shell> shells;
    std::vector<Tank> tanks;
};

// Basic test to ensure the test fixture is set up correctly
TEST_F(CollisionHandlerTest, TestFixtureSetup) {
    EXPECT_EQ(testBoard.getWidth(), 5);
    EXPECT_EQ(testBoard.getHeight(), 5);
    EXPECT_EQ(tanks.size(), 2);
    EXPECT_EQ(tanks[0].getPlayerId(), 1);
    EXPECT_EQ(tanks[1].getPlayerId(), 2);
    
    // Verify the board layout
    EXPECT_EQ(testBoard.getCellType(Point(0, 0)), GameBoard::CellType::Wall);
    EXPECT_EQ(testBoard.getCellType(Point(2, 3)), GameBoard::CellType::Mine);
}


TEST_F(CollisionHandlerTest, ShellWallCollision_ShellDestroyed) {
    // Place a shell on a wall position
    Point wallPos(0, 0);
    ASSERT_TRUE(testBoard.isWall(wallPos));
    
    Shell shell(1, wallPos, Direction::Right);
    EXPECT_FALSE(shell.isDestroyed());
    
    // Check collision
    bool result = CollisionHandler::checkShellWallCollision(testBoard, shell);
    
    // Shell should be destroyed
    EXPECT_TRUE(shell.isDestroyed());
    // Wall should be damaged but not destroyed yet
    EXPECT_TRUE(testBoard.isWall(wallPos));
    // No tank was destroyed
    EXPECT_FALSE(result);
    
    // Wall health should be reduced
    EXPECT_EQ(testBoard.getWallHealth(wallPos), GameBoard::WALL_STARTING_HEALTH - 1);
}

TEST_F(CollisionHandlerTest, ShellWallCollision_WallDestroyed) {
    // Place a shell on a wall position
    Point wallPos(0, 0);
    ASSERT_TRUE(testBoard.isWall(wallPos));
    
    // Damage the wall first so it's down to 1 health
    testBoard.damageWall(wallPos);
    EXPECT_EQ(testBoard.getWallHealth(wallPos), 1);
    
    Shell shell(1, wallPos, Direction::Right);
    
    // Check collision
    bool result = CollisionHandler::checkShellWallCollision(testBoard, shell);
    
    // Shell should be destroyed
    EXPECT_TRUE(shell.isDestroyed());
    // Wall should be destroyed
    EXPECT_FALSE(testBoard.isWall(wallPos));
    // No tank was destroyed
    EXPECT_FALSE(result);
    
    // Cell should now be empty
    EXPECT_EQ(testBoard.getCellType(wallPos), GameBoard::CellType::Empty);
}

TEST_F(CollisionHandlerTest, ShellWallCollision_NoCollision) {
    // Place a shell on an empty position
    Point emptyPos(2, 2);
    ASSERT_EQ(testBoard.getCellType(emptyPos), GameBoard::CellType::Empty);
    
    Shell shell(1, emptyPos, Direction::Right);
    
    // Check collision
    bool result = CollisionHandler::checkShellWallCollision(testBoard, shell);
    
    // Shell should not be destroyed
    EXPECT_FALSE(shell.isDestroyed());
    // No tank was destroyed
    EXPECT_FALSE(result);
}

TEST_F(CollisionHandlerTest, MultipleShellsOnWall_CumulativeDamage) {
    // Place two shells on the same wall
    Point wallPos(0, 0);
    ASSERT_TRUE(testBoard.isWall(wallPos));
    
    // Verify initial wall health
    EXPECT_EQ(testBoard.getWallHealth(wallPos), GameBoard::WALL_STARTING_HEALTH);
    
    Shell shell1(1, wallPos, Direction::Right);
    Shell shell2(2, wallPos, Direction::Left);
    
    // Check both shells for wall collision
    bool result1 = CollisionHandler::checkShellWallCollision(testBoard, shell1);
    bool result2 = CollisionHandler::checkShellWallCollision(testBoard, shell2);
    
    // Both shells should be destroyed
    EXPECT_TRUE(shell1.isDestroyed());
    EXPECT_TRUE(shell2.isDestroyed());
    
    // Wall should have received 2 damage and be destroyed
    EXPECT_FALSE(testBoard.isWall(wallPos));
    EXPECT_EQ(testBoard.getCellType(wallPos), GameBoard::CellType::Empty);
    
    // No tanks destroyed in these collisions
    EXPECT_FALSE(result1);
    EXPECT_FALSE(result2);
}