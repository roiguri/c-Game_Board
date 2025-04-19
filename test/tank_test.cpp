#include "gtest/gtest.h"
#include "tank.h"

class TankTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Default setup creates a tank for player 1 at position (3, 4) facing Right
        playerId = 1;
        position = Point(3, 4);
        direction = Direction::Right;
        tank = new Tank(playerId, position, direction);
    }

    void TearDown() override {
        delete tank;
    }

    // Common test data
    int playerId;
    Point position;
    Direction direction;
    Tank* tank;
};

// Constructor and Basic State Tests
TEST_F(TankTest, Constructor_InitializesCorrectly) {
    EXPECT_EQ(tank->getPlayerId(), playerId);
    EXPECT_EQ(tank->getPosition(), position);
    EXPECT_EQ(tank->getDirection(), direction);
    EXPECT_EQ(tank->getRemainingShells(), Tank::INITIAL_SHELLS);
    EXPECT_FALSE(tank->isDestroyed());
    EXPECT_FALSE(tank->isMovingBackward());
    EXPECT_FALSE(tank->isContinuousBackward());
    EXPECT_EQ(tank->getBackwardCounter(), 0);
}

// State Mutator Tests
TEST_F(TankTest, SetPosition_ChangesPosition) {
    Point newPosition(5, 6);
    tank->setPosition(newPosition);
    EXPECT_EQ(tank->getPosition(), newPosition);
}

TEST_F(TankTest, SetDirection_ChangesDirection) {
    Direction newDirection = Direction::Down;
    tank->setDirection(newDirection);
    EXPECT_EQ(tank->getDirection(), newDirection);
}

TEST_F(TankTest, DecrementShells_ReducesShellCount) {
  int initialShells = tank->getRemainingShells();
  tank->decrementShells();
  EXPECT_EQ(tank->getRemainingShells(), initialShells - 1);
}

TEST_F(TankTest, DecrementShells_StopsAtZero) {
  // Empty all shells
  for (int i = 0; i < Tank::INITIAL_SHELLS; i++) {
      tank->decrementShells();
  }
  EXPECT_EQ(tank->getRemainingShells(), 0);
  
  // Try to decrement again
  tank->decrementShells();
  EXPECT_EQ(tank->getRemainingShells(), 0);
}

TEST_F(TankTest, Destroy_MarksTankAsDestroyed) {
  tank->destroy();
  EXPECT_TRUE(tank->isDestroyed());
}

TEST_F(TankTest, MoveForward_UpdatesPosition) {
  Point newPosition(4, 4);
  EXPECT_TRUE(tank->moveForward(newPosition));
  EXPECT_EQ(tank->getPosition(), newPosition);
}

TEST_F(TankTest, RotateLeft_EighthTurn) {
  Direction initialDirection = Direction::Right;
  tank->setDirection(initialDirection);
  
  EXPECT_TRUE(tank->rotateLeft(/*quarterTurn=*/false));
  EXPECT_EQ(tank->getDirection(), Direction::UpRight);
  
  EXPECT_TRUE(tank->rotateLeft(/*quarterTurn=*/false));
  EXPECT_EQ(tank->getDirection(), Direction::Up);
}

TEST_F(TankTest, RotateLeft_QuarterTurn) {
  Direction initialDirection = Direction::Right;
  tank->setDirection(initialDirection);
  
  EXPECT_TRUE(tank->rotateLeft(/*quarterTurn=*/true));
  EXPECT_EQ(tank->getDirection(), Direction::Up);
  
  EXPECT_TRUE(tank->rotateLeft(/*quarterTurn=*/true));
  EXPECT_EQ(tank->getDirection(), Direction::Left);
}

TEST_F(TankTest, RotateRight_EighthTurn) {
  Direction initialDirection = Direction::Right;
  tank->setDirection(initialDirection);
  
  EXPECT_TRUE(tank->rotateRight(/*quarterTurn=*/false));
  EXPECT_EQ(tank->getDirection(), Direction::DownRight);
  
  EXPECT_TRUE(tank->rotateRight(/*quarterTurn=*/false));
  EXPECT_EQ(tank->getDirection(), Direction::Down);
}

TEST_F(TankTest, RotateRight_QuarterTurn) {
  Direction initialDirection = Direction::Right;
  tank->setDirection(initialDirection);
  
  EXPECT_TRUE(tank->rotateRight(/*quarterTurn=*/true));
  EXPECT_EQ(tank->getDirection(), Direction::Down);
  
  EXPECT_TRUE(tank->rotateRight(/*quarterTurn=*/true));
  EXPECT_EQ(tank->getDirection(), Direction::Left);
}

TEST_F(TankTest, GetNextForwardPosition) {
  // Initial position is (3, 4) and initial direction is Right
  EXPECT_EQ(tank->getNextForwardPosition(), Point(4, 4));
  
  tank->setDirection(Direction::Down);
  EXPECT_EQ(tank->getNextForwardPosition(), Point(3, 5));
  
  tank->setDirection(Direction::UpLeft);
  EXPECT_EQ(tank->getNextForwardPosition(), Point(2, 3));
}

TEST_F(TankTest, GetNextBackwardPosition) {
  // Initial position is (3, 4) and initial direction is Right
  EXPECT_EQ(tank->getNextBackwardPosition(), Point(2, 4));
  
  tank->setDirection(Direction::Down);
  EXPECT_EQ(tank->getNextBackwardPosition(), Point(3, 3));
  
  tank->setDirection(Direction::UpLeft);
  EXPECT_EQ(tank->getNextBackwardPosition(), Point(4, 5));
}

// Shooting Tests
TEST_F(TankTest, CanShoot_InitiallyTrue) {
  EXPECT_TRUE(tank->canShoot());
}

TEST_F(TankTest, CanShoot_FalseWhenDestroyed) {
  tank->destroy();
  EXPECT_FALSE(tank->canShoot());
}

TEST_F(TankTest, CanShoot_FalseWhenNoShells) {
  // Use all shells
  for (int i = 0; i < Tank::INITIAL_SHELLS; i++) {
      tank->decrementShells();
  }
  EXPECT_FALSE(tank->canShoot());
}

TEST_F(TankTest, Shoot_SuccessfulShot) {
  int initialShells = tank->getRemainingShells();
  EXPECT_TRUE(tank->shoot());
  EXPECT_EQ(tank->getRemainingShells(), initialShells - 1);
  EXPECT_FALSE(tank->canShoot());
}

TEST_F(TankTest, Shoot_FailsOnCooldown) {
  EXPECT_TRUE(tank->shoot());
  
  int shellsAfterFirstShot = tank->getRemainingShells();
  EXPECT_FALSE(tank->shoot());
  EXPECT_EQ(tank->getRemainingShells(), shellsAfterFirstShot);
}

TEST_F(TankTest, UpdateCooldowns_DecrementsCooldown) {
  // Activate cooldown
  EXPECT_TRUE(tank->shoot());
  EXPECT_FALSE(tank->canShoot());
  
  // Update cooldowns Tank::SHOOT_COOLDOWN times
  for (int i = 0; i < Tank::SHOOT_COOLDOWN; i++) {
      EXPECT_FALSE(tank->canShoot());
      tank->updateCooldowns();
  }
  
  EXPECT_TRUE(tank->canShoot());
}

TEST_F(TankTest, Shoot_FailsWhenNoShells) {
  // Use all shells
  for (int i = 0; i < Tank::INITIAL_SHELLS; i++) {
      tank->decrementShells();
  }
  
  EXPECT_FALSE(tank->shoot());
}

TEST_F(TankTest, Shoot_FailsWhenDestroyed) {
  tank->destroy();
  EXPECT_FALSE(tank->shoot());
}

// Backward Movement Tests
TEST_F(TankTest, MoveBackward_InitiatesBackwardMoveWithDelay) {
  Point backwardPosition = tank->getNextBackwardPosition();
  Point originalPosition = tank->getPosition();
  
  // First backward move request should start the process but not move the tank
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_EQ(tank->getPosition(), originalPosition); 
  EXPECT_TRUE(tank->isMovingBackward()); 
  EXPECT_EQ(tank->getBackwardCounter(), 1);
  
  // Second request should increment the counter but still not move
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_EQ(tank->getPosition(), originalPosition);
  EXPECT_TRUE(tank->isMovingBackward()); 
  EXPECT_EQ(tank->getBackwardCounter(), 2);
  
  // Third request should actually move the tank
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_EQ(tank->getPosition(), backwardPosition); 
  EXPECT_FALSE(tank->isMovingBackward()); 
  EXPECT_EQ(tank->getBackwardCounter(), 0); 
  EXPECT_TRUE(tank->isContinuousBackward());
}

TEST_F(TankTest, MoveBackward_ContinuousMovesTakeOneStep) {
  Point backwardPosition1 = tank->getNextBackwardPosition();
  // Set up the continuous backward state
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition1));
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition1));
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition1));
  EXPECT_TRUE(tank->isContinuousBackward());
  
  Point tankPos = tank->getPosition();
  Point backwardPosition2 = tankPos - getDirectionDelta(tank->getDirection());
  
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition2));
  EXPECT_EQ(tank->getPosition(), backwardPosition2);
  EXPECT_TRUE(tank->isContinuousBackward());
}

TEST_F(TankTest, MoveForward_CancelsPendingBackwardMove) {
  Point backwardPosition = tank->getNextBackwardPosition();
  Point originalPosition = tank->getPosition();
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->isMovingBackward());
  
  EXPECT_TRUE(tank->moveForward(originalPosition));
  EXPECT_FALSE(tank->isMovingBackward());
  EXPECT_EQ(tank->getPosition(), originalPosition);
}

TEST_F(TankTest, RotateRight_IgnoredDuringBackwardDelay) {
  Point backwardPosition = tank->getNextBackwardPosition();
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->isMovingBackward());
  
  Direction initialDirection = tank->getDirection();
  EXPECT_FALSE(tank->rotateRight(false));
  EXPECT_EQ(tank->getDirection(), initialDirection);
  
  EXPECT_TRUE(tank->isMovingBackward());
}

TEST_F(TankTest, RotateLeft_IgnoredDuringBackwardDelay) {
  Point backwardPosition = tank->getNextBackwardPosition();
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->isMovingBackward());
  
  Direction initialDirection = tank->getDirection();
  EXPECT_FALSE(tank->rotateLeft(false));
  EXPECT_EQ(tank->getDirection(), initialDirection);
  
  EXPECT_TRUE(tank->isMovingBackward());
}

TEST_F(TankTest, Shoot_IgnoredDuringBackwardDelay) {
  Point backwardPosition = tank->getNextBackwardPosition();
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->isMovingBackward());
  
  int initialShells = tank->getRemainingShells();
  EXPECT_FALSE(tank->shoot());
  EXPECT_EQ(tank->getRemainingShells(), initialShells);
  
  EXPECT_TRUE(tank->isMovingBackward());
}

TEST_F(TankTest, ContinuousBackward_ResetByOtherActions) {
  Point backwardPosition = tank->getNextBackwardPosition();
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->isContinuousBackward());
  
  EXPECT_TRUE(tank->moveForward(tank->getPosition()));
  EXPECT_FALSE(tank->isContinuousBackward());
  
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->isContinuousBackward());
  
  EXPECT_TRUE(tank->rotateLeft(false));
  EXPECT_FALSE(tank->isContinuousBackward());
  
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->isContinuousBackward());
  
  EXPECT_TRUE(tank->shoot());
  EXPECT_FALSE(tank->isContinuousBackward());
}

TEST_F(TankTest, ResetBackwardMovement_ResetsAllBackwardState) {
  Point backwardPosition = tank->getNextBackwardPosition();
  
  // Set up a backward move in progress
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->isMovingBackward());
  
  tank->resetBackwardMovement();

  EXPECT_FALSE(tank->isMovingBackward());
  EXPECT_FALSE(tank->isContinuousBackward());
  EXPECT_EQ(tank->getBackwardCounter(), 0);
  
  // Set up continuous backward state
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->requestMoveBackward(backwardPosition));
  EXPECT_TRUE(tank->isContinuousBackward());
  
  tank->resetBackwardMovement();

  EXPECT_FALSE(tank->isContinuousBackward());
}