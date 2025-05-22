#include "gtest/gtest.h"
#include "bonus/cli/cli_parser.h" // Adjust path as necessary
#include <vector>
#include <string>
#include <cstring> // For strcpy
#include <sstream> // For redirecting cerr
#include <iostream>

namespace { // Anonymous namespace for helpers
    void createArgs(const std::vector<std::string>& arg_strs, int& argc, char**& argv) {
        argc = static_cast<int>(arg_strs.size());
        // Allocate for argv array itself, plus one for a potential NULL terminator for the array
        argv = new char*[argc + 1]; 
        for (int i = 0; i < argc; ++i) {
            argv[i] = new char[arg_strs[i].length() + 1];
            std::strcpy(argv[i], arg_strs[i].c_str());
        }
        argv[argc] = nullptr; // Null-terminate the array itself, good practice
    }

    void freeArgs(int argc, char**& argv) { // Pass argv by reference to set to nullptr
        if (!argv) return;
        for (int i = 0; i < argc; ++i) {
            delete[] argv[i];
        }
        delete[] argv;
        argv = nullptr; // Avoid double deletion by setting the original pointer to nullptr
    }
} // end anonymous namespace

TEST(CustomCliParserTest, DefaultValues) {
    std::vector<std::string> arg_strs = {"./tanks_game"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);

    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse()); 

    EXPECT_FALSE(parser.isHelp());
    EXPECT_EQ(parser.getGameBoardFile(), "");
    EXPECT_FALSE(parser.isOnlyGenerate());
    EXPECT_FALSE(parser.isRunGenerated());
    EXPECT_EQ(parser.getConfigPath(), "");
    EXPECT_FALSE(parser.isEnableLogging());
    EXPECT_EQ(parser.getLogLevel(), "INFO");
    EXPECT_FALSE(parser.isLogToFile());
    EXPECT_FALSE(parser.isNoConsoleLog());
    EXPECT_EQ(parser.getLogFile(), "tankbattle.log");

    freeArgs(argc, argv);
}

// Test Cases for Boolean Flags
TEST(CustomCliParserTest, OnlyGenerateFlag) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--only_generate"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse());
    EXPECT_TRUE(parser.isOnlyGenerate());
    freeArgs(argc, argv);
}

TEST(CustomCliParserTest, RunGeneratedFlag) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--run_generated"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse());
    EXPECT_TRUE(parser.isRunGenerated());
    freeArgs(argc, argv);
}

TEST(CustomCliParserTest, EnableLoggingFlag) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--enable_logging"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse());
    EXPECT_TRUE(parser.isEnableLogging());
    freeArgs(argc, argv);
}

TEST(CustomCliParserTest, LogToFileFlag) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--log_to_file"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse());
    EXPECT_TRUE(parser.isLogToFile());
    freeArgs(argc, argv);
}

TEST(CustomCliParserTest, NoConsoleLogFlag) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--no_console_log"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse());
    EXPECT_TRUE(parser.isNoConsoleLog());
    freeArgs(argc, argv);
}

TEST(CustomCliParserTest, HelpFlagLong) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--help"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse());
    EXPECT_TRUE(parser.isHelp());
    freeArgs(argc, argv);
}

TEST(CustomCliParserTest, HelpFlagShort) {
    std::vector<std::string> arg_strs = {"./tanks_game", "-h"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse());
    EXPECT_TRUE(parser.isHelp());
    freeArgs(argc, argv);
}


// Test Cases for Options with Values
TEST(CustomCliParserTest, ConfigPathWithValue) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--config_path", "path/to/config.txt"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse());
    EXPECT_EQ(parser.getConfigPath(), "path/to/config.txt");
    freeArgs(argc, argv);
}

TEST(CustomCliParserTest, LogLevelWithValue) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--log_level", "DEBUG"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse());
    EXPECT_EQ(parser.getLogLevel(), "DEBUG");
    freeArgs(argc, argv);
}

TEST(CustomCliParserTest, LogFileWithValue) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--log_file", "my_app.log"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse());
    EXPECT_EQ(parser.getLogFile(), "my_app.log");
    freeArgs(argc, argv);
}

// Error Cases for Options with Values
TEST(CustomCliParserTest, ConfigPathMissingValue) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--config_path"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    // Redirect cerr to check for error message
    std::streambuf* oldCerr = std::cerr.rdbuf();
    std::ostringstream newCerr;
    std::cerr.rdbuf(newCerr.rdbuf());

    EXPECT_FALSE(parser.parse()); // Expect parse to fail

    std::cerr.rdbuf(oldCerr); // Restore cerr
    EXPECT_NE(newCerr.str().find("Error: --config_path requires a value."), std::string::npos);
    freeArgs(argc, argv);
}

TEST(CustomCliParserTest, LogLevelMissingValue) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--log_level"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    std::streambuf* oldCerr = std::cerr.rdbuf();
    std::ostringstream newCerr;
    std::cerr.rdbuf(newCerr.rdbuf());
    EXPECT_FALSE(parser.parse());
    std::cerr.rdbuf(oldCerr);
    EXPECT_NE(newCerr.str().find("Error: --log_level requires a value."), std::string::npos);
    freeArgs(argc, argv);
}

TEST(CustomCliParserTest, ConfigPathWithAnotherFlagAsValue) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--config_path", "--log_to_file"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    std::streambuf* oldCerr = std::cerr.rdbuf();
    std::ostringstream newCerr;
    std::cerr.rdbuf(newCerr.rdbuf());
    EXPECT_FALSE(parser.parse()); // Expect parse to fail because --log_to_file is not a valid path
    std::cerr.rdbuf(oldCerr);
    EXPECT_NE(newCerr.str().find("Error: --config_path requires a value."), std::string::npos);
    freeArgs(argc, argv);
}

// Test Cases for Positional Argument
TEST(CustomCliParserTest, PositionalArgumentOnly) {
    std::vector<std::string> arg_strs = {"./tanks_game", "board.txt"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse());
    EXPECT_EQ(parser.getGameBoardFile(), "board.txt");
    freeArgs(argc, argv);
}

TEST(CustomCliParserTest, PositionalArgumentWithFlags) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--enable_logging", "board.txt", "--log_level", "ERROR"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse());
    EXPECT_EQ(parser.getGameBoardFile(), "board.txt");
    EXPECT_TRUE(parser.isEnableLogging());
    EXPECT_EQ(parser.getLogLevel(), "ERROR");
    freeArgs(argc, argv);
}

TEST(CustomCliParserTest, MultiplePositionalArguments) {
    std::vector<std::string> arg_strs = {"./tanks_game", "board1.txt", "board2.txt"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    
    std::streambuf* oldCerr = std::cerr.rdbuf();
    std::ostringstream newCerr;
    std::cerr.rdbuf(newCerr.rdbuf());

    EXPECT_TRUE(parser.parse()); // Current implementation warns but returns true
    
    std::cerr.rdbuf(oldCerr); // Restore cerr
    
    EXPECT_EQ(parser.getGameBoardFile(), "board1.txt"); // First one is taken
    EXPECT_NE(newCerr.str().find("Warning: Multiple positional arguments. Ignoring 'board2.txt'."), std::string::npos);
    freeArgs(argc, argv);
}

// Test Case for Combined Arguments
TEST(CustomCliParserTest, CombinedArguments) {
    std::vector<std::string> arg_strs = {
        "./tanks_game", 
        "--enable_logging", 
        "--log_level", "DEBUG", 
        "myboard.txt", 
        "--log_to_file",
        "--config_path", "configs/my_config.json",
        "--no_console_log"
    };
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    EXPECT_TRUE(parser.parse());

    EXPECT_FALSE(parser.isHelp());
    EXPECT_EQ(parser.getGameBoardFile(), "myboard.txt");
    EXPECT_FALSE(parser.isOnlyGenerate());
    EXPECT_FALSE(parser.isRunGenerated());
    EXPECT_EQ(parser.getConfigPath(), "configs/my_config.json");
    EXPECT_TRUE(parser.isEnableLogging());
    EXPECT_EQ(parser.getLogLevel(), "DEBUG");
    EXPECT_TRUE(parser.isLogToFile());
    EXPECT_TRUE(parser.isNoConsoleLog());
    EXPECT_EQ(parser.getLogFile(), "tankbattle.log"); // Default, as not specified
    freeArgs(argc, argv);
}

// Test Case for Unknown Option
TEST(CustomCliParserTest, UnknownOption) {
    std::vector<std::string> arg_strs = {"./tanks_game", "--unknown-flag", "board_file.txt"};
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);

    std::streambuf* oldCerr = std::cerr.rdbuf();
    std::ostringstream newCerr;
    std::cerr.rdbuf(newCerr.rdbuf());

    EXPECT_TRUE(parser.parse()); // Current implementation warns but returns true
    
    std::cerr.rdbuf(oldCerr); // Restore cerr

    EXPECT_EQ(parser.getGameBoardFile(), "board_file.txt"); // Positional should still be found
    EXPECT_NE(newCerr.str().find("Warning: Unknown option '--unknown-flag'."), std::string::npos);
    freeArgs(argc, argv);
}

// Test Case for Help Message Content (Basic)
TEST(CustomCliParserTest, HelpMessageContent) {
    std::vector<std::string> arg_strs = {"./tanks_game"}; // Args don't matter for getHelpMessage content
    int argc = 0;
    char** argv = nullptr;
    createArgs(arg_strs, argc, argv);
    CliParser parser(argc, argv);
    
    std::string help = parser.getHelpMessage();
    EXPECT_NE(help.find("--help"), std::string::npos);
    EXPECT_NE(help.find("--only_generate"), std::string::npos);
    EXPECT_NE(help.find("--config_path <path>"), std::string::npos);
    EXPECT_NE(help.find("--log_level <level>"), std::string::npos);
    // ... add more checks for key options ...
    
    freeArgs(argc, argv);
}
