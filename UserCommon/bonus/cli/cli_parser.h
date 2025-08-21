#pragma once

#include <map>
#include <string>
#include <vector>

namespace UserCommon_318835816_211314471 {

class CliParser {
public:
    CliParser(int argc, char** argv);

    bool parse();
    std::string getHelpMessage() const;

    // Getter methods
    bool isHelp() const;
    std::string getGameBoardFile() const;
    bool isOnlyGenerate() const;
    bool isRunGenerated() const;
    std::string getConfigPath() const;
    
    bool isEnableLogging() const;
    std::string getLogLevel() const;
    bool isLogToFile() const;
    bool isNoConsoleLog() const;
    std::string getLogFile() const;
    bool isLogFileSet() const;
    bool isLogLevelSet() const;

private:
    // Input arguments
    int argc_;
    char** argv_;
    std::vector<std::string> tokens_; // Store argv as vector of strings

    // Helper for value check
    bool nextTokenIsValue(size_t i) const;

    // Parsed values storage
    bool help_ = false;
    std::string gameBoardFile_ = "";
    bool onlyGenerate_ = false;
    bool runGenerated_ = false;
    std::string configPath_ = ""; // Default empty
    
    bool enableLogging_ = false;
    std::string logLevel_ = "INFO"; // Default
    bool logToFile_ = false;
    bool noConsoleLog_ = false;
    std::string logFile_ = "tankbattle.log"; // Default
    bool logFileSet_ = false;
    bool logLevelSet_ = false;
};

} // namespace UserCommon_318835816_211314471
