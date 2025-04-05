#include "bonus/logger.h"
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <cstdio>

// Helper function to read a file into a string
std::string readEntireFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Use a temporary file for testing
        m_testLogFile = "test_log.txt";
        
        // Start with a clean test file
        std::remove(m_testLogFile.c_str());
        
        // Disable console output to avoid noise during tests
        Logger::getInstance().enableConsoleOutput(false);
    }
    
    void TearDown() override {
        // Clean up
        Logger::getInstance().closeLogFile();
        std::remove(m_testLogFile.c_str());
    }
    
    std::string m_testLogFile;
};

// Test that log levels are respected (messages below current level are filtered)
TEST_F(LoggerTest, RespectLogLevels) {
    Logger& logger = Logger::getInstance();
    
    // Set up the logger
    EXPECT_TRUE(logger.setLogFile(m_testLogFile));
    logger.setLogLevel(Logger::LogLevel::WARNING);
    
    // Log messages at different levels
    logger.debug("This is a debug message");
    logger.info("This is an info message");
    logger.warning("This is a warning message");
    logger.error("This is an error message");
    
    // Read the log file
    std::string fileContent = readEntireFile(m_testLogFile);
    
    // Check that only WARNING and ERROR messages are in the log
    EXPECT_EQ(fileContent.find("DEBUG"), std::string::npos);
    EXPECT_EQ(fileContent.find("INFO"), std::string::npos);
    EXPECT_NE(fileContent.find("WARNING"), std::string::npos);
    EXPECT_NE(fileContent.find("ERROR"), std::string::npos);
}

// Test that messages are properly formatted
TEST_F(LoggerTest, MessageFormatting) {
    Logger& logger = Logger::getInstance();
    
    // Set up the logger
    EXPECT_TRUE(logger.setLogFile(m_testLogFile));
    logger.setLogLevel(Logger::LogLevel::INFO);
    
    // Log a test message
    const std::string testMessage = "Test message content";
    logger.info(testMessage);
    
    // Read the log file
    std::string fileContent = readEntireFile(m_testLogFile);
    
    // Check the format: [TIMESTAMP] [LEVEL] Message
    EXPECT_NE(fileContent.find("["), std::string::npos);
    EXPECT_NE(fileContent.find("] [INFO]"), std::string::npos);
    EXPECT_NE(fileContent.find(testMessage), std::string::npos);
}

// Test that the isLevelEnabled function works correctly
TEST_F(LoggerTest, IsLevelEnabled) {
    Logger& logger = Logger::getInstance();
    
    // Set log level to WARNING
    logger.setLogLevel(Logger::LogLevel::WARNING);
    
    // Check which levels are enabled
    EXPECT_FALSE(logger.isLevelEnabled(Logger::LogLevel::DEBUG));
    EXPECT_FALSE(logger.isLevelEnabled(Logger::LogLevel::INFO));
    EXPECT_TRUE(logger.isLevelEnabled(Logger::LogLevel::WARNING));
    EXPECT_TRUE(logger.isLevelEnabled(Logger::LogLevel::ERROR));
    
    // Change log level to DEBUG
    logger.setLogLevel(Logger::LogLevel::DEBUG);
    
    // Check which levels are enabled now
    EXPECT_TRUE(logger.isLevelEnabled(Logger::LogLevel::DEBUG));
    EXPECT_TRUE(logger.isLevelEnabled(Logger::LogLevel::INFO));
    EXPECT_TRUE(logger.isLevelEnabled(Logger::LogLevel::WARNING));
    EXPECT_TRUE(logger.isLevelEnabled(Logger::LogLevel::ERROR));
}

// Test that consecutive writes work
TEST_F(LoggerTest, ConsecutiveWrites) {
    Logger& logger = Logger::getInstance();
    
    // Set up the logger
    EXPECT_TRUE(logger.setLogFile(m_testLogFile));
    logger.setLogLevel(Logger::LogLevel::INFO);
    
    // Log several messages
    logger.info("Message 1");
    logger.info("Message 2");
    logger.info("Message 3");
    
    // Read the log file
    std::string fileContent = readEntireFile(m_testLogFile);
    
    // Check that all messages are in the log
    EXPECT_NE(fileContent.find("Message 1"), std::string::npos);
    EXPECT_NE(fileContent.find("Message 2"), std::string::npos);
    EXPECT_NE(fileContent.find("Message 3"), std::string::npos);
}

// Test file handling when file can't be opened
TEST_F(LoggerTest, FileHandling) {
    Logger& logger = Logger::getInstance();
    
    // Try to open a file in a directory that doesn't exist
    EXPECT_FALSE(logger.setLogFile("/path/that/does/not/exist/test.log"));
    
    // Try to open a valid file
    EXPECT_TRUE(logger.setLogFile(m_testLogFile));
}