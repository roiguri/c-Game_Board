#include "bonus/cli/cli_parser.h"
#include <iostream> // For error messages

CliParser::CliParser(int argc, char** argv) : argc_(argc), argv_(argv) {
    // Convert argv to a vector of strings for easier processing
    for (int i = 0; i < argc_; ++i) {
        tokens_.push_back(std::string(argv_[i]));
    }
}

bool CliParser::nextTokenIsValue(size_t i) const {
    return (i + 1 < tokens_.size() &&
            tokens_[i+1].rfind("--", 0) != 0 &&
            tokens_[i+1].rfind("-", 0) != 0);
}

bool CliParser::parse() {
    // tokens_[0] is program name, skip it.
    for (size_t i = 1; i < tokens_.size(); ++i) {
        const std::string& token = tokens_[i];

        if (token == "--help" || token == "-h") {
            help_ = true;
            // As per instructions, continue parsing, main logic will check isHelp() first.
        } else if (token == "--only_generate") {
            onlyGenerate_ = true;
        } else if (token == "--run_generated") {
            runGenerated_ = true;
        } else if (token == "--enable_logging") {
            enableLogging_ = true;
        } else if (token == "--log_to_file") {
            logToFile_ = true;
        } else if (token == "--no_console_log") {
            noConsoleLog_ = true;
        } else if (token == "--config_path") {
            if (nextTokenIsValue(i)) {
                configPath_ = tokens_[++i];
            } else {
                std::cerr << "Error: --config_path requires a value." << std::endl;
                return false;
            }
        } else if (token == "--log_level") {
            if (nextTokenIsValue(i)) {
                logLevel_ = tokens_[++i];
            } else {
                std::cerr << "Error: --log_level requires a value." << std::endl;
                return false;
            }
        } else if (token == "--log_file") {
            if (nextTokenIsValue(i)) {
                logFile_ = tokens_[++i];
                logFileSet_ = true;
            } else {
                std::cerr << "Error: --log_file requires a value." << std::endl;
                return false;
            }
        } else if (token.rfind("-", 0) != 0) { // Doesn't start with -- or -, potential positional argument
            if (gameBoardFile_.empty()) { // Take the first positional as the game board file
                gameBoardFile_ = token;
            } else {
                std::cerr << "Warning: Multiple positional arguments. Ignoring '" << token << "'." << std::endl;
            }
        } else {
            std::cerr << "Warning: Unknown option '" << token << "'." << std::endl;
        }
    }
    return true; // Parsing deemed successful if no explicit error caused a false return
}

std::string CliParser::getHelpMessage() const {
    return "Usage: tanks_game [options] [game_board_file]\n"
           "Options:\n"
           "  -h, --help                 Show this help message\n"
           "  --only_generate            Only generate a board\n"
           "  --run_generated            Generate a board and run the game with it\n"
           "  --config_path <path>       Path to board configuration file\n"
           "  --enable_logging           Enable application logging\n"
           "  --log_level <level>        Set log level (DEBUG, INFO, WARNING, ERROR)\n"
           "  --log_to_file              Enable logging to a file\n"
           "  --no_console_log           Disable console logging\n"
           "  --log_file <path>          Specify log file path (default: tankbattle.log)\n";
}

bool CliParser::isHelp() const { return help_; }
std::string CliParser::getGameBoardFile() const { return gameBoardFile_; }
bool CliParser::isOnlyGenerate() const { return onlyGenerate_; }
bool CliParser::isRunGenerated() const { return runGenerated_; }
std::string CliParser::getConfigPath() const { return configPath_; }
bool CliParser::isEnableLogging() const { return enableLogging_; }
std::string CliParser::getLogLevel() const { return logLevel_; }
bool CliParser::isLogToFile() const { return logToFile_; }
bool CliParser::isNoConsoleLog() const { return noConsoleLog_; }
std::string CliParser::getLogFile() const { return logFile_; }
bool CliParser::isLogFileSet() const { return logFileSet_; }
