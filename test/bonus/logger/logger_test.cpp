// test/bonus/logger/logger_test.cpp
#include "gtest/gtest.h"
#include "bonus/logger/logger.h"
#include "bonus/logger/logger_config.h"
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
        
        // Clean up invalid directory test file if it exists
        std::string invalidPath = "/invalid_dir/test.log";
        if (std::filesystem::exists(invalidPath)) {
            std::filesystem::remove(invalidPath);
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

TEST_F(LoggerTest, InitializationFailure) {
    Logger& logger = Logger::getInstance();
    
    // Try to initialize with an invalid file path
    EXPECT_FALSE(logger.initialize(Logger::Level::INFO, false, true, "/invalid_dir/test.log"));
    
    // Should still be able to initialize with console output
    EXPECT_TRUE(logger.initialize(Logger::Level::INFO, true, false));
}

TEST_F(LoggerTest, LogLevels) {
    Logger& logger = Logger::getInstance();
    logger.initialize(Logger::Level::DEBUG, false, true, "logger_test.log");
    logger.setEnabled(true);
    
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
    logger.setEnabled(true);
    
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
    EXPECT_TRUE(logger.isInitializedAndEnabled());
    
    // Log a message
    logger.info("Logging enabled");
    EXPECT_TRUE(logFileContains("Logging enabled"));
    
    // Disable logging and check
    logger.setEnabled(false);
    EXPECT_FALSE(logger.isEnabled());
    EXPECT_FALSE(logger.isInitializedAndEnabled());
    
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

TEST_F(LoggerTest, MacroSafety) {
    Logger& logger = Logger::getInstance();
    
    // Make sure logger is not initialized or enabled
    logger.setEnabled(false);
    
    // These should do nothing and not crash
    LOG_DEBUG("Debug through macro");
    LOG_INFO("Info through macro");
    LOG_WARNING("Warning through macro");
    LOG_ERROR("Error through macro");
    
    // Initialize but don't enable
    logger.initialize(Logger::Level::DEBUG, false, true, "logger_test.log");
    
    // These should still do nothing
    LOG_DEBUG("Debug after init");
    LOG_INFO("Info after init");
    
    // Verify no logs were written
    EXPECT_FALSE(logFileContains("Debug through macro"));
    EXPECT_FALSE(logFileContains("Info through macro"));
    EXPECT_FALSE(logFileContains("Debug after init"));
    EXPECT_FALSE(logFileContains("Info after init"));
    
    // Now enable logging
    logger.setEnabled(true);
    
    // These should now log
    LOG_INFO("Info when enabled");
    
    // Verify log was written
    EXPECT_TRUE(logFileContains("Info when enabled"));
}

TEST_F(LoggerTest, ConfigFromCommandLine) {
    // Clean up existing files
    if (std::filesystem::exists("cmdline_test.log")) {
        std::filesystem::remove("cmdline_test.log");
    }
    
    // Test with valid arguments
    const char* args1[] = {
        "program",
        "--enable-logging",
        "--log-level", "warning",
        "--log-to-file",
        "--log-file", "cmdline_test.log"
    };
    int argc1 = 7;
    CliParser parser1(argc1, const_cast<char**>(args1));
    ASSERT_TRUE(parser1.parse());
    EXPECT_TRUE(LoggerConfig::configure(parser1));
    EXPECT_TRUE(Logger::getInstance().isInitializedAndEnabled());
    
    // Log something to verify
    LOG_WARNING("Command line test");
    
    // Check file was created and contains log
    EXPECT_TRUE(std::filesystem::exists("cmdline_test.log"));
    
    std::ifstream logFile("cmdline_test.log");
    EXPECT_TRUE(logFile.is_open());
    
    std::string content;
    std::getline(logFile, content);
    EXPECT_TRUE(content.find("Command line test") != std::string::npos);
    logFile.close();

    // Test with invalid file path but fallback to console
    const char* args2[] = {
        "program",
        "--enable-logging",
        "--log-file", "/invalid_path/test.log"
    };
    int argc2 = 4;
    CliParser parser2(argc2, const_cast<char**>(args2));
    ASSERT_TRUE(parser2.parse());
    // Should still return true (fallback to console logging)
    EXPECT_TRUE(LoggerConfig::configure(parser2));
    
    // Clean up
    Logger::getInstance().setEnabled(false);
    if (std::filesystem::exists("cmdline_test.log")) {
        std::filesystem::remove("cmdline_test.log");
    }
}

TEST_F(LoggerTest, ConfigFromCommandLine_WrongConfig) {
    // Test with no logging requested
    const char* args3[] = {
        "program",
        "--some-other-flag"
    };
    int argc3 = 2;
    CliParser parser3(argc3, const_cast<char**>(args3));
    ASSERT_TRUE(parser3.parse());
    EXPECT_TRUE(LoggerConfig::configure(parser3));
    EXPECT_FALSE(Logger::getInstance().isEnabled());
}