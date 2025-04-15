#include "gtest/gtest.h"
#include "file_loader.h"
#include <fstream>
#include <cstdio>

class FileLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempFileName = "temp_test_board.txt";
    }
    
    void TearDown() override {
        std::remove(tempFileName.c_str());
    }
    
    void createTestFile(const std::vector<std::string>& lines) {
        std::ofstream outFile(tempFileName);
        for (const auto& line : lines) {
            outFile << line << std::endl;
        }
        outFile.close();
    }
    
    std::string tempFileName;
    int width;
    int height;
};

TEST_F(FileLoaderTest, LoadBoardFile_NonexistentFile) {
    const std::string nonExistentFile = "this_file_does_not_exist.txt";
    
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    
    std::vector<std::string> result = FileLoader::loadBoardFile(nonExistentFile, width, height);
    
    std::cerr.rdbuf(old_cerr);
    
    EXPECT_TRUE(result.empty());
    
    std::string errorOutput = cerr_buffer.str();
    EXPECT_TRUE(errorOutput.find("Could not open file") != std::string::npos);
}

TEST_F(FileLoaderTest, LoadBoardFile_EmptyFile) {
    createTestFile({});
    
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, width, height);
    
    std::cerr.rdbuf(old_cerr);
    
    EXPECT_TRUE(result.empty());
    
    std::string errorOutput = cerr_buffer.str();
    EXPECT_TRUE(errorOutput.find("File is empty") != std::string::npos);
}

TEST_F(FileLoaderTest, LoadBoardFile_ValidFile) {
    std::vector<std::string> content = {
        "5 5",  // 5x5 board
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    };
    createTestFile(content);
    
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, width, height);
    
    EXPECT_EQ(width, 5);
    EXPECT_EQ(height, 5);
    EXPECT_EQ(result.size(), 5);
    
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_EQ(result[i], content[i+1]); // +1 to skip dimensions line
    }
}

TEST_F(FileLoaderTest, LoadBoardFile_InvalidDimensions) {
    createTestFile({
        "-5 -3",  // Negative dimensions
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    });
    
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, width, height);
    
    std::cerr.rdbuf(old_cerr);
    
    EXPECT_TRUE(result.empty());
    
    std::string errorOutput = cerr_buffer.str();
    EXPECT_TRUE(errorOutput.find("Invalid board dimensions") != std::string::npos);
}

TEST_F(FileLoaderTest, LoadBoardFile_MissingSizeLine) {
    createTestFile({
        "This is not a valid dimension line",
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    });
    
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, width, height);
    
    std::cerr.rdbuf(old_cerr);
    
    EXPECT_TRUE(result.empty());
    
    std::string errorOutput = cerr_buffer.str();
    EXPECT_TRUE(errorOutput.find("valid board dimensions") != std::string::npos);
}

TEST_F(FileLoaderTest, LoadBoardFile_ExtraWhitespace) {
    createTestFile({
        "  10    8  ",
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    });
    
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, width, height);
    
    EXPECT_EQ(width, 10);
    EXPECT_EQ(height, 8);
    EXPECT_FALSE(result.empty());
}

TEST_F(FileLoaderTest, LoadBoardFile_OnlyDimensionsLine) {
    createTestFile({
        "5 5"
    });
    
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, width, height);
    
    EXPECT_EQ(width, 5);
    EXPECT_EQ(height, 5);
    EXPECT_TRUE(result.empty());
}

TEST_F(FileLoaderTest, LoadBoardFile_TrailingWhitespace) {
    std::vector<std::string> content = {
        "5 5  ",
        "#####  ",
        "#1 2#   ",
        "#   #",
        "# @ #  ",
        "#####"
    };
    createTestFile(content);
    
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, width, height);
    
    EXPECT_EQ(width, 5);
    EXPECT_EQ(height, 5);
    
    EXPECT_EQ(result[0], "#####  ");
    EXPECT_EQ(result[1], "#1 2#   ");
}

TEST_F(FileLoaderTest, LoadBoardFile_LeadingWhitespace) {
  std::vector<std::string> content = {
      "5 5  ",
      " ####",
      "     ",
      "#1 2#",
      "# @ #",
      "#####"
  };
  createTestFile(content);
  
  std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, width, height);
  
  EXPECT_EQ(width, 5);
  EXPECT_EQ(height, 5);
  
  EXPECT_EQ(result[0], " ####");
  EXPECT_EQ(result[1], "     ");
}