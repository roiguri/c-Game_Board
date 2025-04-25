#include "gtest/gtest.h"
#include "bonus/config/board_config.h"
#include <fstream>
#include <string>

class BoardConfigTest : public ::testing::Test {
protected:
    // Helper to write a test config file
    std::string writeTestConfig(const std::string& content) {
        std::string filename = "test_board_config.txt";
        std::ofstream file(filename);
        file << content;
        file.close();
        return filename;
    }
    
    void TearDown() override {
        // Clean up any test files
        std::remove("test_board_config.txt");
    }
};

// Test default constructor
TEST_F(BoardConfigTest, DefaultConstructor) {
    BoardConfig config;
    
    EXPECT_EQ(config.width, 15);
    EXPECT_EQ(config.height, 10);
    EXPECT_FLOAT_EQ(config.wallDensity, 0.25f);
    EXPECT_FLOAT_EQ(config.mineDensity, 0.05f);
    EXPECT_EQ(config.symmetry, "none");
    EXPECT_EQ(config.seed, -1);
}

// Test loading a valid configuration
TEST_F(BoardConfigTest, LoadValidConfig) {
    std::string configContent = 
        "dimensions: 20 15\n"
        "wall_density: 0.4\n"
        "mine_density: 0.1\n"
        "symmetry: horizontal\n"
        "seed: 12345\n";
    
    std::string filename = writeTestConfig(configContent);
    
    BoardConfig config;
    EXPECT_TRUE(config.loadFromFile(filename));
    
    EXPECT_EQ(config.width, 20);
    EXPECT_EQ(config.height, 15);
    EXPECT_FLOAT_EQ(config.wallDensity, 0.4f);
    EXPECT_FLOAT_EQ(config.mineDensity, 0.1f);
    EXPECT_EQ(config.symmetry, "horizontal");
    EXPECT_EQ(config.seed, 12345);
}

// Test loading configuration with invalid dimensions
TEST_F(BoardConfigTest, InvalidDimensions) {
    std::string configContent = 
        "dimensions: 3 3\n" // Too small
        "wall_density: 0.4\n";
    
    std::string filename = writeTestConfig(configContent);
    
    BoardConfig config;
    EXPECT_FALSE(config.loadFromFile(filename));
    
    // Dimensions should remain at defaults
    EXPECT_EQ(config.width, 15);
    EXPECT_EQ(config.height, 10);
    
    // Other values should still be applied
    EXPECT_FLOAT_EQ(config.wallDensity, 0.4f);
}

// Test loading configuration with invalid symmetry
TEST_F(BoardConfigTest, InvalidSymmetry) {
    std::string configContent = 
        "symmetry: invalid_symmetry\n"
        "wall_density: 0.4\n";
    
    std::string filename = writeTestConfig(configContent);
    
    BoardConfig config;
    EXPECT_FALSE(config.loadFromFile(filename));
    
    // Symmetry should remain at default
    EXPECT_EQ(config.symmetry, "none");
    
    // Other values should still be applied
    EXPECT_FLOAT_EQ(config.wallDensity, 0.4f);
}

// Test loading configuration with invalid density values
TEST_F(BoardConfigTest, InvalidDensities) {
    std::string configContent = 
        "wall_density: 1.2\n" // Too high
        "mine_density: -0.1\n"; // Negative
    
    std::string filename = writeTestConfig(configContent);
    
    BoardConfig config;
    EXPECT_FALSE(config.loadFromFile(filename));
    
    // Densities should remain at defaults
    EXPECT_FLOAT_EQ(config.wallDensity, 0.25f);
    EXPECT_FLOAT_EQ(config.mineDensity, 0.05f);
}

// Test all symmetry options
TEST_F(BoardConfigTest, AllSymmetryOptions) {
    // Test horizontal symmetry
    std::string configHorizontal = "symmetry: horizontal\n";
    std::string filenameH = writeTestConfig(configHorizontal);
    
    BoardConfig configH;
    EXPECT_TRUE(configH.loadFromFile(filenameH));
    EXPECT_EQ(configH.symmetry, "horizontal");
    
    // Test vertical symmetry
    std::string configVertical = "symmetry: vertical\n";
    std::string filenameV = writeTestConfig(configVertical);
    
    BoardConfig configV;
    EXPECT_TRUE(configV.loadFromFile(filenameV));
    EXPECT_EQ(configV.symmetry, "vertical");
    
    // Test diagonal symmetry
    std::string configDiagonal = "symmetry: diagonal\n";
    std::string filenameD = writeTestConfig(configDiagonal);
    
    BoardConfig configD;
    EXPECT_TRUE(configD.loadFromFile(filenameD));
    EXPECT_EQ(configD.symmetry, "diagonal");
    
    // Test none symmetry
    std::string configNone = "symmetry: none\n";
    std::string filenameN = writeTestConfig(configNone);
    
    BoardConfig configN;
    EXPECT_TRUE(configN.loadFromFile(filenameN));
    EXPECT_EQ(configN.symmetry, "none");
}

// Test density boundary values
TEST_F(BoardConfigTest, DensityBoundaries) {
    // Test minimum values
    std::string configMin = 
        "wall_density: 0.0\n"
        "mine_density: 0.0\n";
    std::string filenameMin = writeTestConfig(configMin);
    
    BoardConfig configMinimum;
    EXPECT_TRUE(configMinimum.loadFromFile(filenameMin));
    EXPECT_FLOAT_EQ(configMinimum.wallDensity, 0.0f);
    EXPECT_FLOAT_EQ(configMinimum.mineDensity, 0.0f);
    
    // Test maximum values
    std::string configMax = 
        "wall_density: 0.9\n" // Maximum for walls
        "mine_density: 0.5\n"; // Maximum for mines
    std::string filenameMax = writeTestConfig(configMax);
    
    BoardConfig configMaximum;
    EXPECT_TRUE(configMaximum.loadFromFile(filenameMax));
    EXPECT_FLOAT_EQ(configMaximum.wallDensity, 0.9f);
    EXPECT_FLOAT_EQ(configMaximum.mineDensity, 0.5f);
}

// Test dimensions format
TEST_F(BoardConfigTest, DimensionsFormat) {
    // Test invalid dimensions format (missing height)
    std::string configBadFormat = "dimensions: 20\n";
    std::string filenameBad = writeTestConfig(configBadFormat);
    
    BoardConfig configBad;
    EXPECT_FALSE(configBad.loadFromFile(filenameBad));
    
    // Dimensions should remain at defaults
    EXPECT_EQ(configBad.width, 15);
    EXPECT_EQ(configBad.height, 10);
    
    // Test non-numeric dimensions
    std::string configNonNumeric = "dimensions: width height\n";
    std::string filenameNonNumeric = writeTestConfig(configNonNumeric);
    
    BoardConfig configNonNum;
    EXPECT_FALSE(configNonNum.loadFromFile(filenameNonNumeric));
    
    // Dimensions should remain at defaults
    EXPECT_EQ(configNonNum.width, 15);
    EXPECT_EQ(configNonNum.height, 10);
}