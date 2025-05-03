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
};

TEST_F(FileLoaderTest, LoadBoardFile_NonexistentFile) {
    const std::string nonExistentFile = "this_file_does_not_exist.txt";
    int rows, cols, maxSteps, numShells;
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    std::vector<std::string> result = FileLoader::loadBoardFile(nonExistentFile, rows, cols, maxSteps, numShells);
    std::cerr.rdbuf(old_cerr);
    EXPECT_TRUE(result.empty());
    std::string errorOutput = cerr_buffer.str();
    EXPECT_TRUE(errorOutput.find("Could not open file") != std::string::npos);
}

TEST_F(FileLoaderTest, LoadBoardFile_EmptyFile) {
    createTestFile({});
    int rows, cols, maxSteps, numShells;
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells);
    std::cerr.rdbuf(old_cerr);
    EXPECT_TRUE(result.empty());
    std::string errorOutput = cerr_buffer.str();
    EXPECT_TRUE(errorOutput.find("File must have at least 5 header lines") != std::string::npos);
}

TEST_F(FileLoaderTest, LoadBoardFile_ValidFile) {
    std::vector<std::string> content = {
        "MapName",  // map name
        "MaxSteps = 1000",
        "NumShells = 20",
        "Rows = 5",
        "Cols = 5",
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    };
    createTestFile(content);
    int rows, cols, maxSteps, numShells;
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells);
    EXPECT_EQ(rows, 5);
    EXPECT_EQ(cols, 5);
    EXPECT_EQ(maxSteps, 1000);
    EXPECT_EQ(numShells, 20);
    EXPECT_EQ(result.size(), 5);
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_EQ(result[i], content[i+5]); // +5 to skip header
    }
}

TEST_F(FileLoaderTest, LoadBoardFile_InvalidRows) {
    createTestFile({
        "MapName",
        "MaxSteps = 1000",
        "NumShells = 20",
        "Rows = -3",  // Invalid
        "Cols = 5",
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    });
    int rows, cols, maxSteps, numShells;
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells);
    std::cerr.rdbuf(old_cerr);
    EXPECT_TRUE(result.empty());
    std::string errorOutput = cerr_buffer.str();
    EXPECT_TRUE(errorOutput.find("Invalid or missing Rows line") != std::string::npos);
}

TEST_F(FileLoaderTest, LoadBoardFile_MissingHeaderLine) {
    createTestFile({
        "MapName",
        "MaxSteps = 1000",
        "NumShells = 20",
        // Missing Rows line
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    });
    int rows, cols, maxSteps, numShells;
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells);
    std::cerr.rdbuf(old_cerr);
    EXPECT_TRUE(result.empty());
    std::string errorOutput = cerr_buffer.str();
    EXPECT_TRUE(errorOutput.find("Error: Invalid or missing Rows line:") != std::string::npos);
}

TEST_F(FileLoaderTest, LoadBoardFile_ExtraWhitespace) {
    createTestFile({
        "MapName",
        "MaxSteps = 2000  ",
        "NumShells =  15",
        "Rows =  10 ",
        "Cols = 8  ",
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    });
    int rows, cols, maxSteps, numShells;
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells);
    EXPECT_EQ(rows, 10);
    EXPECT_EQ(cols, 8);
    EXPECT_EQ(maxSteps, 2000);
    EXPECT_EQ(numShells, 15);
    EXPECT_FALSE(result.empty());
}

TEST_F(FileLoaderTest, LoadBoardFile_OnlyHeaders) {
    createTestFile({
        "MapName",
        "MaxSteps = 1000",
        "NumShells = 20",
        "Rows = 5",
        "Cols = 5"
    });
    int rows, cols, maxSteps, numShells;
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells);
    EXPECT_EQ(rows, 5);
    EXPECT_EQ(cols, 5);
    EXPECT_EQ(maxSteps, 1000);
    EXPECT_EQ(numShells, 20);
    EXPECT_TRUE(result.empty());
}

TEST_F(FileLoaderTest, LoadBoardFile_TrailingWhitespace) {
    std::vector<std::string> content = {
        "MapName",
        "MaxSteps = 1000 ",
        "NumShells = 20 ",
        "Rows = 5 ",
        "Cols = 5 ",
        "#####  ",
        "#1 2#   ",
        "#   #",
        "# @ #  ",
        "#####"
    };
    createTestFile(content);
    int rows, cols, maxSteps, numShells;
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells);
    EXPECT_EQ(rows, 5);
    EXPECT_EQ(cols, 5);
    EXPECT_EQ(maxSteps, 1000);
    EXPECT_EQ(numShells, 20);
    EXPECT_EQ(result[0], "#####  ");
    EXPECT_EQ(result[1], "#1 2#   ");
}

TEST_F(FileLoaderTest, LoadBoardFile_LeadingWhitespace) {
    std::vector<std::string> content = {
        "MapName",
        "MaxSteps = 1000 ",
        "NumShells = 20 ",
        "Rows = 5 ",
        "Cols = 5 ",
        " ####",
        "     ",
        "#1 2#",
        "# @ #",
        "#####"
    };
    createTestFile(content);
    int rows, cols, maxSteps, numShells;
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells);
    EXPECT_EQ(rows, 5);
    EXPECT_EQ(cols, 5);
    EXPECT_EQ(maxSteps, 1000);
    EXPECT_EQ(numShells, 20);
    EXPECT_EQ(result[0], " ####");
    EXPECT_EQ(result[1], "     ");
}