#include "command_line_parser.h"
#include <algorithm>
#include <filesystem>
#include <sstream>

CommandLineParser::CommandLineParser() 
    : m_mode(Mode::Unknown), m_verbose(false), m_hasBasicFlag(false), m_hasComparativeFlag(false), m_hasCompetitionFlag(false) {
}

CommandLineParser::~CommandLineParser() = default;

CommandLineParser::ParseResult CommandLineParser::parse(int argc, char* argv[]) {
    reset();
    
    ParseResult result;
    
    // Step 1: Parse all flags and arguments
    if (!parseFlags(argc, argv)) {
        result.success = false;
        result.errorMessage = "Failed to parse command line arguments";
        result.missingParams = m_missingParams;
        result.unsupportedParams = m_unsupportedParams;
        return result;
    }
    
    // Step 2: Detect mode
    m_mode = detectMode();
    result.mode = m_mode;
    
    if (m_mode == Mode::Unknown) {
        result.success = false;
        result.errorMessage = "Unable to determine parsing mode";
        return result;
    }
    
    // Step 3: Validate mode and parameters
    if (!validateMode()) {
        result.success = false;
        result.errorMessage = "Invalid mode configuration";
        result.missingParams = m_missingParams;
        result.unsupportedParams = m_unsupportedParams;
        return result;
    }
    
    result.success = true;
    return result;
}

bool CommandLineParser::isVerbose() const {
    return m_verbose;
}

std::optional<int> CommandLineParser::getNumThreads() const {
    // Only return threads if supported by current mode
    if (!isParameterSupported("num_threads")) {
        return std::nullopt;
    }
    
    auto it = m_parameters.find("num_threads");
    if (it == m_parameters.end()) {
        return std::nullopt;
    }
    
    try {
        int threads = std::stoi(it->second);
        return threads > 0 ? std::optional<int>(threads) : std::nullopt;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::string CommandLineParser::getMapFile() const {
    if (m_mode != Mode::Basic) return "";
    auto it = m_parameters.find("game_map");
    return it != m_parameters.end() ? it->second : "";
}

std::string CommandLineParser::getGameManagerLib() const {
    if (m_mode != Mode::Basic) return "";
    auto it = m_parameters.find("game_manager");
    return it != m_parameters.end() ? it->second : "";
}

std::string CommandLineParser::getAlgorithm1Lib() const {
    if (m_mode != Mode::Basic) return "";
    auto it = m_parameters.find("algorithm1");
    return it != m_parameters.end() ? it->second : "";
}

std::string CommandLineParser::getAlgorithm2Lib() const {
    if (m_mode != Mode::Basic) return "";
    auto it = m_parameters.find("algorithm2");
    return it != m_parameters.end() ? it->second : "";
}

std::string CommandLineParser::getGameMap() const {
    if (m_mode != Mode::Comparative) return "";
    auto it = m_parameters.find("game_map");
    return it != m_parameters.end() ? it->second : "";
}

std::string CommandLineParser::getGameManagersFolder() const {
    if (m_mode != Mode::Comparative) return "";
    auto it = m_parameters.find("game_managers_folder");
    return it != m_parameters.end() ? it->second : "";
}

std::string CommandLineParser::getAlgorithm1() const {
    if (m_mode != Mode::Comparative) return "";
    auto it = m_parameters.find("algorithm1");
    return it != m_parameters.end() ? it->second : "";
}

std::string CommandLineParser::getAlgorithm2() const {
    if (m_mode != Mode::Comparative) return "";
    auto it = m_parameters.find("algorithm2");
    return it != m_parameters.end() ? it->second : "";
}

std::string CommandLineParser::getGameMapsFolder() const {
    if (m_mode != Mode::Competition) return "";
    auto it = m_parameters.find("game_maps_folder");
    return it != m_parameters.end() ? it->second : "";
}

std::string CommandLineParser::getGameManager() const {
    if (m_mode != Mode::Competition) return "";
    auto it = m_parameters.find("game_manager");
    return it != m_parameters.end() ? it->second : "";
}

std::string CommandLineParser::getAlgorithmsFolder() const {
    if (m_mode != Mode::Competition) return "";
    auto it = m_parameters.find("algorithms_folder");
    return it != m_parameters.end() ? it->second : "";
}

std::string CommandLineParser::generateUsage(const std::string& programName) const {
    std::ostringstream usage;
    usage << "Usage:\n";
    usage << "  Basic mode:\n";
    usage << "    " << programName << " -basic game_map=<file> game_manager=<file> ";
    usage << "algorithm1=<file> algorithm2=<file> [-verbose]\n\n";
    
    usage << "  Comparative mode:\n";
    usage << "    " << programName << " -comparative game_map=<file> game_managers_folder=<folder> ";
    usage << "algorithm1=<file> algorithm2=<file> [num_threads=<num>] [-verbose]\n\n";
    
    usage << "  Competition mode:\n";
    usage << "    " << programName << " -competition game_maps_folder=<folder> game_manager=<file> ";
    usage << "algorithms_folder=<folder> [num_threads=<num>] [-verbose]\n\n";
    
    usage << "Notes:\n";
    usage << "  - All arguments can appear in any order\n";
    usage << "  - Key=value parameters must have no spaces around the = sign\n";
    usage << "  - All non-optional arguments are mandatory\n";
    usage << "  - Mode flag (-basic, -comparative, -competition) is required\n";
    
    return usage.str();
}

void CommandLineParser::reset() {
    m_mode = Mode::Unknown;
    m_verbose = false;
    m_parameters.clear();
    m_positionalArgs.clear();
    m_missingParams.clear();
    m_unsupportedParams.clear();
    m_hasBasicFlag = false;
    m_hasComparativeFlag = false;
    m_hasCompetitionFlag = false;
}

bool CommandLineParser::parseFlags(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-verbose") {
            m_verbose = true;
        } else if (arg == "-basic") {
            m_hasBasicFlag = true;
        } else if (arg == "-comparative") {
            m_hasComparativeFlag = true;
        } else if (arg == "-competition") {
            m_hasCompetitionFlag = true;
        } else if (arg.length() > 0 && arg[0] == '-') {
            // Unknown flag
            m_unsupportedParams.push_back(arg);
        } else if (arg.find('=') != std::string::npos) {
            // Key=value pair
            std::string key, value;
            if (parseKeyValue(arg, key, value)) {
                if (key == "num_threads") {
                    try {
                        int threads = std::stoi(value);
                        if (threads > 0) {
                            m_parameters[key] = value;
                        } else {
                            m_unsupportedParams.push_back(arg + " (must be positive)");
                        }
                    } catch (const std::exception&) {
                        m_unsupportedParams.push_back(arg + " (invalid number)");
                    }
                } else {
                    m_parameters[key] = value;
                }
            } else {
                m_unsupportedParams.push_back(arg);
            }
        } else {
            // Unknown argument (not flag, not key=value)
            m_unsupportedParams.push_back(arg);
        }
    }
    
    return m_unsupportedParams.empty();
}

CommandLineParser::Mode CommandLineParser::detectMode() const {
    // Check for conflicting mode flags
    int flagCount = (m_hasBasicFlag ? 1 : 0) + (m_hasComparativeFlag ? 1 : 0) + (m_hasCompetitionFlag ? 1 : 0);
    if (flagCount > 1) {
        return Mode::Unknown; // Multiple mode flags
    }
    
    // Return mode based on explicit flags only
    if (m_hasBasicFlag) {
        return Mode::Basic;
    }
    
    if (m_hasComparativeFlag) {
        return Mode::Comparative;
    }
    
    if (m_hasCompetitionFlag) {
        return Mode::Competition;
    }
    
    // No explicit mode flag specified
    return Mode::Unknown;
}

bool CommandLineParser::validateMode() {
    // Validate unsupported parameters for current mode
    for (const auto& [key, value] : m_parameters) {
        if (!isParameterSupported(key)) {
            m_unsupportedParams.push_back(key + "=" + value);
        }
    }
    
    // Validate required parameters
    return validateRequiredParameters() && m_unsupportedParams.empty();
}

bool CommandLineParser::parseKeyValue(const std::string& arg, std::string& key, std::string& value) {
    size_t eqPos = arg.find('=');
    if (eqPos == std::string::npos || eqPos == 0 || eqPos == arg.length() - 1) {
        return false;
    }
    
    key = arg.substr(0, eqPos);
    value = arg.substr(eqPos + 1);
    
    // Check for spaces around = (not allowed)
    if (!key.empty() && key.back() == ' ') return false;
    if (!value.empty() && value.front() == ' ') return false;
    
    return !key.empty() && !value.empty();
}

bool CommandLineParser::validateRequiredParameters() {
    std::vector<std::string> required = getRequiredParameters(m_mode);
    m_missingParams.clear();
    
    for (const auto& param : required) {
        if (m_parameters.find(param) == m_parameters.end() || m_parameters.at(param).empty()) {
            m_missingParams.push_back(param);
        }
    }
    
    return m_missingParams.empty();
}

std::vector<std::string> CommandLineParser::getRequiredParameters(Mode mode) const {
    switch (mode) {
        case Mode::Basic:
            return {"game_map", "game_manager", "algorithm1", "algorithm2"};
        case Mode::Comparative:
            return {"game_map", "game_managers_folder", "algorithm1", "algorithm2"};
        case Mode::Competition:
            return {"game_maps_folder", "game_manager", "algorithms_folder"};
        default:
            return {};
    }
}

bool CommandLineParser::isParameterSupported(const std::string& paramName) const {
    std::vector<std::string> supported = getSupportedParameters(m_mode);
    return std::find(supported.begin(), supported.end(), paramName) != supported.end();
}

std::vector<std::string> CommandLineParser::getSupportedParameters(Mode mode) const {
    switch (mode) {
        case Mode::Basic:
            return {"game_map", "game_manager", "algorithm1", "algorithm2"};
        case Mode::Comparative:
            return {"game_map", "game_managers_folder", "algorithm1", "algorithm2", "num_threads"};
        case Mode::Competition:
            return {"game_maps_folder", "game_manager", "algorithms_folder", "num_threads"};
        default:
            return {};
    }
}