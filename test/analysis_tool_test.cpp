#include "gtest/gtest.h"
#include "src/analysis_tool.cpp" // Include cpp directly for simplicity, guarded by TEST_BUILD in analysis_tool.cpp's main
#include "bonus/board_generator.h" // For BoardConfig definition

// Test fixture for AnalysisTool tests
class AnalysisToolTest : public ::testing::Test {
protected:
    BoardConfig config1;
    BoardConfig config2;

    void SetUp() override {
        // Initialize default configs for testing GenerateKey
        config1.width = 10;
        config1.height = 10;
        config1.wallDensity = 0.1f;
        config1.mineDensity = 0.05f;
        config1.symmetryType = "none";
        config1.seed = 12345;
        config1.maxSteps = 500;
        config1.numShells = 10;
        config1.numTanksPerPlayer = 1;
        // Ensure all relevant fields are initialized for BoardConfig if it has more.
        // For example, if BoardConfig has minMines, maxMines, ensurePathExists, etc.
        // config1.minMines = 0;
        // config1.maxMines = 0;
        // config1.ensurePathExists = false;


        config2 = config1; // Start with a copy
    }
};

// --- Tests for ParseGameResult ---
TEST_F(AnalysisToolTest, ParsePlayer1Win) {
    EXPECT_EQ(ParseGameResult("Player 1 won with 3 tanks still alive"), Winner::PLAYER1);
    EXPECT_EQ(ParseGameResult("Round 100: Player 1 won with 1 tanks still alive"), Winner::PLAYER1);
}

TEST_F(AnalysisToolTest, ParsePlayer2Win) {
    EXPECT_EQ(ParseGameResult("Player 2 won with 1 tanks still alive"), Winner::PLAYER2);
    EXPECT_EQ(ParseGameResult("Round 50: Player 2 won with 5 tanks still alive"), Winner::PLAYER2);
}

TEST_F(AnalysisToolTest, ParseTie) {
    EXPECT_EQ(ParseGameResult("Tie, both players have zero tanks"), Winner::TIE);
    EXPECT_EQ(ParseGameResult("Tie, reached max steps = 1000, player 1 has 2 tanks, player 2 has 1 tanks"), Winner::TIE);
    EXPECT_EQ(ParseGameResult("Tie, reached max steps = 500, player 1 has 0 tanks, player 2 has 0 tanks"), Winner::TIE);
}

TEST_F(AnalysisToolTest, ParseUnknownResult) {
    EXPECT_EQ(ParseGameResult("Some unexpected game output"), Winner::UNKNOWN);
    EXPECT_EQ(ParseGameResult("Player 3 won!"), Winner::UNKNOWN);
    EXPECT_EQ(ParseGameResult(""), Winner::UNKNOWN); // Empty string
}

// --- Tests for GenerateKey ---
TEST_F(AnalysisToolTest, GenerateKeyIdenticalConfigs) {
    std::string key1 = GenerateKey(config1);
    std::string key2 = GenerateKey(config2); // config2 is a copy of config1
    EXPECT_EQ(key1, key2);
}

TEST_F(AnalysisToolTest, GenerateKeyDifferentWidth) {
    config2.width = 20;
    std::string key1 = GenerateKey(config1);
    std::string key2 = GenerateKey(config2);
    EXPECT_NE(key1, key2);
}

TEST_F(AnalysisToolTest, GenerateKeyDifferentHeight) {
    config2.height = 20;
    std::string key1 = GenerateKey(config1);
    std::string key2 = GenerateKey(config2);
    EXPECT_NE(key1, key2);
}

TEST_F(AnalysisToolTest, GenerateKeyDifferentWallDensity) {
    config2.wallDensity = 0.25f;
    std::string key1 = GenerateKey(config1);
    std::string key2 = GenerateKey(config2);
    EXPECT_NE(key1, key2);
}

TEST_F(AnalysisToolTest, GenerateKeyDifferentMineDensity) {
    config2.mineDensity = 0.15f;
    std::string key1 = GenerateKey(config1);
    std::string key2 = GenerateKey(config2);
    EXPECT_NE(key1, key2);
}

TEST_F(AnalysisToolTest, GenerateKeyDifferentSymmetryType) {
    config2.symmetryType = "horizontal";
    std::string key1 = GenerateKey(config1);
    std::string key2 = GenerateKey(config2);
    EXPECT_NE(key1, key2);
}

TEST_F(AnalysisToolTest, GenerateKeyDifferentSeed) {
    config2.seed = 54321;
    std::string key1 = GenerateKey(config1);
    std::string key2 = GenerateKey(config2);
    EXPECT_NE(key1, key2);
}

TEST_F(AnalysisToolTest, GenerateKeyDifferentMaxSteps) {
    config2.maxSteps = 1000;
    std::string key1 = GenerateKey(config1);
    std::string key2 = GenerateKey(config2);
    EXPECT_NE(key1, key2);
}

TEST_F(AnalysisToolTest, GenerateKeyDifferentNumShells) {
    config2.numShells = 20;
    std::string key1 = GenerateKey(config1);
    std::string key2 = GenerateKey(config2);
    EXPECT_NE(key1, key2);
}

TEST_F(AnalysisToolTest, GenerateKeyDifferentNumTanksPerPlayer) {
    config2.numTanksPerPlayer = 2;
    std::string key1 = GenerateKey(config1);
    std::string key2 = GenerateKey(config2);
    EXPECT_NE(key1, key2);
}

TEST_F(AnalysisToolTest, GenerateKeyFloatPrecision) {
    BoardConfig c1 = config1;
    BoardConfig c2 = config1;

    c1.wallDensity = 0.123f;
    c2.wallDensity = 0.1234f; // Should be different due to setprecision(3) in GenerateKey
    // GenerateKey uses std::fixed and std::setprecision(3) for densities.
    // So, 0.123f and 0.1234f will both be formatted as "0.123".
    // The test below expects them to be THE SAME if precision is correctly applied.
    
    // Re-thinking the precision test:
    // GenerateKey uses setprecision(3) for floats.
    // So 0.123f and 0.1234f should result in the same key string "0.123".
    // And 0.123f vs 0.124f should be different.

    std::string key_c1_wd_123 = GenerateKey(c1);

    c1.wallDensity = 0.123f; // Explicitly set
    c2.wallDensity = 0.1234f; // This will be truncated to 0.123 by setprecision(3)
    std::string key1_float = GenerateKey(c1);
    std::string key2_float_trunc = GenerateKey(c2);
    EXPECT_EQ(key1_float, key2_float_trunc) << "Keys should be same due to precision truncation for wallDensity";

    c1.mineDensity = 0.056f;
    c2.mineDensity = 0.0567f; // This will be truncated to 0.057 (NOTE: it rounds!) by setprecision(3)
    // Correction: setprecision(3) with std::fixed means 3 digits *after* the decimal point.
    // 0.056f -> "0.056"
    // 0.0567f -> "0.057" (std::fixed will round)
    // So these should be DIFFERENT.
    
    BoardConfig c3 = config1;
    c3.mineDensity = 0.056f;
    std::string key_c3_md_056 = GenerateKey(c3);
    
    BoardConfig c4 = config1;
    c4.mineDensity = 0.0567f; 
    std::string key_c4_md_057_rounded = GenerateKey(c4); // Should be "0.057"
    EXPECT_NE(key_c3_md_056, key_c4_md_057_rounded) << "Keys should be different due to rounding for mineDensity";

    BoardConfig c5 = config1;
    c5.mineDensity = 0.056f;
    BoardConfig c6 = config1;
    c6.mineDensity = 0.056f; // exactly same
    EXPECT_EQ(GenerateKey(c5), GenerateKey(c6)) << "Keys should be same for identical float values";

}
// It's important that BoardConfig is fully initialized in SetUp.
// If bonus/board_generator.h defines BoardConfig with more members than analysis_tool.cpp
// currently initializes (e.g. minMines, maxMines), those could lead to GenerateKey
// producing different keys if those uninitialized members have varying default values
// or contain garbage. The tests assume all members affecting GenerateKey are controlled.
// The BoardConfig struct in analysis_tool.cpp might need to be perfectly synced with
// the one in bonus/board_generator.h for these tests to be fully robust.
// For now, we assume analysis_tool.cpp's usage of BoardConfig is what we are testing GenerateKey against.

// To make this more robust, if BoardConfig had an official constructor or factory
// that initializes all members, tests should use that.
// Since it's a plain struct, manual initialization in SetUp is our best bet.
// The current BoardConfig in analysis_tool.cpp does not have minMines/maxMines etc.
// but the actual one in bonus/board_generator.h might. This is a known limitation.
// The `GenerateKey` function itself only uses fields explicitly mentioned.

// Make sure the BoardConfig used in tests has all fields used by GenerateKey initialized.
// The current GenerateKey uses: width, height, wallDensity, mineDensity, symmetryType, seed, maxSteps, numShells, numTanksPerPlayer.
// All these are initialized in SetUp().
