#include "gtest/gtest.h"
#include "game_object.h"
#include "utils/point.h"
#include "utils/direction.h"

class GameObjectTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Default setup creates a game object for player 1 at position (3, 4) facing Right
        playerId = 1;
        position = Point(3, 4);
        direction = Direction::Right;
        gameObject = new GameObject(playerId, position, direction);
    }

    void TearDown() override {
        delete gameObject;
    }

    // Common test data
    int playerId;
    Point position;
    Direction direction;
    GameObject* gameObject;
};

// Constructor and Basic State Tests
TEST_F(GameObjectTest, Constructor_InitializesCorrectly) {
    EXPECT_EQ(gameObject->getPlayerId(), playerId);
    EXPECT_EQ(gameObject->getPosition(), position);
    EXPECT_EQ(gameObject->getDirection(), direction);
    EXPECT_FALSE(gameObject->isDestroyed());
    EXPECT_EQ(gameObject->getPreviousPosition(), position); // Previous position should be initialized to current position
}

// State Mutator Tests
TEST_F(GameObjectTest, SetPosition_ChangesPosition) {
    Point newPosition(5, 6);
    gameObject->setPosition(newPosition);
    EXPECT_EQ(gameObject->getPosition(), newPosition);
    EXPECT_EQ(gameObject->getPreviousPosition(), position); // Previous position should be updated to old position
}

TEST_F(GameObjectTest, SetDirection_ChangesDirection) {
    Direction newDirection = Direction::Down;
    gameObject->setDirection(newDirection);
    EXPECT_EQ(gameObject->getDirection(), newDirection);
}

TEST_F(GameObjectTest, Destroy_MarksAsDestroyed) {
    gameObject->destroy();
    EXPECT_TRUE(gameObject->isDestroyed());
}

TEST_F(GameObjectTest, UpdatePreviousPosition_ExplicitUpdate) {
    Point originalPosition = gameObject->getPosition();
    
    // Call updatePreviousPosition
    gameObject->updatePreviousPosition();
    
    // Set position directly (bypassing setPosition to test updatePreviousPosition)
    Point newPosition(7, 8);
    gameObject->setPosition(newPosition);
    
    // Check previous position was correctly updated
    EXPECT_EQ(gameObject->getPreviousPosition(), originalPosition);
}

TEST_F(GameObjectTest, MultiplePositionChanges_TracksPreviousPositions) {
    Point firstPosition = gameObject->getPosition(); // (3, 4)
    Point secondPosition(5, 6);
    Point thirdPosition(7, 8);
    
    // First position change
    gameObject->setPosition(secondPosition);
    EXPECT_EQ(gameObject->getPosition(), secondPosition);
    EXPECT_EQ(gameObject->getPreviousPosition(), firstPosition);
    
    // Second position change
    gameObject->setPosition(thirdPosition);
    EXPECT_EQ(gameObject->getPosition(), thirdPosition);
    EXPECT_EQ(gameObject->getPreviousPosition(), secondPosition);
}

TEST_F(GameObjectTest, MultiplePlayerIds_DifferentObjects) {
    // Create game objects for different players
    GameObject player1Object(1, Point(1, 1), Direction::Right);
    GameObject player2Object(2, Point(2, 2), Direction::Left);
    
    // Check player IDs are set correctly
    EXPECT_EQ(player1Object.getPlayerId(), 1);
    EXPECT_EQ(player2Object.getPlayerId(), 2);
}

TEST_F(GameObjectTest, DestroyedObject_StateIsPreserved) {
    // Set up object with specific state
    Point testPosition(5, 5);
    Direction testDirection = Direction::UpRight;
    gameObject->setPosition(testPosition);
    gameObject->setDirection(testDirection);
    
    // Destroy the object
    gameObject->destroy();
    
    // Check that other state is preserved
    EXPECT_TRUE(gameObject->isDestroyed());
    EXPECT_EQ(gameObject->getPosition(), testPosition);
    EXPECT_EQ(gameObject->getDirection(), testDirection);
    EXPECT_EQ(gameObject->getPlayerId(), playerId);
}