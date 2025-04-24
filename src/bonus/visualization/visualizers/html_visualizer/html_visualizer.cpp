#include "bonus/visualization/visualizers/html_visualizer/html_visualizer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

HTMLVisualizer::HTMLVisualizer(const std::string& templatePath) {
    if (!templatePath.empty()) {
        m_templatePath = templatePath;
    } else {
        // Try to find templates directory
        m_templatePath = findTemplatesDirectory();
        if (m_templatePath.empty()) {
            std::cerr << "HTMLVisualizer: Could not locate templates directory. "
                      << "HTML visualization may not work correctly." << std::endl;
        }
    }
}

HTMLVisualizer::~HTMLVisualizer() {
    // Cleanup not needed
}

void HTMLVisualizer::processSnapshot(const GameSnapshot& snapshot) {
    m_snapshots.push_back(snapshot);
}

void HTMLVisualizer::clear() {
    m_snapshots.clear();
}

bool HTMLVisualizer::generateOutput(const std::string& outputPath) {
    if (m_snapshots.empty()) {
        std::cerr << "HTMLVisualizer: No snapshots to visualize" << std::endl;
        return false;
    }
    
    // Load templates
    std::string htmlTemplate = loadTemplate("visualizer.html");
    std::string cssTemplate = loadTemplate("visualizer.css");
    std::string jsTemplate = loadTemplate("visualizer.js");
    
    if (htmlTemplate.empty() || cssTemplate.empty() || jsTemplate.empty()) {
        std::cerr << "HTMLVisualizer: Failed to load one or more templates" << std::endl;
        return false;
    }
    
    // Generate game data JavaScript
    std::string gameDataJs = generateGameDataJS();
    
    // Replace template placeholders
    size_t cssPos = htmlTemplate.find("{{STYLE_CONTENT}}");
    if (cssPos != std::string::npos) {
        htmlTemplate.replace(cssPos, 17, cssTemplate);
    }
    
    size_t dataPos = htmlTemplate.find("{{GAME_DATA}}");
    if (dataPos != std::string::npos) {
        htmlTemplate.replace(dataPos, 13, gameDataJs);
    }
    
    size_t jsPos = htmlTemplate.find("{{JS_CONTENT}}");
    if (jsPos != std::string::npos) {
        htmlTemplate.replace(jsPos, 14, jsTemplate);
    }
    
    // Insert total steps
    size_t stepsPos = htmlTemplate.find("{{TOTAL_STEPS}}");
    if (stepsPos != std::string::npos) {
        std::string totalSteps = std::to_string(m_snapshots.size() > 0 ? 
            m_snapshots.back().getStepNumber() : 0);
        htmlTemplate.replace(stepsPos, 15, totalSteps);
    }
    
    // Write output file
    std::string outputFilePath = outputPath + ".html";
    try {
        std::ofstream outputFile(outputFilePath);
        if (!outputFile.is_open()) {
            std::cerr << "HTMLVisualizer: Failed to open file for writing: " 
                      << outputFilePath << std::endl;
            return false;
        }
        
        outputFile << htmlTemplate;
        outputFile.close();
        
        std::cout << "HTMLVisualizer: Generated visualization at " 
                  << outputFilePath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "HTMLVisualizer: Error writing output file: " 
                  << e.what() << std::endl;
        return false;
    }
}

bool HTMLVisualizer::supportsLiveMode() const {
    return false; // Basic HTML visualizer doesn't support live mode
}

void HTMLVisualizer::displayCurrentState() {
    // No-op since this visualizer doesn't support live mode
}

std::string HTMLVisualizer::loadTemplate(const std::string& templateName) const {
    if (m_templatePath.empty()) {
        std::cerr << "HTMLVisualizer: Template path not set" << std::endl;
        return "";
    }
    
    std::filesystem::path filePath = m_templatePath / templateName;
    
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "HTMLVisualizer: Could not open template file: " 
                      << filePath << std::endl;
            return "";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    } catch (const std::exception& e) {
        std::cerr << "HTMLVisualizer: Error reading template file " 
                  << filePath << ": " << e.what() << std::endl;
        return "";
    }
}

std::string HTMLVisualizer::generateGameDataJS() const {
    std::stringstream js;
    
    js << "const gameData = {\n";
    js << "    snapshots: [";
    
    for (size_t i = 0; i < m_snapshots.size(); ++i) {
        const auto& snapshot = m_snapshots[i];
        
        if (i > 0) {
            js << ",";
        }
        
        js << "\n        {\n";
        js << "            step: " << snapshot.getStepNumber() << ",\n";
        js << "            countdown: " << snapshot.getCountdown() << ",\n";
        js << "            message: \"" << snapshot.getMessage() << "\",\n";
        
        // Board state
        js << "            board: [\n";
        const auto& boardState = snapshot.getBoardState();
        for (size_t y = 0; y < boardState.size(); ++y) {
            js << "                [";
            for (size_t x = 0; x < boardState[y].size(); ++x) {
                if (x > 0) {
                    js << ", ";
                }
                js << static_cast<int>(boardState[y][x]);
            }
            js << "]";
            if (y < boardState.size() - 1) {
                js << ",";
            }
            js << "\n";
        }
        js << "            ],\n";

        // Wall health
        js << "            wallHealth: [\n";
        const auto& wallHealth = snapshot.getWallHealth();
        size_t wallCount = 0;
        for (const auto& [position, health] : wallHealth) {
            if (wallCount > 0) {
                js << ",\n";
            }
            js << "                {\n";
            js << "                    x: " << position.getX() << ",\n";
            js << "                    y: " << position.getY() << ",\n";
            js << "                    health: " << health << "\n";
            js << "                }";
            wallCount++;
        }
        if (wallCount > 0) {
            js << "\n";
        }
        js << "            ],\n";
        
        // Tanks
        js << "            tanks: [\n";
        const auto& tanks = snapshot.getTanks();
        for (size_t t = 0; t < tanks.size(); ++t) {
            const auto& tank = tanks[t];
            
            js << "                {\n";
            js << "                    playerId: " << tank.playerId << ",\n";
            js << "                    position: { x: " << tank.position.getX() 
               << ", y: " << tank.position.getY() << " },\n";
            js << "                    direction: " << static_cast<int>(tank.direction) << ",\n";
            js << "                    remainingShells: " << tank.remainingShells << ",\n";
            js << "                    destroyed: " << (tank.destroyed ? "true" : "false") << "\n";
            js << "                }";
            
            if (t < tanks.size() - 1) {
                js << ",";
            }
            js << "\n";
        }
        js << "            ],\n";
        
        // Shells
        js << "            shells: [\n";
        const auto& shells = snapshot.getShells();
        for (size_t s = 0; s < shells.size(); ++s) {
            const auto& shell = shells[s];
            
            js << "                {\n";
            js << "                    playerId: " << shell.playerId << ",\n";
            js << "                    position: { x: " << shell.position.getX() 
               << ", y: " << shell.position.getY() << " },\n";
            js << "                    direction: " << static_cast<int>(shell.direction) << ",\n";
            js << "                    destroyed: " << (shell.destroyed ? "true" : "false") << "\n";
            js << "                }";
            
            if (s < shells.size() - 1) {
                js << ",";
            }
            js << "\n";
        }
        js << "            ]\n";
        js << "        }";
    }
    
    js << "\n    ],\n";
    
    // Add cell type enum mapping for reference
    js << "    cellTypes: {\n";
    js << "        EMPTY: 0,\n";
    js << "        WALL: 1,\n";
    js << "        MINE: 2,\n";
    js << "        TANK1: 3,\n";
    js << "        TANK2: 4\n";
    js << "    },\n";
    
    // Add direction enum mapping for reference
    js << "    directions: {\n";
    js << "        UP: 0,\n";
    js << "        UP_RIGHT: 1,\n";
    js << "        RIGHT: 2,\n";
    js << "        DOWN_RIGHT: 3,\n";
    js << "        DOWN: 4,\n";
    js << "        DOWN_LEFT: 5,\n";
    js << "        LEFT: 6,\n";
    js << "        UP_LEFT: 7\n";
    js << "    }\n";
    js << "};";
    
    return js.str();
}

std::filesystem::path HTMLVisualizer::getExecutableDirectory() {
    std::filesystem::path executablePath;
    
#ifdef _WIN32
    char path[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, path, MAX_PATH);
    executablePath = path;
#else
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count != -1) {
        path[count] = '\0';
        executablePath = path;
    }
#endif
    
    return executablePath.parent_path();
}

std::filesystem::path HTMLVisualizer::findTemplatesDirectory() const {
    std::vector<std::filesystem::path> searchPaths;
    
    // Current directory
    searchPaths.push_back(std::filesystem::current_path() / "templates");
    
    // Executable directory
    std::filesystem::path execDir = getExecutableDirectory();
    searchPaths.push_back(execDir / "templates");
    
    // Relative to executable
    searchPaths.push_back(execDir / "../share/tankbattle/templates");
    searchPaths.push_back(execDir / "../resources/templates");
    
    // Specific to this visualizer
    searchPaths.push_back(std::filesystem::current_path() / 
        "bonus/visualization/visualizers/html_visualizer/templates");
    searchPaths.push_back(execDir / 
        "bonus/visualization/visualizers/html_visualizer/templates");
    
    // Check all search paths
    for (const auto& path : searchPaths) {
        if (std::filesystem::exists(path / "visualizer.html") &&
            std::filesystem::exists(path / "visualizer.css") &&
            std::filesystem::exists(path / "visualizer.js")) {
            return path;
        }
    }
    
    return {};
}