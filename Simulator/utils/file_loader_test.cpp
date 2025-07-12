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
    size_t rows, cols, maxSteps, numShells;
    std::string mapName;
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    std::vector<std::string> result = FileLoader::loadBoardFile(nonExistentFile, rows, cols, maxSteps, numShells, mapName);
    std::cerr.rdbuf(old_cerr);
    EXPECT_TRUE(result.empty());
    std::string errorOutput = cerr_buffer.str();
    EXPECT_TRUE(errorOutput.find("Could not open file") != std::string::npos);
}

TEST_F(FileLoaderTest, LoadBoardFile_EmptyFile) {
    createTestFile({});
    size_t rows, cols, maxSteps, numShells;
    std::string mapName;
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells, mapName);
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
    size_t rows, cols, maxSteps, numShells;
    std::string mapName;
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells, mapName);
    EXPECT_EQ(rows, 5);
    EXPECT_EQ(cols, 5);
    EXPECT_EQ(maxSteps, 1000);
    EXPECT_EQ(numShells, 20);
    EXPECT_EQ(result.size(), 5u);
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
    size_t rows, cols, maxSteps, numShells;
    std::string mapName;
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells, mapName);
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
    size_t rows, cols, maxSteps, numShells;
    std::string mapName;
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells, mapName);
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
    size_t rows, cols, maxSteps, numShells;
    std::string mapName;
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells, mapName);
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
    size_t rows, cols, maxSteps, numShells;
    std::string mapName;
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells, mapName);
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
    size_t rows, cols, maxSteps, numShells;
    std::string mapName;
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells, mapName);
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
    size_t rows, cols, maxSteps, numShells;
    std::string mapName;
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells, mapName);
    EXPECT_EQ(rows, 5);
    EXPECT_EQ(cols, 5);
    EXPECT_EQ(maxSteps, 1000);
    EXPECT_EQ(numShells, 20);
    EXPECT_EQ(result[0], " ####");
    EXPECT_EQ(result[1], "     ");
}

TEST_F(FileLoaderTest, LoadBoardFile_NegativeMaxSteps) {
    createTestFile({
        "MapName",
        "MaxSteps = -500",  // Invalid negative
        "NumShells = 20",
        "Rows = 5",
        "Cols = 5",
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    });
    size_t rows, cols, maxSteps, numShells;
    std::string mapName;
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells, mapName);
    std::cerr.rdbuf(old_cerr);
    EXPECT_TRUE(result.empty());
    std::string errorOutput = cerr_buffer.str();
    EXPECT_TRUE(errorOutput.find("Invalid or missing MaxSteps line") != std::string::npos);
}

TEST_F(FileLoaderTest, LoadBoardFile_NegativeNumShells) {
    createTestFile({
        "MapName",
        "MaxSteps = 1000",
        "NumShells = -10",  // Invalid negative
        "Rows = 5",
        "Cols = 5",
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    });
    size_t rows, cols, maxSteps, numShells;
    std::string mapName;
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells, mapName);
    std::cerr.rdbuf(old_cerr);
    EXPECT_TRUE(result.empty());
    std::string errorOutput = cerr_buffer.str();
    EXPECT_TRUE(errorOutput.find("Invalid or missing NumShells line") != std::string::npos);
}

TEST_F(FileLoaderTest, LoadBoardFile_NegativeCols) {
    createTestFile({
        "MapName",
        "MaxSteps = 1000",
        "NumShells = 20",
        "Rows = 5",
        "Cols = -5",  // Invalid negative
        "#####",
        "#1 2#",
        "#   #",
        "# @ #",
        "#####"
    });
    size_t rows, cols, maxSteps, numShells;
    std::string mapName;
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    std::vector<std::string> result = FileLoader::loadBoardFile(tempFileName, rows, cols, maxSteps, numShells, mapName);
    std::cerr.rdbuf(old_cerr);
    EXPECT_TRUE(result.empty());
    std::string errorOutput = cerr_buffer.str();
    EXPECT_TRUE(errorOutput.find("Invalid or missing Cols line") != std::string::npos);
}

// Tests for loadBoardWithSatelliteView method
TEST_F(FileLoaderTest, LoadBoardWithSatelliteView_ValidFile) {
    std::vector<std::string> content = {
        "MapName",
        "MaxSteps = 1000",
        "NumShells = 20",
        "Rows = 3",
        "Cols = 3",
        "###",
        "#1#",
        "###"
    };
    createTestFile(content);
    
    FileLoader::BoardInfo boardInfo = FileLoader::loadBoardWithSatelliteView(tempFileName);
    
    // Verify parsed values
    EXPECT_EQ(boardInfo.rows, 3);
    EXPECT_EQ(boardInfo.cols, 3);
    EXPECT_EQ(boardInfo.maxSteps, 1000);
    EXPECT_EQ(boardInfo.numShells, 20);
    
    // Verify SatelliteView was created
    EXPECT_NE(boardInfo.satelliteView, nullptr);
    
    // Test SatelliteView functionality
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(0, 0), '#');
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(1, 1), '1');  // Now has tank
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(2, 2), '#');
}

TEST_F(FileLoaderTest, LoadBoardWithSatelliteView_NonexistentFile) {
    FileLoader::BoardInfo boardInfo = FileLoader::loadBoardWithSatelliteView("nonexistent_file.txt");
    
    // Should have default/zero values
    EXPECT_EQ(boardInfo.rows, 0);
    EXPECT_EQ(boardInfo.cols, 0);
    EXPECT_EQ(boardInfo.maxSteps, 0);
    EXPECT_EQ(boardInfo.numShells, 0);
    
    // SatelliteView should be null/empty
    EXPECT_EQ(boardInfo.satelliteView, nullptr);
}

TEST_F(FileLoaderTest, LoadBoardWithSatelliteView_EmptyFile) {
    std::vector<std::string> content = {};
    createTestFile(content);
    
    FileLoader::BoardInfo boardInfo = FileLoader::loadBoardWithSatelliteView(tempFileName);
    
    // Should have default/zero values
    EXPECT_EQ(boardInfo.rows, 0);
    EXPECT_EQ(boardInfo.cols, 0);
    EXPECT_EQ(boardInfo.maxSteps, 0);
    EXPECT_EQ(boardInfo.numShells, 0);
    
    // SatelliteView should be null/empty
    EXPECT_EQ(boardInfo.satelliteView, nullptr);
}

TEST_F(FileLoaderTest, LoadBoardWithSatelliteView_ComplexBoard) {
    std::vector<std::string> content = {
        "TestMap",
        "MaxSteps = 500",
        "NumShells = 10", 
        "Rows = 4",
        "Cols = 5",
        "#####",
        "#1 2#",
        "# @ #",
        "#####"
    };
    createTestFile(content);
    
    FileLoader::BoardInfo boardInfo = FileLoader::loadBoardWithSatelliteView(tempFileName);
    
    // Verify parsed values
    EXPECT_EQ(boardInfo.rows, 4);
    EXPECT_EQ(boardInfo.cols, 5);
    EXPECT_EQ(boardInfo.maxSteps, 500);
    EXPECT_EQ(boardInfo.numShells, 10);
    
    // Verify SatelliteView was created and works correctly
    ASSERT_NE(boardInfo.satelliteView, nullptr);
    
    // Test specific board positions
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(0, 0), '#');  // Top-left corner
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(4, 0), '#');  // Top-right corner  
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(1, 1), '1');  // Player 1 position
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(3, 1), '2');  // Player 2 position
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(2, 1), ' ');  // Empty space
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(2, 2), '@');  // Mine position
    
    // Test out of bounds access
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(5, 2), '&');  // x out of bounds
    EXPECT_EQ(boardInfo.satelliteView->getObjectAt(2, 4), '&');  // y out of bounds
}

// Tests for BoardInfo validation interface
TEST_F(FileLoaderTest, BoardInfoValidation_ValidBoard) {
    std::vector<std::string> content = {
        "MapName",
        "MaxSteps = 1000", 
        "NumShells = 20",
        "Rows = 3",
        "Cols = 3",
        "###",
        "#1#",
        "###"
    };
    createTestFile(content);
    
    FileLoader::BoardInfo boardInfo = FileLoader::loadBoardWithSatelliteView(tempFileName);
    
    // Valid board should pass validation
    EXPECT_TRUE(boardInfo.isValid());
    EXPECT_EQ(boardInfo.getErrorReason(), "");
    EXPECT_TRUE(boardInfo.getWarnings().empty());
}

TEST_F(FileLoaderTest, BoardInfoValidation_InvalidBoard) {
    std::vector<std::string> content = {
        "MapName",
        "MaxSteps = 1000",
        "NumShells = 20", 
        "Rows = 3",
        "Cols = 3",
        "###",
        "#@#",  // No tanks
        "###"
    };
    createTestFile(content);
    
    FileLoader::BoardInfo boardInfo = FileLoader::loadBoardWithSatelliteView(tempFileName);
    
    // Invalid board (no tanks) should fail validation
    EXPECT_FALSE(boardInfo.isValid());
    EXPECT_TRUE(boardInfo.getErrorReason().find("tank") != std::string::npos);
    EXPECT_TRUE(boardInfo.getWarnings().empty());
}

TEST_F(FileLoaderTest, BoardInfoValidation_BoardWithWarnings) {
    std::vector<std::string> content = {
        "MapName",
        "MaxSteps = 1000",
        "NumShells = 20",
        "Rows = 3", 
        "Cols = 3",
        "#1&",  // Invalid character '&'
        "###",
        "###"
    };
    createTestFile(content);
    
    FileLoader::BoardInfo boardInfo = FileLoader::loadBoardWithSatelliteView(tempFileName);
    
    // Board should be valid but have warnings
    EXPECT_TRUE(boardInfo.isValid());
    EXPECT_EQ(boardInfo.getErrorReason(), "");
    EXPECT_FALSE(boardInfo.getWarnings().empty());
    EXPECT_TRUE(boardInfo.getWarnings()[0].find("Invalid character") != std::string::npos);
}

TEST_F(FileLoaderTest, BoardInfoValidation_NullSatelliteView) {
    // Test the case where satelliteView is null (file load failed)
    FileLoader::BoardInfo boardInfo = FileLoader::loadBoardWithSatelliteView("nonexistent_file.txt");
    
    // Should return appropriate fallback values
    EXPECT_FALSE(boardInfo.isValid());
    EXPECT_EQ(boardInfo.getErrorReason(), "Failed to load board file");
    EXPECT_TRUE(boardInfo.getWarnings().empty());
}