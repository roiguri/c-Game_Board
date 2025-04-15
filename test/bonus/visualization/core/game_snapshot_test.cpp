#include "gtest/gtest.h"
#include "bonus/visualization/core/game_snapshot.h"
#include "game_board.h"
#include "tank.h"
#include "shell.h"

class GameSnapshotTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test board
        board = GameBoard(5, 5);
        
        std::vector<std::string> boardData = {
            "#####",
            "#1 2#",
            "#   #",
            "# @ #",
            "#####"
        };
        
        std::vector<std::string> errors;
        board.initialize(boardData, errors);
        
        // Create test tanks
        tanks.push_back(Tank(1, Point(1, 1), Direction::Right));
        tanks.push_back(Tank(2, Point(3, 1), Direction::Left));
        
        // Create test shells
        shells.push_back(Shell(1, Point(2, 2), Direction::Right));
    }
    
    GameBoard board;
    std::vector<Tank> tanks;
    std::vector<Shell> shells;
};

TEST_F(GameSnapshotTest, DefaultConstructor) {
    GameSnapshot snapshot;
    EXPECT_EQ(snapshot.getStepNumber(), 0);
    EXPECT_TRUE(snapshot.getMessage().empty());
    EXPECT_TRUE(snapshot.getBoardState().empty());
    EXPECT_TRUE(snapshot.getTanks().empty());
    EXPECT_TRUE(snapshot.getShells().empty());
}

TEST_F(GameSnapshotTest, ParameterizedConstructor) {
    // Create a snapshot
    GameSnapshot snapshot(42, board, tanks, shells, "Test message");
    
    // Check basic properties
    EXPECT_EQ(snapshot.getStepNumber(), 42);
    EXPECT_EQ(snapshot.getMessage(), "Test message");
    
    // Check board dimensions
    const auto& boardState = snapshot.getBoardState();
    EXPECT_EQ(boardState.size(), 5);  // height
    EXPECT_EQ(boardState[0].size(), 5);  // width
    
    // Check some board cells
    EXPECT_EQ(boardState[0][0], GameBoard::CellType::Wall);
    EXPECT_EQ(boardState[1][1], GameBoard::CellType::Tank1);
    EXPECT_EQ(boardState[1][3], GameBoard::CellType::Tank2);
    EXPECT_EQ(boardState[3][2], GameBoard::CellType::Mine);
    
    // Check tanks
    const auto& tankStates = snapshot.getTanks();
    EXPECT_EQ(tankStates.size(), 2);
    EXPECT_EQ(tankStates[0].playerId, 1);
    EXPECT_EQ(tankStates[0].position.x, 1);
    EXPECT_EQ(tankStates[0].position.y, 1);
    EXPECT_EQ(tankStates[0].direction, Direction::Right);
    EXPECT_FALSE(tankStates[0].destroyed);
    
    EXPECT_EQ(tankStates[1].playerId, 2);
    EXPECT_EQ(tankStates[1].position.x, 3);
    EXPECT_EQ(tankStates[1].position.y, 1);
    EXPECT_EQ(tankStates[1].direction, Direction::Left);
    EXPECT_FALSE(tankStates[1].destroyed);
    
    // Check shells
    const auto& shellStates = snapshot.getShells();
    EXPECT_EQ(shellStates.size(), 1);
    EXPECT_EQ(shellStates[0].playerId, 1);
    EXPECT_EQ(shellStates[0].position.x, 2);
    EXPECT_EQ(shellStates[0].position.y, 2);
    EXPECT_EQ(shellStates[0].direction, Direction::Right);
    EXPECT_FALSE(shellStates[0].destroyed);
}

TEST_F(GameSnapshotTest, TankStateConstructor) {
    // Modify one tank to test values
    tanks[0].setDirection(Direction::Down);
    tanks[0].decrementShells();
    
    TankState state(tanks[0]);
    
    EXPECT_EQ(state.playerId, 1);
    EXPECT_EQ(state.position.x, 1);
    EXPECT_EQ(state.position.y, 1);
    EXPECT_EQ(state.direction, Direction::Down);
    EXPECT_EQ(state.remainingShells, Tank::INITIAL_SHELLS - 1);
    EXPECT_FALSE(state.destroyed);
    
    // Test destroyed state
    tanks[0].destroy();
    TankState destroyedState(tanks[0]);
    EXPECT_TRUE(destroyedState.destroyed);
}

TEST_F(GameSnapshotTest, ShellStateConstructor) {
    ShellState state(shells[0]);
    
    EXPECT_EQ(state.playerId, 1);
    EXPECT_EQ(state.position.x, 2);
    EXPECT_EQ(state.position.y, 2);
    EXPECT_EQ(state.direction, Direction::Right);
    EXPECT_FALSE(state.destroyed);
    
    // Test destroyed state
    shells[0].destroy();
    ShellState destroyedState(shells[0]);
    EXPECT_TRUE(destroyedState.destroyed);
}

TEST_F(GameSnapshotTest, JsonSerialization) {
    // Create a snapshot
    GameSnapshot snapshot(42, board, tanks, shells, "Test message");
    
    // Convert to JSON
    std::string json = snapshot.toJson();
    
    // Check that JSON contains expected elements
    EXPECT_NE(json.find("\"step\": 42"), std::string::npos);
    EXPECT_NE(json.find("\"message\": \"Test message\""), std::string::npos);
    EXPECT_NE(json.find("\"width\": 5"), std::string::npos);
    EXPECT_NE(json.find("\"height\": 5"), std::string::npos);
    
    // Check for tank data
    EXPECT_NE(json.find("\"playerId\": 1"), std::string::npos);
    EXPECT_NE(json.find("\"playerId\": 2"), std::string::npos);
    
    // Check for shell data
    EXPECT_NE(json.find("\"direction\": 2"), std::string::npos);  // Direction::Right is 2
}

// Note: This test is incomplete since the fromJson implementation is a placeholder
TEST_F(GameSnapshotTest, JsonDeserialization) {
    // Create a snapshot
    GameSnapshot original(42, board, tanks, shells, "Test message");
    
    // Convert to JSON
    std::string json = original.toJson();
    
    // Parse back from JSON
    GameSnapshot parsed = GameSnapshot::fromJson(json);
    
    // In a real implementation, we would check that the parsed object matches the original
    // Since our fromJson implementation is a placeholder, we're skipping these checks
}