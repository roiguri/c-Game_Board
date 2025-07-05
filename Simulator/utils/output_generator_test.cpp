#include "gtest/gtest.h"
#include "output_generator.h"
#include "test/mocks/MockSatelliteView.h"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <regex>

class OutputGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = "test_output_dir";
        testFile = testDir + "/test_output.txt";
        
        // Create test directory
        std::filesystem::create_directories(testDir);
    }
    
    void TearDown() override {
        // Clean up test directory
        std::error_code ec;
        std::filesystem::remove_all(testDir, ec);
    }
    
    std::string readFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return "";
        }
        std::string content;
        std::string line;
        while (std::getline(file, line)) {
            content += line + "\n";
        }
        // Remove trailing newline if present
        if (!content.empty() && content.back() == '\n') {
            content.pop_back();
        }
        return content;
    }
    
    GameResult createTestGameResult(int winner = 1, GameResult::Reason reason = GameResult::ALL_TANKS_DEAD) {
        GameResult result;
        result.winner = winner;
        result.reason = reason;
        return result;
    }
    
    std::string testDir;
    std::string testFile;
};

TEST_F(OutputGeneratorTest, GenerateTimestamp_BasicFormat) {
    std::string timestamp = OutputGenerator::generateTimestamp(false);
    
    // Should match format: YYYYMMDD_HHMMSS
    std::regex timestampRegex(R"(\d{8}_\d{6})");
    EXPECT_TRUE(std::regex_match(timestamp, timestampRegex));
    EXPECT_EQ(timestamp.length(), 15); // 8 digits + _ + 6 digits
}

TEST_F(OutputGeneratorTest, GenerateTimestamp_WithMilliseconds) {
    std::string timestamp = OutputGenerator::generateTimestamp(true);
    
    // Should match format: YYYYMMDD_HHMMSS_mmm
    std::regex timestampRegex(R"(\d{8}_\d{6}_\d{3})");
    EXPECT_TRUE(std::regex_match(timestamp, timestampRegex));
    EXPECT_EQ(timestamp.length(), 19); // 8 digits + _ + 6 digits + _ + 3 digits
}

TEST_F(OutputGeneratorTest, GenerateTimestamp_Uniqueness) {
    std::string timestamp1 = OutputGenerator::generateTimestamp(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::string timestamp2 = OutputGenerator::generateTimestamp(true);
    
    EXPECT_NE(timestamp1, timestamp2);
}

TEST_F(OutputGeneratorTest, WriteToFile_Success) {
    std::string content = "Test content for file writing";
    bool success = OutputGenerator::writeToFile(testFile, content, false);
    
    EXPECT_TRUE(success);
    EXPECT_TRUE(std::filesystem::exists(testFile));
    EXPECT_EQ(readFile(testFile), content);
    EXPECT_TRUE(OutputGenerator::getLastError().empty());
}

TEST_F(OutputGeneratorTest, WriteToFile_NonexistentDirectory) {
    std::string nestedFile = testDir + "/nested/deep/test.txt";
    std::string content = "Test content";
    bool success = OutputGenerator::writeToFile(nestedFile, content, false);
    
    EXPECT_TRUE(success);
    EXPECT_TRUE(std::filesystem::exists(nestedFile));
    EXPECT_EQ(readFile(nestedFile), content);
}

TEST_F(OutputGeneratorTest, WriteToFile_InvalidPath) {
    // Try to write to a path with invalid characters (depending on OS)
    std::string invalidFile = "/invalid/path/\0test.txt";
    std::string content = "Test content";
    bool success = OutputGenerator::writeToFile(invalidFile, content, false);
    
    EXPECT_FALSE(success);
    EXPECT_FALSE(OutputGenerator::getLastError().empty());
}

TEST_F(OutputGeneratorTest, FormatGameResult_Winner) {
    GameResult result = createTestGameResult(2, GameResult::ALL_TANKS_DEAD);
    std::string formatted = OutputGenerator::formatGameResult(result);
    
    EXPECT_EQ(formatted, "Winner: Player 2 (all tanks dead)");
}

TEST_F(OutputGeneratorTest, FormatGameResult_Tie) {
    GameResult result = createTestGameResult(0, GameResult::MAX_STEPS);
    std::string formatted = OutputGenerator::formatGameResult(result);
    
    EXPECT_EQ(formatted, "Tie (max steps reached)");
}

TEST_F(OutputGeneratorTest, FormatGameResult_ZeroShells) {
    GameResult result = createTestGameResult(1, GameResult::ZERO_SHELLS);
    std::string formatted = OutputGenerator::formatGameResult(result);
    
    EXPECT_EQ(formatted, "Winner: Player 1 (zero shells remaining)");
}

TEST_F(OutputGeneratorTest, FormatGameState_SmallBoard) {
    std::vector<std::string> board = {
        "##",
        "1 ",
        "##"
    };
    MockSatelliteView mockView(board);
    
    std::string formatted = OutputGenerator::formatGameState(mockView, 3, 2);
    std::string expected = "##\n1 \n##";
    
    EXPECT_EQ(formatted, expected);
}

TEST_F(OutputGeneratorTest, FormatGameState_LargerBoard) {
    std::vector<std::string> board = {
        "####",
        "#1 #",
        "# 2#",
        "####"
    };
    MockSatelliteView mockView(board);
    
    std::string formatted = OutputGenerator::formatGameState(mockView, 4, 4);
    std::string expected = "####\n#1 #\n# 2#\n####";
    
    EXPECT_EQ(formatted, expected);
}

TEST_F(OutputGeneratorTest, CreateOutputPath_BasicUsage) {
    std::string path = OutputGenerator::createOutputPath("test_results");
    
    // Should match format: test_results_YYYYMMDD_HHMMSS.txt
    std::regex pathRegex(R"(test_results_\d{8}_\d{6}\.txt)");
    EXPECT_TRUE(std::regex_match(path, pathRegex));
}

TEST_F(OutputGeneratorTest, CreateOutputPath_WithDirectory) {
    std::string path = OutputGenerator::createOutputPath("results", "output", ".json");
    
    // Should match format: output/results_YYYYMMDD_HHMMSS.json
    std::regex pathRegex(R"(output[/\\]results_\d{8}_\d{6}\.json)");
    EXPECT_TRUE(std::regex_match(path, pathRegex));
}

TEST_F(OutputGeneratorTest, CreateOutputPath_WithMilliseconds) {
    std::string path = OutputGenerator::createOutputPath("test", "", ".txt", true);
    
    // Should match format: test_YYYYMMDD_HHMMSS_mmm.txt
    std::regex pathRegex(R"(test_\d{8}_\d{6}_\d{3}\.txt)");
    EXPECT_TRUE(std::regex_match(path, pathRegex));
}

TEST_F(OutputGeneratorTest, CreateOutputPath_EmptyDirectory) {
    std::string path = OutputGenerator::createOutputPath("test", "");
    
    // Should not have directory separator
    EXPECT_EQ(path.find('/'), std::string::npos);
    EXPECT_EQ(path.find('\\'), std::string::npos);
}

TEST_F(OutputGeneratorTest, ErrorHandling_ThreadLocal) {
    // Test that error messages are thread-local
    OutputGenerator::writeToFile("/invalid/path/test.txt", "content", false);
    std::string error1 = OutputGenerator::getLastError();
    
    OutputGenerator::writeToFile("/another/invalid/path/test.txt", "content", false);
    std::string error2 = OutputGenerator::getLastError();
    
    EXPECT_FALSE(error1.empty());
    EXPECT_FALSE(error2.empty());
    // Errors should be different (different paths)
    EXPECT_NE(error1, error2);
}

TEST_F(OutputGeneratorTest, WriteToFile_ClearsPreviousError) {
    // First, cause an error
    OutputGenerator::writeToFile("/invalid/path/test.txt", "content", false);
    EXPECT_FALSE(OutputGenerator::getLastError().empty());
    
    // Then, write successfully
    bool success = OutputGenerator::writeToFile(testFile, "content", false);
    EXPECT_TRUE(success);
    EXPECT_TRUE(OutputGenerator::getLastError().empty());
}

TEST_F(OutputGeneratorTest, EnsureDirectoryExists_DeepNesting) {
    std::string deepFile = testDir + "/level1/level2/level3/deep_file.txt";
    bool success = OutputGenerator::writeToFile(deepFile, "content", false);
    
    EXPECT_TRUE(success);
    EXPECT_TRUE(std::filesystem::exists(deepFile));
    EXPECT_TRUE(std::filesystem::is_directory(testDir + "/level1"));
    EXPECT_TRUE(std::filesystem::is_directory(testDir + "/level1/level2"));
    EXPECT_TRUE(std::filesystem::is_directory(testDir + "/level1/level2/level3"));
}

TEST_F(OutputGeneratorTest, WriteToFile_FilePermissions) {
    // Create a valid file first
    bool success = OutputGenerator::writeToFile(testFile, "initial content", false);
    EXPECT_TRUE(success);
    
    // Overwrite the file
    success = OutputGenerator::writeToFile(testFile, "new content", false);
    EXPECT_TRUE(success);
    EXPECT_EQ(readFile(testFile), "new content");
}