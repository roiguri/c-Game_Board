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