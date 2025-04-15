#include "gtest/gtest.h"
#include "shell.h"
#include "utils/point.h"
#include "utils/direction.h"

class ShellTest : public ::testing::Test {
  protected:
    void SetUp() override {
        playerId = 1;
        position = Point(5, 5);
        direction = Direction::Right;
        shell = new Shell(playerId, position, direction);
    }

    void TearDown() override {
        delete shell;
    }

    int playerId;
    Point position;
    Direction direction;
    Shell* shell;
};

TEST_F(ShellTest, Constructor) {
    EXPECT_EQ(shell->getPlayerId(), playerId);
    EXPECT_EQ(shell->getPosition().x, position.x);
    EXPECT_EQ(shell->getPosition().y, position.y);
    EXPECT_EQ(shell->getDirection(), direction);
    EXPECT_FALSE(shell->isDestroyed());
}

TEST_F(ShellTest, DifferentPlayers) {
    Shell player1Shell(1, Point(5, 5), Direction::Right);
    Shell player2Shell(2, Point(5, 5), Direction::Right);
    
    EXPECT_EQ(player1Shell.getPlayerId(), 1);
    EXPECT_EQ(player2Shell.getPlayerId(), 2);
}

TEST_F(ShellTest, DifferentPositions) {
    Shell shell1(1, Point(0, 0), Direction::Right);
    Shell shell2(1, Point(10, 20), Direction::Right);
    
    EXPECT_EQ(shell1.getPosition(), Point(0, 0));
    EXPECT_EQ(shell2.getPosition(), Point(10, 20));
}

TEST_F(ShellTest, DifferentDirections) {
    Shell shell1(1, Point(5, 5), Direction::Up);
    Shell shell2(1, Point(5, 5), Direction::DownLeft);
    
    EXPECT_EQ(shell1.getDirection(), Direction::Up);
    EXPECT_EQ(shell2.getDirection(), Direction::DownLeft);
}

TEST_F(ShellTest, InitialDestructionState) {
    Shell shell(1, Point(5, 5), Direction::Right);
    EXPECT_FALSE(shell.isDestroyed());
}

TEST_F(ShellTest, SetPosition) {
  EXPECT_EQ(shell->getPosition(), position);
  
  Point newPosition(10, 15);
  shell->setPosition(newPosition);
  EXPECT_EQ(shell->getPosition(), newPosition);
  
  Point anotherPosition(-5, 8);
  shell->setPosition(anotherPosition);
  EXPECT_EQ(shell->getPosition(), anotherPosition);
}

TEST_F(ShellTest, Destroy) {
  EXPECT_FALSE(shell->isDestroyed());
  
  shell->destroy();
  
  EXPECT_TRUE(shell->isDestroyed());
  
  shell->destroy();
  EXPECT_TRUE(shell->isDestroyed());
}

TEST_F(ShellTest, GetNextPosition) {
  // Test horizontal/vertical directions
  Shell rightShell(1, Point(5, 5), Direction::Right);
  EXPECT_EQ(rightShell.getNextPosition(), Point(7, 5)); // +2 in x

  Shell leftShell(1, Point(5, 5), Direction::Left);
  EXPECT_EQ(leftShell.getNextPosition(), Point(3, 5)); // -2 in x

  Shell upShell(1, Point(5, 5), Direction::Up);
  EXPECT_EQ(upShell.getNextPosition(), Point(5, 3)); // -2 in y (up is negative in y)

  Shell downShell(1, Point(5, 5), Direction::Down);
  EXPECT_EQ(downShell.getNextPosition(), Point(5, 7)); // +2 in y
  
  // Test diagonal directions
  Shell upRightShell(1, Point(5, 5), Direction::UpRight);
  EXPECT_EQ(upRightShell.getNextPosition(), Point(7, 3)); // +2 in x, -2 in y

  Shell downRightShell(1, Point(5, 5), Direction::DownRight);
  EXPECT_EQ(downRightShell.getNextPosition(), Point(7, 7)); // +2 in x, +2 in y

  Shell downLeftShell(1, Point(5, 5), Direction::DownLeft);
  EXPECT_EQ(downLeftShell.getNextPosition(), Point(3, 7)); // -2 in x, +2 in y

  Shell upLeftShell(1, Point(5, 5), Direction::UpLeft);
  EXPECT_EQ(upLeftShell.getNextPosition(), Point(3, 3)); // -2 in x, -2 in y
}