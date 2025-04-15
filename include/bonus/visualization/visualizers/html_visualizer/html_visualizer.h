#pragma once

#include "bonus/visualization/core/visualizer_base.h"
#include <vector>
#include <string>
#include <filesystem>

/**
 * @brief HTML-based visualizer that generates an interactive web visualization
 * 
 * This visualizer creates a standalone HTML file with embedded JavaScript
 * that allows users to view and step through the game simulation.
 * It uses external template files for the HTML, CSS, and JavaScript parts.
 */
class HTMLVisualizer : public VisualizerBase {
public:
    /**
     * @brief Constructor
     * 
     * @param templatePath Optional path to template directory
     *        If not provided, will use a default relative path
     */
    explicit HTMLVisualizer(const std::string& templatePath = "");
    
    /**
     * @brief Destructor
     */
    ~HTMLVisualizer() override;
    
    /**
     * @brief Process a new game state snapshot
     * 
     * @param snapshot The game state to process
     */
    void processSnapshot(const GameSnapshot& snapshot) override;
    
    /**
     * @brief Clear all stored snapshots
     */
    void clear() override;
    
    /**
     * @brief Generate HTML visualization file
     * 
     * @param outputPath Base path for output files (without extension)
     * @return true if output was successfully generated, false otherwise
     */
    bool generateOutput(const std::string& outputPath) override;
    
    /**
     * @brief Check if visualizer supports live mode
     * 
     * @return false since basic HTML visualizer doesn't support live mode
     */
    bool supportsLiveMode() const override;
    
    /**
     * @brief No-op since this visualizer doesn't support live mode
     */
    void displayCurrentState() override;
    
private:
    std::vector<GameSnapshot> m_snapshots;
    std::filesystem::path m_templatePath;
    
    /**
     * @brief Load template content from a file
     * 
     * @param templateName Name of the template file (without path)
     * @return Content of the template file or empty string if not found
     */
    std::string loadTemplate(const std::string& templateName) const;
    
    /**
     * @brief Generate JavaScript with game data
     * 
     * @return JavaScript code defining the game data as string
     */
    std::string generateGameDataJS() const;
    
    /**
     * @brief Find the executable directory to locate template files
     * 
     * @return Path to the directory containing the executable
     */
    static std::filesystem::path getExecutableDirectory();
    
    /**
     * @brief Attempt to find the templates directory
     * 
     * @return Path to the templates directory or empty if not found
     */
    std::filesystem::path findTemplatesDirectory() const;
};