#include "gtest/gtest.h"
#include "file_enumerator.h"
#include <fstream>
#include <filesystem>
#include <cstdio>

class FileEnumeratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = "test_enum_dir";
        nestedDir = testDir + "/nested";
        
        // Create test directory structure
        std::filesystem::create_directories(testDir);
        std::filesystem::create_directories(nestedDir);
        
        // Create test files with different extensions
        createFile(testDir + "/lib1.so");
        createFile(testDir + "/lib2.so");
        createFile(testDir + "/map1.txt");
        createFile(testDir + "/map2.dat");
        createFile(testDir + "/readme.md");
        createFile(nestedDir + "/nested_lib.so");
    }
    
    void TearDown() override {
        // Clean up test directory
        std::error_code ec;
        std::filesystem::remove_all(testDir, ec);
    }
    
    void createFile(const std::string& path) {
        std::ofstream file(path);
        file << "test content" << std::endl;
        file.close();
    }
    
    std::string testDir;
    std::string nestedDir;
};

TEST_F(FileEnumeratorTest, EnumerateSoFiles_ValidDirectory) {
    auto soFiles = FileEnumerator::enumerateSoFiles(testDir);
    
    EXPECT_EQ(soFiles.size(), 2);
    EXPECT_TRUE(std::find(soFiles.begin(), soFiles.end(), testDir + "/lib1.so") != soFiles.end());
    EXPECT_TRUE(std::find(soFiles.begin(), soFiles.end(), testDir + "/lib2.so") != soFiles.end());
    
    // Check sorting
    EXPECT_TRUE(std::is_sorted(soFiles.begin(), soFiles.end()));
}

TEST_F(FileEnumeratorTest, EnumerateSoFiles_NonexistentDirectory) {
    auto soFiles = FileEnumerator::enumerateSoFiles("nonexistent_directory");
    
    EXPECT_TRUE(soFiles.empty());
    EXPECT_FALSE(FileEnumerator::getLastError().empty());
}

TEST_F(FileEnumeratorTest, EnumerateSoFiles_EmptyDirectory) {
    std::string emptyDir = "empty_test_dir";
    std::filesystem::create_directory(emptyDir);
    
    auto soFiles = FileEnumerator::enumerateSoFiles(emptyDir);
    
    EXPECT_TRUE(soFiles.empty());
    EXPECT_TRUE(FileEnumerator::getLastError().empty());
    
    std::filesystem::remove(emptyDir);
}

TEST_F(FileEnumeratorTest, EnumerateMapFiles_ValidDirectory) {
    auto mapFiles = FileEnumerator::enumerateMapFiles(testDir);
    
    EXPECT_EQ(mapFiles.size(), 1); // Only .txt files: map1.txt
    
    // Should include only .txt files
    bool hasMap1 = std::find(mapFiles.begin(), mapFiles.end(), testDir + "/map1.txt") != mapFiles.end();
    bool hasLib1 = std::find(mapFiles.begin(), mapFiles.end(), testDir + "/lib1.so") != mapFiles.end();
    bool hasMap2 = std::find(mapFiles.begin(), mapFiles.end(), testDir + "/map2.dat") != mapFiles.end();
    bool hasReadme = std::find(mapFiles.begin(), mapFiles.end(), testDir + "/readme.md") != mapFiles.end();
    
    EXPECT_TRUE(hasMap1);
    EXPECT_FALSE(hasLib1);  // .so files should not be included
    EXPECT_FALSE(hasMap2);  // .dat files should not be included
    EXPECT_FALSE(hasReadme); // .md files should not be included
    
    // Check sorting
    EXPECT_TRUE(std::is_sorted(mapFiles.begin(), mapFiles.end()));
}

TEST_F(FileEnumeratorTest, IsValidDirectory_ValidDirectory) {
    EXPECT_TRUE(FileEnumerator::isValidDirectory(testDir));
    EXPECT_TRUE(FileEnumerator::getLastError().empty());
}

TEST_F(FileEnumeratorTest, IsValidDirectory_NonexistentDirectory) {
    EXPECT_FALSE(FileEnumerator::isValidDirectory("nonexistent_directory"));
    EXPECT_FALSE(FileEnumerator::getLastError().empty());
}

TEST_F(FileEnumeratorTest, IsValidDirectory_EmptyPath) {
    EXPECT_FALSE(FileEnumerator::isValidDirectory(""));
    EXPECT_FALSE(FileEnumerator::getLastError().empty());
}

TEST_F(FileEnumeratorTest, IsValidDirectory_FileAsDirectory) {
    std::string testFile = testDir + "/lib1.so";
    EXPECT_FALSE(FileEnumerator::isValidDirectory(testFile));
    EXPECT_FALSE(FileEnumerator::getLastError().empty());
}

TEST_F(FileEnumeratorTest, ErrorHandling_ThreadLocal) {
    // Test that error messages are thread-local
    FileEnumerator::isValidDirectory("nonexistent1");
    std::string error1 = FileEnumerator::getLastError();
    
    FileEnumerator::isValidDirectory("nonexistent2");
    std::string error2 = FileEnumerator::getLastError();
    
    EXPECT_FALSE(error1.empty());
    EXPECT_FALSE(error2.empty());
    EXPECT_NE(error1, error2);
}

TEST_F(FileEnumeratorTest, EnumerateFiles_DoesNotIncludeNestedDirectories) {
    // Ensure that enumeration is not recursive
    auto soFiles = FileEnumerator::enumerateSoFiles(testDir);
    
    // Should not include nested/nested_lib.so
    bool hasNestedLib = std::find(soFiles.begin(), soFiles.end(), nestedDir + "/nested_lib.so") != soFiles.end();
    EXPECT_FALSE(hasNestedLib);
    
    // But should include files directly in testDir
    EXPECT_EQ(soFiles.size(), 2);
}