#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include <fstream>
#include "command_line_parser.h"

using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::UnorderedElementsAre;

class CommandLineParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser.reset();
        
        // Create temporary test files and directories
        std::filesystem::create_directories("test_temp/game_managers");
        std::filesystem::create_directories("test_temp/algorithms");
        std::filesystem::create_directories("test_temp/maps");
        
        std::ofstream("test_temp/map.txt").close();
        std::ofstream("test_temp/gamemanager.so").close();
        std::ofstream("test_temp/algo1.so").close();
        std::ofstream("test_temp/algo2.so").close();
        std::ofstream("test_temp/game_managers/gm1.so").close();
        std::ofstream("test_temp/algorithms/alg1.so").close();
        std::ofstream("test_temp/maps/map1.txt").close();
    }
    
    void TearDown() override {
        std::filesystem::remove_all("test_temp");
    }
    
    CommandLineParser::ParseResult parseArgs(const std::vector<std::string>& args) {
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>("program"));
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        return parser.parse(static_cast<int>(argv.size()), argv.data());
    }
    
    CommandLineParser parser;
};

// ===== BASIC MODE TESTS =====

TEST_F(CommandLineParserTest, BasicMode_ValidArguments) {
    auto result = parseArgs({"-basic", "game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so", 
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so"});
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.mode, CommandLineParser::Mode::Basic);
    EXPECT_EQ(parser.getMapFile(), "test_temp/map.txt");
    EXPECT_EQ(parser.getGameManagerLib(), "test_temp/gamemanager.so");
    EXPECT_EQ(parser.getAlgorithm1Lib(), "test_temp/algo1.so");
    EXPECT_EQ(parser.getAlgorithm2Lib(), "test_temp/algo2.so");
    EXPECT_FALSE(parser.isVerbose());
    EXPECT_FALSE(parser.getNumThreads().has_value());
}

TEST_F(CommandLineParserTest, BasicMode_WithVerbose) {
    auto result = parseArgs({"-basic", "game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so", 
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so", "-verbose"});
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.mode, CommandLineParser::Mode::Basic);
    EXPECT_TRUE(parser.isVerbose());
}

TEST_F(CommandLineParserTest, BasicMode_DifferentOrder) {
    auto result = parseArgs({"-verbose", "algorithm2=test_temp/algo2.so", "-basic", "game_map=test_temp/map.txt", 
                           "game_manager=test_temp/gamemanager.so", "algorithm1=test_temp/algo1.so"});
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.mode, CommandLineParser::Mode::Basic);
    EXPECT_TRUE(parser.isVerbose());
}

TEST_F(CommandLineParserTest, BasicMode_NumThreads_ShouldFail) {
    auto result = parseArgs({"-basic", "game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so", 
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so", "num_threads=2"});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.unsupportedParams, ElementsAre("num_threads=2"));
}

TEST_F(CommandLineParserTest, BasicMode_MissingFlag) {
    auto result = parseArgs({"game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so", 
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so"});
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.mode, CommandLineParser::Mode::Unknown);
}

TEST_F(CommandLineParserTest, BasicMode_MissingParameters) {
    auto result = parseArgs({"-basic", "game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so"});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.missingParams, UnorderedElementsAre("algorithm1", "algorithm2"));
}

TEST_F(CommandLineParserTest, BasicMode_UnsupportedParameter) {
    auto result = parseArgs({"-basic", "game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so", 
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so", "unsupported=value"});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.unsupportedParams, ElementsAre("unsupported=value"));
}

TEST_F(CommandLineParserTest, BasicMode_PositionalArgument_ShouldFail) {
    auto result = parseArgs({"-basic", "game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so", 
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so", "positional_arg"});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.unsupportedParams, ElementsAre("positional_arg"));
}

// ===== COMPARATIVE MODE TESTS =====

TEST_F(CommandLineParserTest, ComparativeMode_ValidArguments) {
    auto result = parseArgs({"-comparative", "game_map=test_temp/map.txt", 
                           "game_managers_folder=test_temp/game_managers",
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so"});
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.mode, CommandLineParser::Mode::Comparative);
    EXPECT_EQ(parser.getGameMap(), "test_temp/map.txt");
    EXPECT_EQ(parser.getGameManagersFolder(), "test_temp/game_managers");
    EXPECT_EQ(parser.getAlgorithm1(), "test_temp/algo1.so");
    EXPECT_EQ(parser.getAlgorithm2(), "test_temp/algo2.so");
}

TEST_F(CommandLineParserTest, ComparativeMode_DifferentOrder) {
    auto result = parseArgs({"algorithm2=test_temp/algo2.so", "-comparative", 
                           "game_map=test_temp/map.txt", "algorithm1=test_temp/algo1.so",
                           "game_managers_folder=test_temp/game_managers"});
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.mode, CommandLineParser::Mode::Comparative);
}

TEST_F(CommandLineParserTest, ComparativeMode_MissingRequired) {
    auto result = parseArgs({"-comparative", "game_map=test_temp/map.txt", 
                           "algorithm1=test_temp/algo1.so"});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.missingParams, UnorderedElementsAre("game_managers_folder", "algorithm2"));
}

// ===== COMPETITION MODE TESTS =====

TEST_F(CommandLineParserTest, CompetitionMode_ValidArguments) {
    auto result = parseArgs({"-competition", "game_maps_folder=test_temp/maps", 
                           "game_manager=test_temp/gamemanager.so",
                           "algorithms_folder=test_temp/algorithms"});
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.mode, CommandLineParser::Mode::Competition);
    EXPECT_EQ(parser.getGameMapsFolder(), "test_temp/maps");
    EXPECT_EQ(parser.getGameManager(), "test_temp/gamemanager.so");
    EXPECT_EQ(parser.getAlgorithmsFolder(), "test_temp/algorithms");
}

TEST_F(CommandLineParserTest, CompetitionMode_MissingRequired) {
    auto result = parseArgs({"-competition", "game_maps_folder=test_temp/maps"});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.missingParams, UnorderedElementsAre("game_manager", "algorithms_folder"));
}

// ===== KEY=VALUE PARSING TESTS =====

TEST_F(CommandLineParserTest, KeyValueParsing_NoSpacesAroundEquals) {
    auto result = parseArgs({"-basic", "game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so",
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so"});
    
    EXPECT_TRUE(result.success);
}

TEST_F(CommandLineParserTest, KeyValueParsing_EmptyKey) {
    auto result = parseArgs({"-basic", "=test_temp/map.txt"});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.unsupportedParams, ElementsAre("=test_temp/map.txt"));
}

TEST_F(CommandLineParserTest, KeyValueParsing_EmptyValue) {
    auto result = parseArgs({"-basic", "game_map="});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.unsupportedParams, ElementsAre("game_map="));
}

TEST_F(CommandLineParserTest, KeyValueParsing_NoEquals) {
    auto result = parseArgs({"-basic", "game_map_test_temp_map.txt"});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.unsupportedParams, ElementsAre("game_map_test_temp_map.txt"));
}

// ===== NUM_THREADS VALIDATION TESTS =====

TEST_F(CommandLineParserTest, NumThreads_ValidPositive) {
    auto result = parseArgs({"-comparative", "game_map=test_temp/map.txt", 
                           "game_managers_folder=test_temp/game_managers",
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so",
                           "num_threads=16"});
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(parser.getNumThreads().value(), 16);
}

TEST_F(CommandLineParserTest, NumThreads_Zero) {
    auto result = parseArgs({"-comparative", "game_map=test_temp/map.txt", 
                           "game_managers_folder=test_temp/game_managers",
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so",
                           "num_threads=0"});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.unsupportedParams, ElementsAre("num_threads=0 (must be positive)"));
}

TEST_F(CommandLineParserTest, NumThreads_Negative) {
    auto result = parseArgs({"-comparative", "game_map=test_temp/map.txt", 
                           "game_managers_folder=test_temp/game_managers",
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so",
                           "num_threads=-5"});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.unsupportedParams, ElementsAre("num_threads=-5 (must be positive)"));
}

TEST_F(CommandLineParserTest, NumThreads_InvalidNumber) {
    auto result = parseArgs({"-comparative", "game_map=test_temp/map.txt", 
                           "game_managers_folder=test_temp/game_managers",
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so",
                           "num_threads=abc"});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.unsupportedParams, ElementsAre("num_threads=abc (invalid number)"));
}

// ===== MODE CONFLICT TESTS =====

TEST_F(CommandLineParserTest, ModeConflict_BasicAndComparative) {
    auto result = parseArgs({"-basic", "-comparative", "game_map=test_temp/map.txt"});
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.mode, CommandLineParser::Mode::Unknown);
}

TEST_F(CommandLineParserTest, ModeConflict_AllThreeFlags) {
    auto result = parseArgs({"-basic", "-comparative", "-competition"});
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.mode, CommandLineParser::Mode::Unknown);
}

TEST_F(CommandLineParserTest, NoModeFlag_WithKeyValueParams) {
    auto result = parseArgs({"game_map=test_temp/map.txt", "algorithm1=test_temp/algo1.so"});
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.mode, CommandLineParser::Mode::Unknown);
}

// ===== VERBOSE FLAG TESTS =====

TEST_F(CommandLineParserTest, VerboseFlag_SingleDashOnly) {
    auto result = parseArgs({"-basic", "game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so",
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so",
                           "-verbose"});
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(parser.isVerbose());
}

TEST_F(CommandLineParserTest, VerboseFlag_DoubleDash_ShouldFail) {
    auto result = parseArgs({"-basic", "game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so",
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so",
                           "--verbose"});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.unsupportedParams, ElementsAre("--verbose"));
}

// ===== ACCESSOR TESTS FOR WRONG MODE =====

TEST_F(CommandLineParserTest, BasicModeAccessors_WrongMode) {
    parseArgs({"-comparative", "game_map=test_temp/map.txt", 
             "game_managers_folder=test_temp/game_managers",
             "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so"});
    
    EXPECT_EQ(parser.getMapFile(), "");
    EXPECT_EQ(parser.getGameManagerLib(), "");
    EXPECT_EQ(parser.getAlgorithm1Lib(), "");
    EXPECT_EQ(parser.getAlgorithm2Lib(), "");
}

TEST_F(CommandLineParserTest, ComparativeModeAccessors_WrongMode) {
    parseArgs({"-basic", "game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so",
             "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so"});
    
    EXPECT_EQ(parser.getGameMap(), "");
    EXPECT_EQ(parser.getGameManagersFolder(), "");
    EXPECT_EQ(parser.getAlgorithm1(), "");
    EXPECT_EQ(parser.getAlgorithm2(), "");
}

TEST_F(CommandLineParserTest, CompetitionModeAccessors_WrongMode) {
    parseArgs({"-basic", "game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so",
             "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so"});
    
    EXPECT_EQ(parser.getGameMapsFolder(), "");
    EXPECT_EQ(parser.getGameManager(), "");
    EXPECT_EQ(parser.getAlgorithmsFolder(), "");
}

// ===== USAGE MESSAGE TEST =====

TEST_F(CommandLineParserTest, GenerateUsage) {
    std::string usage = parser.generateUsage("simulator_test");
    
    EXPECT_NE(usage.find("Basic mode:"), std::string::npos);
    EXPECT_NE(usage.find("Comparative mode:"), std::string::npos);
    EXPECT_NE(usage.find("Competition mode:"), std::string::npos);
    EXPECT_NE(usage.find("simulator_test"), std::string::npos);
    EXPECT_NE(usage.find("no spaces around the = sign"), std::string::npos);
    EXPECT_NE(usage.find("-basic"), std::string::npos);
    EXPECT_NE(usage.find("Mode flag (-basic, -comparative, -competition) is required"), std::string::npos);
}

// ===== RESET FUNCTIONALITY =====

TEST_F(CommandLineParserTest, ResetFunctionality) {
    parseArgs({"-basic", "game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so",
             "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so",
             "-verbose"});
    
    parser.reset();
    
    EXPECT_FALSE(parser.isVerbose());
    EXPECT_FALSE(parser.getNumThreads().has_value());
    EXPECT_EQ(parser.getMapFile(), "");
    EXPECT_EQ(parser.getGameMap(), "");
}

// ===== EDGE CASES =====

TEST_F(CommandLineParserTest, EmptyArguments) {
    auto result = parseArgs({});
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.mode, CommandLineParser::Mode::Unknown);
}

TEST_F(CommandLineParserTest, OnlyModeFlag) {
    auto result = parseArgs({"-basic"});
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.mode, CommandLineParser::Mode::Basic);
    EXPECT_FALSE(result.missingParams.empty());
}

TEST_F(CommandLineParserTest, UnknownFlag) {
    auto result = parseArgs({"-basic", "game_map=test_temp/map.txt", "game_manager=test_temp/gamemanager.so",
                           "algorithm1=test_temp/algo1.so", "algorithm2=test_temp/algo2.so",
                           "--unknown-flag"});
    
    EXPECT_FALSE(result.success);
    EXPECT_THAT(result.unsupportedParams, ElementsAre("--unknown-flag"));
}