#include "gtest/gtest.h"
#include "bonus/config/game_config.h"
#include <fstream>
#include <string>

class GameConfigTest : public ::testing::Test {
protected:
    // Helper to write a test config file
    std::string writeTestConfig(const std::string& content) {
        std::string filename = "test_game_config.txt";
        std::ofstream file(filename);
        file << content;
        file.close();
        return filename;
    }
    
    void TearDown() override {
        // Clean up any test files
        std::remove("test_game_config.txt");
    }
};

// Test default constructor
TEST_F(GameConfigTest, DefaultConstructor) {
    GameConfig config;
    
    EXPECT_EQ(config.initialShells, 16);
    EXPECT_EQ(config.shootCooldown, 4);
    EXPECT_EQ(config.backwardDelay, 2);
    EXPECT_EQ(config.wallHealth, 2);
    EXPECT_EQ(config.shellsDepletedCountdown, 40);
    EXPECT_EQ(config.maxGameSteps, 1000);
}

// Test loading a valid configuration
TEST_F(GameConfigTest, LoadValidConfig) {
    std::string configContent = 
        "initial_shells: 20\n"
        "shoot_cooldown: 3\n"
        "backward_delay: 1\n"
        "wall_health: 3\n"
        "shells_depleted_countdown: 30\n"
        "max_game_steps: 500\n";
    
    std::string filename = writeTestConfig(configContent);
    
    GameConfig config;
    EXPECT_TRUE(config.loadFromFile(filename));
    
    EXPECT_EQ(config.initialShells, 20);
    EXPECT_EQ(config.shootCooldown, 3);
    EXPECT_EQ(config.backwardDelay, 1);
    EXPECT_EQ(config.wallHealth, 3);
    EXPECT_EQ(config.shellsDepletedCountdown, 30);
    EXPECT_EQ(config.maxGameSteps, 500);
}

// Test loading configuration with invalid values
TEST_F(GameConfigTest, InvalidValues) {
    std::string configContent = 
        "initial_shells: 0\n" // Too low
        "shoot_cooldown: -1\n" // Negative
        "wall_health: 0\n" // Too low
        "shell_move_speed: 0\n"; // Too low
    
    std::string filename = writeTestConfig(configContent);
    
    GameConfig config;
    EXPECT_FALSE(config.loadFromFile(filename));
    
    // Values should remain at defaults
    EXPECT_EQ(config.initialShells, 16);
    EXPECT_EQ(config.shootCooldown, 4);
    EXPECT_EQ(config.wallHealth, 2);
}

// Test loading configuration with some valid and some invalid values
TEST_F(GameConfigTest, MixedValidInvalid) {
    std::string configContent = 
        "initial_shells: 20\n" // Valid
        "shoot_cooldown: -1\n" // Invalid
        "wall_health: 3\n"; // Valid
    
    std::string filename = writeTestConfig(configContent);
    
    GameConfig config;
    EXPECT_FALSE(config.loadFromFile(filename)); // Overall result is false due to invalid value
    
    // Valid values should be applied
    EXPECT_EQ(config.initialShells, 20);
    
    // Invalid value should remain at default
    EXPECT_EQ(config.shootCooldown, 4);
}

// Test value boundaries
TEST_F(GameConfigTest, ValueBoundaries) {
    // Test minimum valid values
    std::string configMin = 
        "initial_shells: 1\n"
        "shoot_cooldown: 0\n"
        "backward_delay: 0\n"
        "wall_health: 1\n"
        "shells_depleted_countdown: 0\n"
        "max_game_steps: 1\n";
    
    std::string filenameMin = writeTestConfig(configMin);
    
    GameConfig configMinimum;
    EXPECT_TRUE(configMinimum.loadFromFile(filenameMin));
    
    EXPECT_EQ(configMinimum.initialShells, 1);
    EXPECT_EQ(configMinimum.shootCooldown, 0);
    EXPECT_EQ(configMinimum.backwardDelay, 0);
    EXPECT_EQ(configMinimum.wallHealth, 1);
    EXPECT_EQ(configMinimum.shellsDepletedCountdown, 0);
    EXPECT_EQ(configMinimum.maxGameSteps, 1);
    
    // Test maximum valid values (using reasonable upper bounds)
    std::string configMax = 
        "initial_shells: 100\n"
        "shoot_cooldown: 20\n"
        "backward_delay: 10\n"
        "wall_health: 10\n"
        "shells_depleted_countdown: 1000\n"
        "max_game_steps: 10000\n";
    
    std::string filenameMax = writeTestConfig(configMax);
    
    GameConfig configMaximum;
    EXPECT_TRUE(configMaximum.loadFromFile(filenameMax));
    
    EXPECT_EQ(configMaximum.initialShells, 100);
    EXPECT_EQ(configMaximum.shootCooldown, 20);
    EXPECT_EQ(configMaximum.backwardDelay, 10);
    EXPECT_EQ(configMaximum.wallHealth, 10);
    EXPECT_EQ(configMaximum.shellsDepletedCountdown, 1000);
    EXPECT_EQ(configMaximum.maxGameSteps, 10000);
}

// Test loading configuration with unknown key
TEST_F(GameConfigTest, UnknownKey) {
    std::string configContent = 
        "initial_shells: 20\n"
        "unknown_key: 100\n"
        "wall_health: 3\n";
    
    std::string filename = writeTestConfig(configContent);
    
    GameConfig config;
    EXPECT_FALSE(config.loadFromFile(filename)); // Should fail due to unknown key
    
    // Valid values should still be applied
    EXPECT_EQ(config.initialShells, 20);
    EXPECT_EQ(config.wallHealth, 3);
}

// Test non-integer values
TEST_F(GameConfigTest, NonIntegerValues) {
    std::string configContent = 
        "initial_shells: twenty\n" // Non-integer
        "wall_health: 3.5\n"; // Float for int parameter
    
    std::string filename = writeTestConfig(configContent);
    
    GameConfig config;
    EXPECT_FALSE(config.loadFromFile(filename));
    
    EXPECT_EQ(config.initialShells, 16); // remain at default
    EXPECT_EQ(config.wallHealth, 3); // rounded to 3
}