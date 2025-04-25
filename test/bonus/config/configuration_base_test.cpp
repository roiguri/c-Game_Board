#include "gtest/gtest.h"
#include "bonus/config/configuration_base.h"
#include <fstream>
#include <string>

// Test implementation of ConfigurationBase for testing
class TestConfig : public ConfigurationBase {
public:
    int intValue = 10;
    float floatValue = 0.5f;
    std::string stringValue = "default";

protected:
    bool parseConfigValue(const std::string& key, const std::string& value) override {
        if (key == "int_value") {
            return parseIntValue(value, intValue, 0, 100);
        }
        else if (key == "float_value") {
            return parseFloatValue(value, floatValue, 0.0f, 1.0f);
        }
        else if (key == "string_value") {
            std::set<std::string> allowedValues = {"option1", "option2", "option3"};
            return parseStringValue(value, stringValue, allowedValues);
        }
        else if (key == "any_string_value") {
            return parseStringValue(value, stringValue);
        }
        return false;
    }
};

class ConfigurationBaseTest : public ::testing::Test {
protected:
    // Helper to write a test config file
    std::string writeTestConfig(const std::string& content) {
        std::string filename = "test_config_base.txt";
        std::ofstream file(filename);
        file << content;
        file.close();
        return filename;
    }
    
    void TearDown() override {
        // Clean up any test files
        std::remove("test_config_base.txt");
    }
};

// Test loading a valid configuration file
TEST_F(ConfigurationBaseTest, LoadValidConfig) {
    std::string configContent = 
        "int_value: 42\n"
        "float_value: 0.75\n"
        "string_value: option2\n";
    
    std::string filename = writeTestConfig(configContent);
    
    TestConfig config;
    EXPECT_TRUE(config.loadFromFile(filename));
    
    EXPECT_EQ(config.intValue, 42);
    EXPECT_FLOAT_EQ(config.floatValue, 0.75f);
    EXPECT_EQ(config.stringValue, "option2");
}

// Test loading a configuration file with invalid values
TEST_F(ConfigurationBaseTest, LoadInvalidValues) {
    std::string configContent = 
        "int_value: -10\n" // Below minimum
        "float_value: 1.5\n" // Above maximum
        "string_value: invalid\n"; // Not in allowed values
    
    std::string filename = writeTestConfig(configContent);
    
    TestConfig config;
    EXPECT_FALSE(config.loadFromFile(filename));
    
    // Values should remain at defaults due to validation failures
    EXPECT_EQ(config.intValue, 10);
    EXPECT_FLOAT_EQ(config.floatValue, 0.5f);
    EXPECT_EQ(config.stringValue, "default");
}

// Test handling of comments and empty lines
TEST_F(ConfigurationBaseTest, CommentsAndEmptyLines) {
    std::string configContent = 
        "# This is a comment\n"
        "\n" // Empty line
        "int_value: 42\n"
        "# Another comment\n"
        "float_value: 0.75\n";
    
    std::string filename = writeTestConfig(configContent);
    
    TestConfig config;
    EXPECT_TRUE(config.loadFromFile(filename));
    
    EXPECT_EQ(config.intValue, 42);
    EXPECT_FLOAT_EQ(config.floatValue, 0.75f);
}

// Test loading a non-existent file
TEST_F(ConfigurationBaseTest, NonExistentFile) {
    TestConfig config;
    EXPECT_FALSE(config.loadFromFile("non_existent_file.txt"));
    
    // Values should remain at defaults
    EXPECT_EQ(config.intValue, 10);
    EXPECT_FLOAT_EQ(config.floatValue, 0.5f);
    EXPECT_EQ(config.stringValue, "default");
}

// Test any string value validation (no restrictions)
TEST_F(ConfigurationBaseTest, AnyStringValue) {
    std::string configContent = "any_string_value: some_random_string\n";
    
    std::string filename = writeTestConfig(configContent);
    
    TestConfig config;
    EXPECT_TRUE(config.loadFromFile(filename));
    
    EXPECT_EQ(config.stringValue, "some_random_string");
}

// Test invalid format (missing colon)
TEST_F(ConfigurationBaseTest, InvalidFormat) {
    std::string configContent = "int_value 42\n"; // Missing colon
    
    std::string filename = writeTestConfig(configContent);
    
    TestConfig config;
    EXPECT_FALSE(config.loadFromFile(filename));
    
    // Value should remain at default
    EXPECT_EQ(config.intValue, 10);
}

// Test parsing integer at boundary values
TEST_F(ConfigurationBaseTest, IntBoundaries) {
    std::string configContent = 
        "int_value: 0\n" // Minimum allowed
        "float_value: 0.75\n"; // Just to have multiple values
    
    std::string filename = writeTestConfig(configContent);
    
    TestConfig config;
    EXPECT_TRUE(config.loadFromFile(filename));
    
    EXPECT_EQ(config.intValue, 0); // Should accept minimum
    
    // Test maximum boundary
    std::string maxConfig = "int_value: 100\n"; // Maximum allowed
    std::string maxFilename = writeTestConfig(maxConfig);
    
    TestConfig maxConfig2;
    EXPECT_TRUE(maxConfig2.loadFromFile(maxFilename));
    
    EXPECT_EQ(maxConfig2.intValue, 100); // Should accept maximum
}