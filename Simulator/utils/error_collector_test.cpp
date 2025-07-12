#include "gtest/gtest.h"
#include "error_collector.h"
#include <fstream>
#include <filesystem>

class ErrorCollectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Remove any existing error file before each test
        if (std::filesystem::exists("input_errors.txt")) {
            std::filesystem::remove("input_errors.txt");
        }
    }

    void TearDown() override {
        // Clean up error file after each test
        if (std::filesystem::exists("input_errors.txt")) {
            std::filesystem::remove("input_errors.txt");
        }
    }
};

TEST_F(ErrorCollectorTest, ConstructorInitializesEmpty) {
    ErrorCollector collector;
    
    EXPECT_FALSE(collector.hasErrors());
    EXPECT_TRUE(collector.getAllErrors().empty());
}

TEST_F(ErrorCollectorTest, AddMapWarningsStoresErrors) {
    ErrorCollector collector;
    
    std::vector<std::string> warnings = {"Row 2 is shorter than expected"};
    collector.addMapWarnings("test.txt", warnings);
    
    EXPECT_TRUE(collector.hasErrors());
    EXPECT_EQ(collector.getAllErrors().size(), 1);
    EXPECT_EQ(collector.getAllErrors()[0], "Map 'test.txt': Row 2 is shorter than expected");
}

TEST_F(ErrorCollectorTest, AddMultipleMapWarnings) {
    ErrorCollector collector;
    
    std::vector<std::string> warnings1 = {"Invalid character at position (2,3)", "Column too short"};
    std::vector<std::string> warnings2 = {"Row count mismatch"};
    
    collector.addMapWarnings("map1.txt", warnings1);
    collector.addMapWarnings("map2.txt", warnings2);
    
    EXPECT_TRUE(collector.hasErrors());
    EXPECT_EQ(collector.getAllErrors().size(), 3);
    
    // Check that warnings are stored in order
    EXPECT_EQ(collector.getAllErrors()[0], "Map 'map1.txt': Invalid character at position (2,3)");
    EXPECT_EQ(collector.getAllErrors()[1], "Map 'map1.txt': Column too short");
    EXPECT_EQ(collector.getAllErrors()[2], "Map 'map2.txt': Row count mismatch");
}

TEST_F(ErrorCollectorTest, SaveToFileWithNoErrors) {
    ErrorCollector collector;
    
    bool result = collector.saveToFile();
    
    EXPECT_TRUE(result);
    EXPECT_FALSE(std::filesystem::exists("input_errors.txt"));
}

TEST_F(ErrorCollectorTest, SaveToFileWithErrors) {
    ErrorCollector collector;
    std::vector<std::string> warnings1 = {"Row 1 has invalid character"};
    std::vector<std::string> warnings2 = {"Dimension mismatch"};
    
    collector.addMapWarnings("test.txt", warnings1);
    collector.addMapWarnings("another.txt", warnings2);
    
    bool result = collector.saveToFile();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(std::filesystem::exists("input_errors.txt"));
    
    // Verify file contents
    std::ifstream file("input_errors.txt");
    ASSERT_TRUE(file.is_open());
    
    std::string line1, line2;
    std::getline(file, line1);
    std::getline(file, line2);
    
    EXPECT_EQ(line1, "Map 'test.txt': Row 1 has invalid character");
    EXPECT_EQ(line2, "Map 'another.txt': Dimension mismatch");
    
    file.close();
}

TEST_F(ErrorCollectorTest, ClearRemovesAllErrors) {
    ErrorCollector collector;
    std::vector<std::string> warnings1 = {"Some warning"};
    std::vector<std::string> warnings2 = {"Another warning"};
    
    collector.addMapWarnings("test.txt", warnings1);
    collector.addMapWarnings("other.txt", warnings2);
    
    EXPECT_TRUE(collector.hasErrors());
    EXPECT_EQ(collector.getAllErrors().size(), 2);
    
    collector.clear();
    
    EXPECT_FALSE(collector.hasErrors());
    EXPECT_TRUE(collector.getAllErrors().empty());
}

TEST_F(ErrorCollectorTest, FormatMapWarningWithSpecialCharacters) {
    ErrorCollector collector;
    
    std::vector<std::string> warnings = {"Warning with 'quotes' and \"double quotes\""};
    collector.addMapWarnings("file with spaces.txt", warnings);
    
    EXPECT_TRUE(collector.hasErrors());
    EXPECT_EQ(collector.getAllErrors()[0], "Map 'file with spaces.txt': Warning with 'quotes' and \"double quotes\"");
}

TEST_F(ErrorCollectorTest, FormatMapWarningWithEmptyStrings) {
    ErrorCollector collector;
    
    std::vector<std::string> warnings1 = {"Empty filename warning"};
    std::vector<std::string> warnings2 = {""};
    
    collector.addMapWarnings("", warnings1);
    collector.addMapWarnings("test.txt", warnings2);
    
    EXPECT_TRUE(collector.hasErrors());
    EXPECT_EQ(collector.getAllErrors().size(), 2);
    EXPECT_EQ(collector.getAllErrors()[0], "Map '': Empty filename warning");
    EXPECT_EQ(collector.getAllErrors()[1], "Map 'test.txt': ");
}

TEST_F(ErrorCollectorTest, SaveToFilePreservesOrder) {
    ErrorCollector collector;
    
    // Add warnings in specific order
    std::vector<std::string> warnings1 = {"First warning"};
    std::vector<std::string> warnings2 = {"Second warning"};
    std::vector<std::string> warnings3 = {"Third warning"};
    
    collector.addMapWarnings("first.txt", warnings1);
    collector.addMapWarnings("second.txt", warnings2);
    collector.addMapWarnings("third.txt", warnings3);
    
    bool result = collector.saveToFile();
    EXPECT_TRUE(result);
    
    // Verify file preserves order
    std::ifstream file("input_errors.txt");
    ASSERT_TRUE(file.is_open());
    
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    
    ASSERT_EQ(lines.size(), 3);
    EXPECT_EQ(lines[0], "Map 'first.txt': First warning");
    EXPECT_EQ(lines[1], "Map 'second.txt': Second warning");
    EXPECT_EQ(lines[2], "Map 'third.txt': Third warning");
}

TEST_F(ErrorCollectorTest, GetAllErrorsReturnsConstReference) {
    ErrorCollector collector;
    std::vector<std::string> warnings = {"Some warning"};
    collector.addMapWarnings("test.txt", warnings);
    
    const std::vector<std::string>& errors = collector.getAllErrors();
    
    // Verify it's the same reference (not a copy)
    EXPECT_EQ(&errors, &collector.getAllErrors());
    EXPECT_EQ(errors.size(), 1);
    EXPECT_EQ(errors[0], "Map 'test.txt': Some warning");
}