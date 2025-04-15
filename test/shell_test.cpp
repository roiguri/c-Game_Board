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