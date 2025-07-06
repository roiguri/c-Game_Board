#pragma once

#include <map>
#include <string>
#include <vector>
#include <optional>

class CommandLineParser {
public:
    enum class Mode {
        Basic,
        Comparative,
        Competition,
        Unknown
    };

    struct ParseResult {
        bool success = false;
        Mode mode = Mode::Unknown;
        std::string errorMessage;
        std::vector<std::string> missingParams;
        std::vector<std::string> unsupportedParams;
    };

    CommandLineParser();
    ~CommandLineParser();

    ParseResult parse(int argc, char* argv[]);
    bool isVerbose() const;
    std::optional<int> getNumThreads() const;

    // Basic mode accessors
    std::string getMapFile() const;
    std::string getGameManagerLib() const;
    std::string getAlgorithm1Lib() const;
    std::string getAlgorithm2Lib() const;

    // Comparative mode accessors
    std::string getGameMap() const;
    std::string getGameManagersFolder() const;
    std::string getAlgorithm1() const;
    std::string getAlgorithm2() const;

    // Competition mode accessors
    std::string getGameMapsFolder() const;
    std::string getGameManager() const;
    std::string getAlgorithmsFolder() const;

    std::string generateUsage(const std::string& programName) const;
    void reset();

private:
    Mode m_mode;
    bool m_verbose;
    std::map<std::string, std::string> m_parameters;
    std::vector<std::string> m_positionalArgs;
    std::vector<std::string> m_missingParams;
    std::vector<std::string> m_unsupportedParams;
    bool m_hasBasicFlag;
    bool m_hasComparativeFlag;
    bool m_hasCompetitionFlag;

    bool parseFlags(int argc, char* argv[]);
    Mode detectMode() const;
    bool validateMode();
    bool parseKeyValue(const std::string& arg, std::string& key, std::string& value);
    bool validateRequiredParameters();
    std::vector<std::string> getRequiredParameters(Mode mode) const;
    bool isParameterSupported(const std::string& paramName) const;
    std::vector<std::string> getSupportedParameters(Mode mode) const;
};