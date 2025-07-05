#include "gtest/gtest.h"
#include "library_validator.h"
#include <fstream>
#include <filesystem>
#include <cstdio>

class LibraryValidatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = "test_library_dir";
        validSoFile = testDir + "/valid_library.so";
        invalidSoFile = testDir + "/invalid_library.so";
        nonSoFile = testDir + "/not_a_library.txt";
        
        // Create test directory
        std::filesystem::create_directories(testDir);
        
        // Create test files
        createFile(validSoFile, "Valid library content");
        createFile(invalidSoFile, "Invalid content");
        createFile(nonSoFile, "Not a library");
    }
    
    void TearDown() override {
        // Clean up test directory
        std::error_code ec;
        std::filesystem::remove_all(testDir, ec);
        
        // Clean up validator state
        LibraryValidator::cleanup();
    }
    
    void createFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        file << content << std::endl;
        file.close();
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
        if (!content.empty() && content.back() == '\n') {
            content.pop_back();
        }
        return content;
    }
    
    std::string testDir;
    std::string validSoFile;
    std::string invalidSoFile;
    std::string nonSoFile;
};

TEST_F(LibraryValidatorTest, LibraryInfo_DefaultConstructor) {
    LibraryValidator::LibraryInfo info;
    
    EXPECT_TRUE(info.path.empty());
    EXPECT_TRUE(info.name.empty());
    EXPECT_FALSE(info.loaded);
    EXPECT_FALSE(info.validated);
    EXPECT_TRUE(info.error.empty());
    EXPECT_FALSE(info.isReady());
}

TEST_F(LibraryValidatorTest, LibraryInfo_PathConstructor) {
    LibraryValidator::LibraryInfo info("/path/to/library.so");
    
    EXPECT_EQ(info.path, "/path/to/library.so");
    EXPECT_EQ(info.name, "library");
    EXPECT_FALSE(info.loaded);
    EXPECT_FALSE(info.validated);
    EXPECT_TRUE(info.error.empty());
    EXPECT_FALSE(info.isReady());
}

TEST_F(LibraryValidatorTest, LibraryInfo_PathConstructor_ComplexPath) {
    LibraryValidator::LibraryInfo info("/complex/path/with_underscores/my_lib.so");
    
    EXPECT_EQ(info.path, "/complex/path/with_underscores/my_lib.so");
    EXPECT_EQ(info.name, "my_lib");
    EXPECT_FALSE(info.isReady());
}

TEST_F(LibraryValidatorTest, LibraryInfo_PathConstructor_WindowsPath) {
    LibraryValidator::LibraryInfo info("C:\\Windows\\Path\\library.so");
    
    EXPECT_EQ(info.path, "C:\\Windows\\Path\\library.so");
    EXPECT_EQ(info.name, "library");
}

TEST_F(LibraryValidatorTest, LibraryInfo_PathConstructor_NoExtension) {
    LibraryValidator::LibraryInfo info("/path/to/library");
    
    EXPECT_EQ(info.path, "/path/to/library");
    EXPECT_EQ(info.name, "library");
}

TEST_F(LibraryValidatorTest, LibraryInfo_IsReady) {
    LibraryValidator::LibraryInfo info("/path/to/library.so");
    
    // Initially not ready
    EXPECT_FALSE(info.isReady());
    
    // Still not ready with just loaded
    info.loaded = true;
    EXPECT_FALSE(info.isReady());
    
    // Still not ready with error
    info.validated = true;
    info.error = "Some error";
    EXPECT_FALSE(info.isReady());
    
    // Ready when all conditions met
    info.error.clear();
    EXPECT_TRUE(info.isReady());
}

TEST_F(LibraryValidatorTest, IsValidLibraryFile_ValidFile) {
    bool isValid = LibraryValidator::isValidLibraryFile(validSoFile);
    
    EXPECT_TRUE(isValid);
    EXPECT_TRUE(LibraryValidator::getLastError().empty());
}

TEST_F(LibraryValidatorTest, IsValidLibraryFile_NonexistentFile) {
    bool isValid = LibraryValidator::isValidLibraryFile("nonexistent.so");
    
    EXPECT_FALSE(isValid);
    EXPECT_FALSE(LibraryValidator::getLastError().empty());
    EXPECT_TRUE(LibraryValidator::getLastError().find("does not exist") != std::string::npos);
}

TEST_F(LibraryValidatorTest, IsValidLibraryFile_EmptyPath) {
    bool isValid = LibraryValidator::isValidLibraryFile("");
    
    EXPECT_FALSE(isValid);
    EXPECT_FALSE(LibraryValidator::getLastError().empty());
    EXPECT_TRUE(LibraryValidator::getLastError().find("cannot be empty") != std::string::npos);
}

TEST_F(LibraryValidatorTest, IsValidLibraryFile_WrongExtension) {
    bool isValid = LibraryValidator::isValidLibraryFile(nonSoFile);
    
    EXPECT_FALSE(isValid);
    EXPECT_FALSE(LibraryValidator::getLastError().empty());
    EXPECT_TRUE(LibraryValidator::getLastError().find(".so extension") != std::string::npos);
}

TEST_F(LibraryValidatorTest, IsValidLibraryFile_Directory) {
    bool isValid = LibraryValidator::isValidLibraryFile(testDir);
    
    EXPECT_FALSE(isValid);
    EXPECT_FALSE(LibraryValidator::getLastError().empty());
    EXPECT_TRUE(LibraryValidator::getLastError().find("not a regular file") != std::string::npos);
}

TEST_F(LibraryValidatorTest, ValidateAlgorithm_InvalidFile) {
    auto info = LibraryValidator::validateAlgorithm("nonexistent.so");
    
    EXPECT_FALSE(info.loaded);
    EXPECT_FALSE(info.validated);
    EXPECT_FALSE(info.isReady());
    EXPECT_FALSE(info.error.empty());
}

TEST_F(LibraryValidatorTest, ValidateAlgorithm_WrongExtension) {
    auto info = LibraryValidator::validateAlgorithm(nonSoFile);
    
    EXPECT_FALSE(info.loaded);
    EXPECT_FALSE(info.validated);
    EXPECT_FALSE(info.isReady());
    EXPECT_FALSE(info.error.empty());
}

TEST_F(LibraryValidatorTest, ValidateAlgorithm_ValidFileButInvalidLibrary) {
    // This should fail because our test file isn't a real shared library
    auto info = LibraryValidator::validateAlgorithm(validSoFile);
    
    EXPECT_FALSE(info.loaded);
    EXPECT_FALSE(info.validated);
    EXPECT_FALSE(info.isReady());
    EXPECT_FALSE(info.error.empty());
    EXPECT_TRUE(info.error.find("Failed to load") != std::string::npos);
}

TEST_F(LibraryValidatorTest, ValidateGameManager_InvalidFile) {
    auto info = LibraryValidator::validateGameManager("nonexistent.so");
    
    EXPECT_FALSE(info.loaded);
    EXPECT_FALSE(info.validated);
    EXPECT_FALSE(info.isReady());
    EXPECT_FALSE(info.error.empty());
}

TEST_F(LibraryValidatorTest, ValidateGameManager_ValidFileButInvalidLibrary) {
    // This should fail because our test file isn't a real shared library
    auto info = LibraryValidator::validateGameManager(validSoFile);
    
    EXPECT_FALSE(info.loaded);
    EXPECT_FALSE(info.validated);
    EXPECT_FALSE(info.isReady());
    EXPECT_FALSE(info.error.empty());
}


TEST_F(LibraryValidatorTest, ErrorHandling_ThreadLocal) {
    // Test that error messages are thread-local and properly managed
    LibraryValidator::isValidLibraryFile("nonexistent1.so");
    std::string error1 = LibraryValidator::getLastError();
    
    LibraryValidator::isValidLibraryFile("nonexistent2.so");
    std::string error2 = LibraryValidator::getLastError();
    
    EXPECT_FALSE(error1.empty());
    EXPECT_FALSE(error2.empty());
    // Errors should be different (different filenames)
    EXPECT_NE(error1, error2);
}

TEST_F(LibraryValidatorTest, ErrorHandling_ClearOnSuccess) {
    // First, cause an error
    LibraryValidator::isValidLibraryFile("nonexistent.so");
    EXPECT_FALSE(LibraryValidator::getLastError().empty());
    
    // Then, succeed
    LibraryValidator::isValidLibraryFile(validSoFile);
    EXPECT_TRUE(LibraryValidator::getLastError().empty());
}

TEST_F(LibraryValidatorTest, Cleanup_NoErrors) {
    // Cleanup should not throw or cause errors even when nothing to clean
    EXPECT_NO_THROW(LibraryValidator::cleanup());
    EXPECT_TRUE(LibraryValidator::getLastError().empty());
}

TEST_F(LibraryValidatorTest, LibraryInfo_NameExtraction_EdgeCases) {
    // Test various edge cases for name extraction
    LibraryValidator::LibraryInfo info1("library.so");
    EXPECT_EQ(info1.name, "library");
    
    LibraryValidator::LibraryInfo info2("./library.so");
    EXPECT_EQ(info2.name, "library");
    
    LibraryValidator::LibraryInfo info3("/");
    EXPECT_EQ(info3.name, "");
    
    LibraryValidator::LibraryInfo info4("lib.with.dots.so");
    EXPECT_EQ(info4.name, "lib.with.dots");
}