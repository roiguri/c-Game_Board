// test/bonus/logger/logger_test.cpp
#include "gtest/gtest.h"
#include "bonus/logger/logger.h"
#include <fstream>
#include <string>
#include <filesystem>

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset logger to default state
        Logger::getInstance().initialize(
            Logger::Level::DEBUG,
            false, // Don't use console to avoid test output pollution
            true,  // Use file
            "logger_test.log"
        );
        Logger::getInstance().setEnabled(true);
        
        // Remove any existing test log file
        if (std::filesystem::exists("logger_test.log")) {
            std::filesystem::remove("logger_test.log");
        }
    }
    
    void TearDown() override {
        // Disable logger
        Logger::getInstance().setEnabled(false);
        
        // Clean up test log file
        if (std::filesystem::exists("logger_test.log")) {
            std::filesystem::remove("logger_test.log");
        }
    }
    
    // Helper to check if a string exists in log file
    bool logFileContains(const std::string& text) {
        std::ifstream logFile("logger_test.log");
        if (!logFile.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(logFile, line)) {
            if (line.find(text) != std::string::npos) {
                return true;
            }
        }
        
        return false;
    }
};

TEST_F(LoggerTest, LoggerInitialization) {
    Logger& logger = Logger::getInstance();
    EXPECT_TRUE(logger.initialize(Logger::Level::INFO, false, true, "logger_test.log"));
    
    // Should create the file
    EXPECT_TRUE(std::filesystem::exists("logger_test.log"));
}

TEST_F(LoggerTest, LogLevels) {
    Logger& logger = Logger::getInstance();
    logger.initialize(Logger::Level::DEBUG, false, true, "logger_test.log");
    
    // All levels should be logged
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warning("Warning message");
    logger.error("Error message");
    
    // Verify all messages were logged
    EXPECT_TRUE(logFileContains("Debug message"));
    EXPECT_TRUE(logFileContains("Info message"));
    EXPECT_TRUE(logFileContains("Warning message"));
    EXPECT_TRUE(logFileContains("Error message"));
    
    // Now set level to WARNING
    logger.setLevel(Logger::Level::WARNING);
    
    // Clear log file
    std::filesystem::remove("logger_test.log");
    logger.initialize(Logger::Level::WARNING, false, true, "logger_test.log");
    
    // Log messages at all levels
    logger.debug("Debug message 2");
    logger.info("Info message 2");
    logger.warning("Warning message 2");
    logger.error("Error message 2");
    
    // Verify only WARNING and above were logged
    EXPECT_FALSE(logFileContains("Debug message 2"));
    EXPECT_FALSE(logFileContains("Info message 2"));
    EXPECT_TRUE(logFileContains("Warning message 2"));
    EXPECT_TRUE(logFileContains("Error message 2"));
}

TEST_F(LoggerTest, LogEnabledDisabled) {
    Logger& logger = Logger::getInstance();
    logger.initialize(Logger::Level::DEBUG, false, true, "logger_test.log");
    
    // Enable logging and check
    logger.setEnabled(true);
    EXPECT_TRUE(logger.isEnabled());
    
    // Log a message
    logger.info("Logging enabled");
    EXPECT_TRUE(logFileContains("Logging enabled"));
    
    // Disable logging and check
    logger.setEnabled(false);
    EXPECT_FALSE(logger.isEnabled());
    
    // Clear log file
    std::filesystem::remove("logger_test.log");
    logger.initialize(Logger::Level::DEBUG, false, true, "logger_test.log");
    
    // Log a message when disabled
    logger.info("This should not be logged");
    EXPECT_FALSE(logFileContains("This should not be logged"));
    
    // Re-enable logging
    logger.setEnabled(true);
    logger.info("Logging enabled again");
    EXPECT_TRUE(logFileContains("Logging enabled again"));
}