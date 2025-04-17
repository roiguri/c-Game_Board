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


