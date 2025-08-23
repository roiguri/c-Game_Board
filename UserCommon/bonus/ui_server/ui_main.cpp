#include <httplib.h>
#include <iostream>
#include <string>
#include <mutex>
#include <memory>
#include <atomic>
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace UserCommon_318835816_211314471 {

// Configuration validation and command building utilities
class ConfigurationValidator {
public:
    struct ValidationResult {
        bool success = false;
        std::vector<std::string> errors;
        std::string errorMessage;
    };

    static ValidationResult validateConfiguration(const json& config) {
        ValidationResult result;
        result.success = true;
        
        // Check if mode is specified and valid
        if (!config.contains("mode")) {
            result.errors.push_back("Missing 'mode' parameter");
            result.success = false;
        } else {
            std::string mode = config["mode"];
            if (mode != "basic" && mode != "comparative" && mode != "competition") {
                result.errors.push_back("Invalid mode: " + mode + ". Must be 'basic', 'comparative', or 'competition'");
                result.success = false;
            } else {
                // Mode-specific validation
                result.success = validateModeParameters(config, mode, result.errors) && result.success;
            }
        }
        
        if (!result.success) {
            result.errorMessage = "Configuration validation failed: ";
            for (size_t i = 0; i < result.errors.size(); ++i) {
                if (i > 0) result.errorMessage += "; ";
                result.errorMessage += result.errors[i];
            }
        }
        
        return result;
    }

    static std::vector<std::string> buildCommandArgs(const json& config) {
        std::vector<std::string> args = {"../Simulator/simulator_318835816_211314471"};
        
        std::string mode = config["mode"];
        args.push_back("-" + mode);
        
        // Add mode-specific parameters
        if (mode == "basic") {
            args.push_back("game_map=" + config["gameMap"].get<std::string>());
            args.push_back("game_manager=" + config["gameManager"].get<std::string>());
            args.push_back("algorithm1=" + config["algorithm1"].get<std::string>());
            args.push_back("algorithm2=" + config["algorithm2"].get<std::string>());
        } else if (mode == "comparative") {
            args.push_back("game_map=" + config["gameMap"].get<std::string>());
            args.push_back("game_managers_folder=" + config["gameManagersFolder"].get<std::string>());
            args.push_back("algorithm1=" + config["algorithm1"].get<std::string>());
            args.push_back("algorithm2=" + config["algorithm2"].get<std::string>());
            if (config.contains("numThreads")) {
                args.push_back("num_threads=" + std::to_string(config["numThreads"].get<int>()));
            }
        } else if (mode == "competition") {
            args.push_back("game_maps_folder=" + config["gameMapsFolder"].get<std::string>());
            args.push_back("game_manager=" + config["gameManager"].get<std::string>());
            args.push_back("algorithms_folder=" + config["algorithmsFolder"].get<std::string>());
            if (config.contains("numThreads")) {
                args.push_back("num_threads=" + std::to_string(config["numThreads"].get<int>()));
            }
        }
        
        // Add optional flags
        if (config.contains("verbose") && config["verbose"].get<bool>()) {
            args.push_back("-verbose");
        }
        
        return args;
    }

private:
    static bool validateModeParameters(const json& config, const std::string& mode, std::vector<std::string>& errors) {
        bool success = true;
        
        if (mode == "basic") {
            success = validateRequiredParam(config, "gameMap", errors) && success;
            success = validateRequiredParam(config, "gameManager", errors) && success;
            success = validateRequiredParam(config, "algorithm1", errors) && success;
            success = validateRequiredParam(config, "algorithm2", errors) && success;
        } else if (mode == "comparative") {
            success = validateRequiredParam(config, "gameMap", errors) && success;
            success = validateRequiredParam(config, "gameManagersFolder", errors) && success;
            success = validateRequiredParam(config, "algorithm1", errors) && success;
            success = validateRequiredParam(config, "algorithm2", errors) && success;
        } else if (mode == "competition") {
            success = validateRequiredParam(config, "gameMapsFolder", errors) && success;
            success = validateRequiredParam(config, "gameManager", errors) && success;
            success = validateRequiredParam(config, "algorithmsFolder", errors) && success;
        }
        
        return success;
    }

    static bool validateRequiredParam(const json& config, const std::string& param, std::vector<std::string>& errors) {
        if (!config.contains(param) || config[param].get<std::string>().empty()) {
            errors.push_back("Missing required parameter: " + param);
            return false;
        }
        return true;
    }

    static bool validateFileExists(const json& config, const std::string& param, std::vector<std::string>& errors) {
        if (!config.contains(param)) return true; // Already handled by validateRequiredParam
        
        std::string path = config[param].get<std::string>();
        if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
            errors.push_back("File not found: " + param + " = " + path);
            return false;
        }
        return true;
    }

    static bool validateFolderExists(const json& config, const std::string& param, std::vector<std::string>& errors) {
        if (!config.contains(param)) return true; // Already handled by validateRequiredParam
        
        std::string path = config[param].get<std::string>();
        if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
            errors.push_back("Directory not found: " + param + " = " + path);
            return false;
        }
        return true;
    }
};

class ProcessManager {
private:
    std::atomic<bool> running{false};
    std::string cachedOutput;
    std::mutex outputMutex;

public:
    void startProcess(const std::vector<std::string>& args) {
        std::cout << "🔍 SimpleProcessManager::startProcess() called" << std::endl;
        if (running.load()) {
            std::cout << "🔍 Process already running, throwing exception" << std::endl;
            throw std::runtime_error("Process already running");
        }
        
        running.store(true);
        
        std::string command = args[0];
        for (size_t i = 1; i < args.size(); ++i) {
            command += " " + args[i];
        }
        command += " 2>&1";  // Redirect stderr to stdout
        
        std::cout << "🔍 Executing command: " << command << std::endl;
        
        try {
            // Execute command and capture output
            FILE* pipe = popen(command.c_str(), "r");
            if (!pipe) {
                running.store(false);
                throw std::runtime_error("Failed to execute command");
            }
            
            // Read all output
            std::string output;
            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), pipe)) {
                output += buffer;
            }
            
            // Get exit code and close pipe
            int exitCode = pclose(pipe);
            output += "\nProcess exited with code: " + std::to_string(WEXITSTATUS(exitCode));
            
            // Store output safely
            {
                std::lock_guard<std::mutex> lock(outputMutex);
                cachedOutput = output;
            }
            
            std::cout << "🔍 Process completed with exit code: " << WEXITSTATUS(exitCode) << std::endl;
            running.store(false);
            
        } catch (const std::exception& e) {
            std::cout << "🔍 Process execution failed: " << e.what() << std::endl;
            running.store(false);
            throw;
        }
    }

    std::string getOutput() {
        std::lock_guard<std::mutex> lock(outputMutex);
        return cachedOutput;
    }

    bool isRunning() const {
        return running.load();
    }

    void stop() {
        // For simplicity, we don't implement stop for popen-based execution
        // The process will complete naturally
        std::cout << "🔍 Stop called (no-op for popen implementation)" << std::endl;
    }

    void reset() {
        std::cout << "🔍 ProcessManager::reset() called" << std::endl;
        running.store(false);
        std::lock_guard<std::mutex> lock(outputMutex);
        cachedOutput.clear();
        std::cout << "🔍 ProcessManager reset completed" << std::endl;
    }

    ~ProcessManager() {
        // No cleanup needed for popen-based approach
    }
};

class TankSimulatorUIServer {
private:
    httplib::Server server;
    int port;
    std::unique_ptr<ProcessManager> processManager;
    json lastConfiguration; // Store last used configuration for result path resolution
    std::mutex configMutex; // Thread safety for configuration access

public:
    explicit TankSimulatorUIServer(int port = 8080) : port(port), processManager(std::make_unique<ProcessManager>()) {
        setupRoutes();
    }

    void setupRoutes() {
        // Serve main HTML file
        server.Get("/", [this](const httplib::Request&, httplib::Response& res) {
            serveFile("resources/index.html", res);
        });

        // Serve CSS file
        server.Get("/style.css", [this](const httplib::Request&, httplib::Response& res) {
            serveFile("resources/style.css", res);
        });

        // Serve JavaScript file
        server.Get("/app.js", [this](const httplib::Request&, httplib::Response& res) {
            serveFile("resources/app.js", res);
        });

        // Health check endpoint
        server.Get("/health", [](const httplib::Request&, httplib::Response& res) {
            res.set_content("{\"status\":\"ok\",\"service\":\"tank_simulator_ui\"}", "application/json");
        });

        // Simulation execution endpoint
        server.Post("/api/simulate/run", [this](const httplib::Request& req, httplib::Response& res) {
            std::cout << "🔍 POST /api/simulate/run called" << std::endl;
            try {
                std::cout << "🔍 Checking if process is running..." << std::endl;
                if (processManager->isRunning()) {
                    std::cout << "🔍 Process already running, returning conflict" << std::endl;
                    res.status = 409; // Conflict
                    res.set_content("{\"error\":\"Simulation already running\"}", "application/json");
                    return;
                }

                // Reset process manager state for clean start
                processManager->reset();

                // Parse JSON configuration from request body
                json config;
                if (!req.body.empty()) {
                    try {
                        config = json::parse(req.body);
                        std::cout << "🔍 Received configuration: " << config.dump(2) << std::endl;
                    } catch (const std::exception& e) {
                        res.status = 400; // Bad Request
                        json error = {
                            {"error", "Invalid JSON in request body"},
                            {"details", e.what()}
                        };
                        res.set_content(error.dump(), "application/json");
                        return;
                    }
                } else {
                    // Default basic mode configuration for backward compatibility
                    std::cout << "🔍 No configuration provided, using default basic mode" << std::endl;
                    config = {
                        {"mode", "basic"},
                        {"gameMap", "../UserCommon/bonus/ui_server/examples/game_maps/input_a.txt"},
                        {"gameManager", "../UserCommon/bonus/ui_server/examples/game_managers/RealGameManager_318835816_211314471.so"},
                        {"algorithm1", "../UserCommon/bonus/ui_server/examples/algorithms/TestAlgorithm_098765432_123456789.so"},
                        {"algorithm2", "../UserCommon/bonus/ui_server/examples/algorithms/RealAlgorithm_318835816_211314471.so"},
                        {"verbose", true}
                    };
                }

                // Validate configuration
                auto validation = ConfigurationValidator::validateConfiguration(config);
                if (!validation.success) {
                    std::cout << "🔍 Configuration validation failed: " << validation.errorMessage << std::endl;
                    res.status = 400; // Bad Request
                    json error = {
                        {"error", "Configuration validation failed"},
                        {"details", validation.errorMessage},
                        {"errors", validation.errors}
                    };
                    res.set_content(error.dump(), "application/json");
                    return;
                }

                // Build command arguments from validated configuration
                std::vector<std::string> args = ConfigurationValidator::buildCommandArgs(config);
                
                // Validate file existence for basic mode
                if (config["mode"] == "basic") {
                    std::vector<std::string> missingFiles;
                    
                    // Check simulator executable
                    if (!std::filesystem::exists("../Simulator/simulator_318835816_211314471")) {
                        missingFiles.push_back("simulator_318835816_211314471 executable");
                    }
                    
                    // Check game files
                    std::string gameMap = config["gameMap"];
                    std::string gameManager = config["gameManager"];
                    std::string algorithm1 = config["algorithm1"];
                    std::string algorithm2 = config["algorithm2"];
                    
                    if (!std::filesystem::exists(gameMap)) {
                        missingFiles.push_back("Game map: " + gameMap);
                    }
                    if (!std::filesystem::exists(gameManager)) {
                        missingFiles.push_back("Game manager: " + gameManager);
                    }
                    if (!std::filesystem::exists(algorithm1)) {
                        missingFiles.push_back("Algorithm 1: " + algorithm1);
                    }
                    if (!std::filesystem::exists(algorithm2)) {
                        missingFiles.push_back("Algorithm 2: " + algorithm2);
                    }
                    
                    if (!missingFiles.empty()) {
                        std::cout << "🔍 File validation failed - missing files:" << std::endl;
                        for (const auto& file : missingFiles) {
                            std::cout << "  - " << file << std::endl;
                        }
                        
                        res.status = 400; // Bad Request
                        json error = {
                            {"error", "Required files not found"},
                            {"details", "One or more required files are missing"},
                            {"missingFiles", missingFiles}
                        };
                        res.set_content(error.dump(), "application/json");
                        return;
                    }
                }
                
                // Build command string for display
                std::string commandString;
                for (size_t i = 0; i < args.size(); ++i) {
                    if (i > 0) commandString += " ";
                    // No quotes - keep it simple and consistent
                    commandString += args[i];
                }
                
                std::cout << "🔍 Built command: " << commandString << std::endl;

                // Store configuration for result path resolution
                {
                    std::lock_guard<std::mutex> lock(configMutex);
                    lastConfiguration = config;
                }
                
                std::cout << "🔍 Calling processManager->startProcess..." << std::endl;
                processManager->startProcess(args);
                std::cout << "🔍 startProcess completed" << std::endl;
                
                try {
                    json response = {
                        {"status", "started"},
                        {"message", "Simulation started successfully"},
                        {"mode", config["mode"]},
                        {"command", commandString}
                    };
                    
                    // Add configuration separately to catch any serialization issues
                    try {
                        response["configuration"] = config;
                    } catch (const std::exception& configErr) {
                        std::cout << "⚠️ Warning: Could not serialize configuration: " << configErr.what() << std::endl;
                        response["configuration"] = "Serialization error";
                    }
                    
                    std::string responseStr = response.dump();
                    std::cout << "🔍 Response JSON length: " << responseStr.length() << " bytes" << std::endl;
                    res.set_content(responseStr, "application/json");
                    
                } catch (const std::exception& jsonErr) {
                    std::cout << "❌ JSON Response Error: " << jsonErr.what() << std::endl;
                    json fallbackResponse = {
                        {"status", "started"},
                        {"message", "Simulation started successfully"},
                        {"mode", config["mode"].get<std::string>()},
                        {"command", commandString}
                    };
                    res.set_content(fallbackResponse.dump(), "application/json");
                }
                
            } catch (const std::exception& e) {
                res.status = 500;
                json error = {
                    {"error", "Failed to start simulation"},
                    {"details", e.what()}
                };
                res.set_content(error.dump(), "application/json");
            }
        });

        // Simple output polling endpoint instead of SSE for compatibility
        server.Get("/api/output", [this](const httplib::Request&, httplib::Response& res) {
            json response = {
                {"running", processManager->isRunning()},
                {"output", processManager->getOutput()}
            };
            res.set_content(response.dump(), "application/json");
        });

        // Results discovery endpoint - finds latest result files based on configuration
        server.Get("/api/results/latest", [this](const httplib::Request& req, httplib::Response& res) {
            try {
                // Get mode from query parameter (default to "basic")
                std::string mode = req.has_param("mode") ? req.get_param_value("mode") : "basic";
                
                json response;
                std::string latestFile;
                std::string content;
                
                // Get last used configuration
                json config;
                {
                    std::lock_guard<std::mutex> lock(configMutex);
                    config = lastConfiguration;
                }
                
                if (config.empty()) {
                    response = {
                        {"success", false},
                        {"error", "No simulation has been run yet. Please run a simulation first."}
                    };
                } else if (mode == "basic") {
                    // Basic mode results are in console output - get from process manager
                    std::string consoleOutput = processManager->getOutput();
                    if (!consoleOutput.empty()) {
                        response = {
                            {"success", true},
                            {"mode", "basic"},
                            {"hasResults", true},
                            {"content", consoleOutput},
                            {"filename", "console_output"},
                            {"timestamp", "current"}
                        };
                    } else {
                        response = {
                            {"success", true},
                            {"mode", "basic"},
                            {"hasResults", false},
                            {"message", "No simulation output available yet. Run a simulation first."}
                        };
                    }
                } else if (mode == "comparative") {
                    // Use the gameManagersFolder from the actual configuration
                    if (config.contains("gameManagersFolder")) {
                        std::string searchDir = config["gameManagersFolder"].get<std::string>();
                        latestFile = findLatestResultFile(searchDir, "comparative_results_");
                        if (!latestFile.empty()) {
                            content = readFileContent(latestFile);
                            response = {
                                {"success", true},
                                {"mode", "comparative"},
                                {"hasResults", true},
                                {"filename", std::filesystem::path(latestFile).filename()},
                                {"content", content},
                                {"timestamp", getFileTimestamp(latestFile)},
                                {"searchDirectory", searchDir}
                            };
                        } else {
                            response = {
                                {"success", true},
                                {"mode", "comparative"},
                                {"hasResults", false},
                                {"message", "No comparative results found in " + searchDir},
                                {"searchDirectory", searchDir}
                            };
                        }
                    } else {
                        response = {
                            {"success", false},
                            {"error", "No gameManagersFolder found in last configuration. Cannot determine where to look for results."}
                        };
                    }
                } else if (mode == "competition") {
                    // Use the algorithmsFolder from the actual configuration
                    if (config.contains("algorithmsFolder")) {
                        std::string searchDir = config["algorithmsFolder"].get<std::string>();
                        latestFile = findLatestResultFile(searchDir, "competition_");
                        if (!latestFile.empty()) {
                            content = readFileContent(latestFile);
                            response = {
                                {"success", true},
                                {"mode", "competition"},
                                {"hasResults", true},
                                {"filename", std::filesystem::path(latestFile).filename()},
                                {"content", content},
                                {"timestamp", getFileTimestamp(latestFile)},
                                {"searchDirectory", searchDir}
                            };
                        } else {
                            response = {
                                {"success", true},
                                {"mode", "competition"},
                                {"hasResults", false},
                                {"message", "No competition results found in " + searchDir},
                                {"searchDirectory", searchDir}
                            };
                        }
                    } else {
                        response = {
                            {"success", false},
                            {"error", "No algorithmsFolder found in last configuration. Cannot determine where to look for results."}
                        };
                    }
                } else {
                    response = {
                        {"success", false},
                        {"error", "Invalid mode: " + mode + ". Must be 'basic', 'comparative', or 'competition'"}
                    };
                }
                
                res.set_content(response.dump(), "application/json");
            } catch (const std::exception& e) {
                json error = {
                    {"success", false},
                    {"error", "Failed to discover results: " + std::string(e.what())}
                };
                res.set_content(error.dump(), "application/json");
            }
        });
    }

private:
    void serveFile(const std::string& filename, httplib::Response& res) {
        std::string fullPath = "../UserCommon/bonus/ui_server/" + filename;
        std::cout << "🔍 Attempting to serve file: " << fullPath << std::endl;
        
        std::ifstream file(fullPath);
        if (!file.is_open()) {
            res.status = 404;
            res.set_content("File not found: " + filename, "text/plain");
            std::cout << "❌ Failed to open file: " << fullPath << std::endl;
            return;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Set appropriate content type
        std::string contentType = "text/plain";
        if (filename.ends_with(".html")) {
            contentType = "text/html";
        } else if (filename.ends_with(".css")) {
            contentType = "text/css";
        } else if (filename.ends_with(".js")) {
            contentType = "application/javascript";
        }
        
        res.set_content(content, contentType);
    }

    // Helper function to find the latest result file with given prefix
    std::string findLatestResultFile(const std::string& directory, const std::string& prefix) {
        try {
            if (!std::filesystem::exists(directory)) {
                return "";
            }

            std::filesystem::file_time_type latestTime;
            std::string latestFile;
            bool foundFile = false;

            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (filename.starts_with(prefix) && filename.ends_with(".txt")) {
                        auto fileTime = entry.last_write_time();
                        if (!foundFile || fileTime > latestTime) {
                            latestTime = fileTime;
                            latestFile = entry.path().string();
                            foundFile = true;
                        }
                    }
                }
            }

            return latestFile;
        } catch (const std::exception& e) {
            std::cerr << "Error finding latest result file: " << e.what() << std::endl;
            return "";
        }
    }

    // Helper function to read file content
    std::string readFileContent(const std::string& filePath) {
        try {
            std::ifstream file(filePath);
            if (!file.is_open()) {
                return "";
            }
            
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            return content;
        } catch (const std::exception& e) {
            std::cerr << "Error reading file content: " << e.what() << std::endl;
            return "";
        }
    }

    // Helper function to get file timestamp as string
    std::string getFileTimestamp(const std::string& filePath) {
        try {
            if (!std::filesystem::exists(filePath)) {
                return "";
            }
            
            auto fileTime = std::filesystem::last_write_time(filePath);
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                fileTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
            );
            auto time_t = std::chrono::system_clock::to_time_t(sctp);
            
            std::ostringstream oss;
            oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            return oss.str();
        } catch (const std::exception& e) {
            std::cerr << "Error getting file timestamp: " << e.what() << std::endl;
            return "";
        }
    }

public:
    void start() {
        std::cout << "🚀 Starting Tank Simulator UI Server..." << std::endl;
        std::cout << "📡 Server will be available at: http://localhost:" << port << std::endl;
        std::cout << "🔗 Health check at: http://localhost:" << port << "/health" << std::endl;
        std::cout << "⏹️  Press Ctrl+C to stop the server" << std::endl;
        std::cout << std::string(50, '=') << std::endl;

        if (!server.listen("localhost", port)) {
            std::cerr << "❌ Failed to start server on port " << port << std::endl;
            std::cerr << "   Port might be already in use or access denied." << std::endl;
            exit(1);
        }
    }

    void stop() {
        server.stop();
        std::cout << "\n🛑 Server stopped." << std::endl;
    }
};

} // namespace UserCommon_318835816_211314471

int main(int argc, char* argv[]) {
    int port = 8080;
    
    // Simple argument parsing for port
    if (argc > 1) {
        try {
            port = std::stoi(argv[1]);
            if (port < 1024 || port > 65535) {
                std::cerr << "⚠️  Port must be between 1024 and 65535. Using default: 8080" << std::endl;
                port = 8080;
            }
        } catch (const std::exception&) {
            std::cerr << "⚠️  Invalid port argument. Using default: 8080" << std::endl;
            port = 8080;
        }
    }

    UserCommon_318835816_211314471::TankSimulatorUIServer server(port);
    
    try {
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "❌ Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}