#include "gtest/gtest.h"
#include "Simulator/file_satellite_view.h"

#include <vector>
#include <string>

class FileSatelliteViewTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a simple 3x3 board
        boardData = {
            "###",
            "# #", 
            "###"
        };
        rows = 3;
        cols = 3;
    }

    std::vector<std::string> boardData;
    size_t rows;
    size_t cols;
};

TEST_F(FileSatelliteViewTest, BasicConstruction) {
    FileSatelliteView satelliteView(boardData, rows, cols);
    
    // Test corner positions
    EXPECT_EQ(satelliteView.getObjectAt(0, 0), '#');
    EXPECT_EQ(satelliteView.getObjectAt(2, 0), '#');
    EXPECT_EQ(satelliteView.getObjectAt(0, 2), '#');
    EXPECT_EQ(satelliteView.getObjectAt(2, 2), '#');
    
    // Test center position
    EXPECT_EQ(satelliteView.getObjectAt(1, 1), ' ');
}

TEST_F(FileSatelliteViewTest, OutOfBoundsAccess) {
    FileSatelliteView satelliteView(boardData, rows, cols);
    
    // Test accessing outside board boundaries - should return '&'
    EXPECT_EQ(satelliteView.getObjectAt(3, 1), '&');  // x out of bounds
    EXPECT_EQ(satelliteView.getObjectAt(1, 3), '&');  // y out of bounds
    EXPECT_EQ(satelliteView.getObjectAt(5, 5), '&');  // both out of bounds
}

TEST_F(FileSatelliteViewTest, EmptyBoard) {
    std::vector<std::string> emptyBoard;
    FileSatelliteView satelliteView(emptyBoard, 0, 0);
    
    // Any access to empty board should return '&' (out of bounds)
    EXPECT_EQ(satelliteView.getObjectAt(0, 0), '&');
    EXPECT_EQ(satelliteView.getObjectAt(1, 1), '&');
}

TEST_F(FileSatelliteViewTest, VariableLengthRows) {
    std::vector<std::string> variableBoard = {
        "##",
        "#",
        "###"
    };
    FileSatelliteView satelliteView(variableBoard, 3, 3);
    
    // Test accessing within shorter row
    EXPECT_EQ(satelliteView.getObjectAt(0, 1), '#');
    
    // Test accessing beyond shorter row length - should return ' ' (within board dimensions)
    EXPECT_EQ(satelliteView.getObjectAt(1, 1), ' ');  // Row 1 only has 1 character, but within board
    EXPECT_EQ(satelliteView.getObjectAt(2, 1), ' ');  // Row 1 only has 1 character, but within board
    
    // Test accessing within longer row
    EXPECT_EQ(satelliteView.getObjectAt(2, 2), '#');
}

TEST_F(FileSatelliteViewTest, InvalidCharacterHandling) {
    std::vector<std::string> boardWithInvalidChars = {
        "#1@",
        "A*B",  // A and B are invalid characters
        " 2#"
    };
    FileSatelliteView satelliteView(boardWithInvalidChars, 3, 3);
    
    // Valid characters should be returned as-is
    EXPECT_EQ(satelliteView.getObjectAt(0, 0), '#');  // Wall
    EXPECT_EQ(satelliteView.getObjectAt(1, 0), '1');  // Tank player 1
    EXPECT_EQ(satelliteView.getObjectAt(2, 0), '@');  // Mine
    EXPECT_EQ(satelliteView.getObjectAt(1, 1), '*');  // Shell
    EXPECT_EQ(satelliteView.getObjectAt(0, 2), ' ');  // Empty space
    EXPECT_EQ(satelliteView.getObjectAt(1, 2), '2');  // Tank player 2
    EXPECT_EQ(satelliteView.getObjectAt(2, 2), '#');  // Wall
    
    // Invalid characters should be converted to empty space
    EXPECT_EQ(satelliteView.getObjectAt(0, 1), ' ');  // 'A' -> ' '
    EXPECT_EQ(satelliteView.getObjectAt(2, 1), ' ');  // 'B' -> ' '
}

TEST_F(FileSatelliteViewTest, AllValidCharacters) {
    std::vector<std::string> boardWithValidChars = {
        "#@*",
        "123",
        "456",
        "789",
        " %&"
    };
    FileSatelliteView satelliteView(boardWithValidChars, 5, 3);
    
    // Test all valid characters are preserved
    EXPECT_EQ(satelliteView.getObjectAt(0, 0), '#');  // Wall
    EXPECT_EQ(satelliteView.getObjectAt(1, 0), '@');  // Mine
    EXPECT_EQ(satelliteView.getObjectAt(2, 0), '*');  // Shell
    EXPECT_EQ(satelliteView.getObjectAt(0, 1), '1');  // Tank 1
    EXPECT_EQ(satelliteView.getObjectAt(1, 1), '2');  // Tank 2
    EXPECT_EQ(satelliteView.getObjectAt(2, 1), '3');  // Tank 3
    EXPECT_EQ(satelliteView.getObjectAt(0, 2), '4');  // Tank 4
    EXPECT_EQ(satelliteView.getObjectAt(1, 2), '5');  // Tank 5
    EXPECT_EQ(satelliteView.getObjectAt(2, 2), '6');  // Tank 6
    EXPECT_EQ(satelliteView.getObjectAt(0, 3), '7');  // Tank 7
    EXPECT_EQ(satelliteView.getObjectAt(1, 3), '8');  // Tank 8
    EXPECT_EQ(satelliteView.getObjectAt(2, 3), '9');  // Tank 9
    EXPECT_EQ(satelliteView.getObjectAt(0, 4), ' ');  // Empty
    EXPECT_EQ(satelliteView.getObjectAt(1, 4), '%');  // Current tank
    EXPECT_EQ(satelliteView.getObjectAt(2, 4), ' ');  // '&' inside board -> converted to ' '
}

TEST_F(FileSatelliteViewTest, OutOfBoundsCharacterHandling) {
    std::vector<std::string> boardWithOutOfBoundsChar = {
        "#1&",  // '&' inside board should become ' '
        "2*3"
    };
    FileSatelliteView satelliteView(boardWithOutOfBoundsChar, 2, 3);
    
    // Valid characters preserved
    EXPECT_EQ(satelliteView.getObjectAt(0, 0), '#');
    EXPECT_EQ(satelliteView.getObjectAt(1, 0), '1');
    EXPECT_EQ(satelliteView.getObjectAt(0, 1), '2');
    EXPECT_EQ(satelliteView.getObjectAt(1, 1), '*');
    EXPECT_EQ(satelliteView.getObjectAt(2, 1), '3');
    
    // '&' inside board converted to ' '
    EXPECT_EQ(satelliteView.getObjectAt(2, 0), ' ');
    
    // Out of bounds returns '&'
    EXPECT_EQ(satelliteView.getObjectAt(3, 0), '&');  // x beyond board width (3)
    EXPECT_EQ(satelliteView.getObjectAt(0, 2), '&');  // y beyond board height (2)
}

TEST_F(FileSatelliteViewTest, BoardDimensionsVsStringLength) {
    // Board is 4x3 but strings are shorter
    std::vector<std::string> shortStrings = {
        "#",     // length 1, but board width is 4
        "12"     // length 2, but board width is 4
        // Missing third row entirely
    };
    FileSatelliteView satelliteView(shortStrings, 3, 4);  // 3 rows, 4 cols
    
    // Within board dimensions but beyond string length - should return ' '
    EXPECT_EQ(satelliteView.getObjectAt(0, 0), '#');  // Valid character
    EXPECT_EQ(satelliteView.getObjectAt(1, 0), ' ');  // Beyond string length, within board
    EXPECT_EQ(satelliteView.getObjectAt(2, 0), ' ');  // Beyond string length, within board
    EXPECT_EQ(satelliteView.getObjectAt(3, 0), ' ');  // Beyond string length, within board
    
    EXPECT_EQ(satelliteView.getObjectAt(0, 1), '1');  // Valid character
    EXPECT_EQ(satelliteView.getObjectAt(1, 1), '2');  // Valid character
    EXPECT_EQ(satelliteView.getObjectAt(2, 1), ' ');  // Beyond string length, within board
    EXPECT_EQ(satelliteView.getObjectAt(3, 1), ' ');  // Beyond string length, within board
    
    // Missing row entirely - should return ' ' (within board dimensions)
    EXPECT_EQ(satelliteView.getObjectAt(0, 2), ' ');  // Missing row, within board
    EXPECT_EQ(satelliteView.getObjectAt(1, 2), ' ');  // Missing row, within board
    
    // Truly out of bounds - beyond board dimensions
    EXPECT_EQ(satelliteView.getObjectAt(4, 0), '&');  // x beyond board width
    EXPECT_EQ(satelliteView.getObjectAt(0, 3), '&');  // y beyond board height
}