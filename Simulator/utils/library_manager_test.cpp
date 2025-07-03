#include "library_manager.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

class LibraryManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean up any previous state
        LibraryManager::getInstance().unloadAllLibraries();
        
        // Create test directory if it doesn't exist
        std::filesystem::create_directories("test_libs");
    }
    
    void TearDown() override {
        // Clean up after each test
        LibraryManager::getInstance().unloadAllLibraries();
        
        // Remove test files
        std::filesystem::remove_all("test_libs");
    }
    
    // Helper to create a minimal valid shared library for testing
    void createTestLibrary(const std::string& filename) {
        // Create a simple C file that can be compiled to .so
        std::string cFile = "test_libs/" + filename + ".c";
        std::ofstream file(cFile);
        file << "void test_function() { /* empty */ }\n";
        file.close();
        
        // Compile to shared library
        std::string soFile = "test_libs/" + filename + ".so";
        std::string command = "gcc -shared -fPIC -o " + soFile + " " + cFile + " 2>/dev/null";
        system(command.c_str());
    }
};

TEST_F(LibraryManagerTest, SingletonPattern) {
    LibraryManager& manager1 = LibraryManager::getInstance();
    LibraryManager& manager2 = LibraryManager::getInstance();
    
    EXPECT_EQ(&manager1, &manager2);
}

TEST_F(LibraryManagerTest, LoadValidLibrary) {
    createTestLibrary("valid_lib");
    
    LibraryManager& manager = LibraryManager::getInstance();
    bool result = manager.loadLibrary("test_libs/valid_lib.so");
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(manager.getLastError().empty());
    
    auto loaded = manager.getLoadedLibraryNames();
    EXPECT_EQ(loaded.size(), 1);
    EXPECT_EQ(loaded[0], "test_libs/valid_lib.so");
}

TEST_F(LibraryManagerTest, LoadNonExistentLibrary) {
    LibraryManager& manager = LibraryManager::getInstance();
    bool result = manager.loadLibrary("non_existent.so");
    
    EXPECT_FALSE(result);
    EXPECT_FALSE(manager.getLastError().empty());
    EXPECT_TRUE(manager.getLoadedLibraryNames().empty());
}

TEST_F(LibraryManagerTest, LoadEmptyPath) {
    LibraryManager& manager = LibraryManager::getInstance();
    bool result = manager.loadLibrary("");
    
    EXPECT_FALSE(result);
    EXPECT_EQ(manager.getLastError(), "Library path cannot be empty");
    EXPECT_TRUE(manager.getLoadedLibraryNames().empty());
}

TEST_F(LibraryManagerTest, LoadDuplicateLibrary) {
    createTestLibrary("duplicate_lib");
    
    LibraryManager& manager = LibraryManager::getInstance();
    
    // Load library first time
    bool result1 = manager.loadLibrary("test_libs/duplicate_lib.so");
    EXPECT_TRUE(result1);
    
    // Try to load same library again
    bool result2 = manager.loadLibrary("test_libs/duplicate_lib.so");
    EXPECT_FALSE(result2);
    EXPECT_EQ(manager.getLastError(), "Library already loaded: test_libs/duplicate_lib.so");
    
    // Should still have only one library loaded
    auto loaded = manager.getLoadedLibraryNames();
    EXPECT_EQ(loaded.size(), 1);
}

TEST_F(LibraryManagerTest, LoadMultipleLibraries) {
    createTestLibrary("lib1");
    createTestLibrary("lib2");
    createTestLibrary("lib3");
    
    LibraryManager& manager = LibraryManager::getInstance();
    
    EXPECT_TRUE(manager.loadLibrary("test_libs/lib1.so"));
    EXPECT_TRUE(manager.loadLibrary("test_libs/lib2.so"));
    EXPECT_TRUE(manager.loadLibrary("test_libs/lib3.so"));
    
    auto loaded = manager.getLoadedLibraryNames();
    EXPECT_EQ(loaded.size(), 3);
    EXPECT_EQ(loaded[0], "test_libs/lib1.so");
    EXPECT_EQ(loaded[1], "test_libs/lib2.so");
    EXPECT_EQ(loaded[2], "test_libs/lib3.so");
}

TEST_F(LibraryManagerTest, UnloadAllLibraries) {
    createTestLibrary("unload_lib1");
    createTestLibrary("unload_lib2");
    
    LibraryManager& manager = LibraryManager::getInstance();
    
    // Load libraries
    EXPECT_TRUE(manager.loadLibrary("test_libs/unload_lib1.so"));
    EXPECT_TRUE(manager.loadLibrary("test_libs/unload_lib2.so"));
    EXPECT_EQ(manager.getLoadedLibraryNames().size(), 2);
    
    // Unload all
    manager.unloadAllLibraries();
    EXPECT_TRUE(manager.getLoadedLibraryNames().empty());
    EXPECT_TRUE(manager.getLastError().empty());
}

TEST_F(LibraryManagerTest, ErrorClearingBetweenOperations) {
    LibraryManager& manager = LibraryManager::getInstance();
    
    // Generate an error
    manager.loadLibrary("non_existent.so");
    EXPECT_FALSE(manager.getLastError().empty());
    
    // Next successful operation should clear error
    createTestLibrary("clear_error_lib");
    EXPECT_TRUE(manager.loadLibrary("test_libs/clear_error_lib.so"));
    EXPECT_TRUE(manager.getLastError().empty());
}

TEST_F(LibraryManagerTest, LoadInvalidSharedLibrary) {
    // Create a file that's not a valid shared library
    std::ofstream file("test_libs/invalid.so");
    file << "This is not a valid shared library\n";
    file.close();
    
    LibraryManager& manager = LibraryManager::getInstance();
    bool result = manager.loadLibrary("test_libs/invalid.so");
    
    EXPECT_FALSE(result);
    EXPECT_FALSE(manager.getLastError().empty());
    EXPECT_TRUE(manager.getLoadedLibraryNames().empty());
}