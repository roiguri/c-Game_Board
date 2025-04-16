#include "gtest/gtest.h"
#include "bonus/visualization/visualizers/html_visualizer/html_visualizer.h"
#include "game_board.h"
#include "tank.h"
#include "shell.h"
#include <fstream>
#include <filesystem>

class HTMLVisualizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test directory
        testDir = std::filesystem::temp_directory_path() / "html_visualizer_test";
        std::filesystem::create_directories(testDir);
        
        // Create template files in a test templates directory
        templatesDir = testDir / "templates";
        std::filesystem::create_directories(templatesDir);
        
        // Copy template content from strings to files
        createTemplateFile("visualizer.html", htmlTemplate);
        createTemplateFile("visualizer.css", cssTemplate);
        createTemplateFile("visualizer.js", jsTemplate);
        
        // Create the visualizer with our test templates
        visualizer = new HTMLVisualizer(templatesDir.string());
    }
    
    void TearDown() override {
        delete visualizer;
        
        // Clean up test directory
        try {
            std::filesystem::remove_all(testDir);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Failed to clean up test directory: " << e.what() << std::endl;
        }
    }
    
    void createTemplateFile(const std::string& name, const std::string& content) {
        std::ofstream file(templatesDir / name);
        file << content;
        file.close();
    }
    
    // Create a simple game snapshot for testing
    GameSnapshot createTestSnapshot(int step, int countdown = -1) {
        // Create a simple 3x3 board
        GameBoard board(3, 3);
        std::vector<std::string> boardLines = {
            "###",
            "#1#",
            "#2#"
        };
        std::vector<std::string> errors;
        board.initialize(boardLines, errors);

        // Damage one wall to test wall health
        board.damageWall(Point(0, 0));
        
        // Create two tanks
        std::vector<Tank> tanks;
        tanks.emplace_back(1, Point(1, 1), Direction::Right);
        tanks.emplace_back(2, Point(1, 2), Direction::Left);
        
        // Create a shell
        std::vector<Shell> shells;
        shells.emplace_back(1, Point(2, 1), Direction::Right);
        
        return GameSnapshot(step, board, tanks, shells, countdown, "Test snapshot " + std::to_string(step));
    }
    
    std::filesystem::path testDir;
    std::filesystem::path templatesDir;
    HTMLVisualizer* visualizer;
    
    // Simplified templates for testing
    const std::string htmlTemplate = R"(
<!DOCTYPE html>
<html>
<head>
    <style>{{STYLE_CONTENT}}</style>
</head>
<body>
    <div id="gameBoard"></div>
    <div id="stepDisplay">Step: 0 / {{TOTAL_STEPS}}</div>
    <div id="countdownDisplay"></div>
    <script>
    {{GAME_DATA}}
    {{JS_CONTENT}}
    </script>
</body>
</html>
    )";
    
    const std::string cssTemplate = R"(
      body { font-family: sans-serif; }
      .wall-health-1 { background-color: #bcaaa4; }
      .wall-health-2 { background-color: #8d6e63; }
      .countdown-display { font-weight: bold; }
          )";
        
    const std::string jsTemplate = R"(
        console.log("Game data loaded with " + gameData.snapshots.length + " snapshots");
    )";
};

TEST_F(HTMLVisualizerTest, EmptyVisualizerGeneratesNoOutput) {
    std::string outputPath = (testDir / "output").string();
    EXPECT_FALSE(visualizer->generateOutput(outputPath));
}

TEST_F(HTMLVisualizerTest, SingleSnapshotGeneratesOutput) {
    visualizer->processSnapshot(createTestSnapshot(0));
    
    std::string outputPath = (testDir / "output").string();
    EXPECT_TRUE(visualizer->generateOutput(outputPath));
    
    std::string outputFile = outputPath + ".html";
    EXPECT_TRUE(std::filesystem::exists(outputFile));
    
    // Read the output file
    std::ifstream file(outputFile);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Check that the content contains our template parts and game data
    EXPECT_TRUE(content.find("body { font-family: sans-serif; }") != std::string::npos);
    EXPECT_TRUE(content.find("Game data loaded with") != std::string::npos);
    EXPECT_TRUE(content.find("gameData") != std::string::npos);
    EXPECT_TRUE(content.find("Test snapshot 0") != std::string::npos);

    // Check for wall health styles
    EXPECT_TRUE(content.find("wall-health-1") != std::string::npos);
    EXPECT_TRUE(content.find("wall-health-2") != std::string::npos);
    
    // Check for countdown display element
    EXPECT_TRUE(content.find("countdownDisplay") != std::string::npos);
}

TEST_F(HTMLVisualizerTest, SnapshotWithCountdownGeneratesOutput) {
  // Create a snapshot with active countdown
  visualizer->processSnapshot(createTestSnapshot(0, 15));
  
  std::string outputPath = (testDir / "output").string();
  EXPECT_TRUE(visualizer->generateOutput(outputPath));
  
  std::string outputFile = outputPath + ".html";
  EXPECT_TRUE(std::filesystem::exists(outputFile));
  
  // Read the output file
  std::ifstream file(outputFile);
  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  file.close();
  
  // Check that the content contains countdown data
  EXPECT_TRUE(content.find("countdown: 15") != std::string::npos);
}

TEST_F(HTMLVisualizerTest, MultipleSnapshotsGenerateOutput) {
    for (int i = 0; i < 5; i++) {
        visualizer->processSnapshot(createTestSnapshot(i));
    }
    
    std::string outputPath = (testDir / "output").string();
    EXPECT_TRUE(visualizer->generateOutput(outputPath));
    
    std::string outputFile = outputPath + ".html";
    EXPECT_TRUE(std::filesystem::exists(outputFile));
    
    // Read the output file
    std::ifstream file(outputFile);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Check that the content contains data for all snapshots
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(content.find("Test snapshot " + std::to_string(i)) != std::string::npos);
    }

    // Check for countdown data in appropriate snapshots
    EXPECT_TRUE(content.find("countdown: 39") != std::string::npos);  // 40 - 1
    EXPECT_TRUE(content.find("countdown: 37") != std::string::npos);  // 40 - 3
}

TEST_F(HTMLVisualizerTest, ClearRemovesAllSnapshots) {
    for (int i = 0; i < 3; i++) {
        visualizer->processSnapshot(createTestSnapshot(i));
    }
    
    visualizer->clear();
    
    std::string outputPath = (testDir / "output").string();
    EXPECT_FALSE(visualizer->generateOutput(outputPath));
}

TEST_F(HTMLVisualizerTest, NoLiveModeSupport) {
    EXPECT_FALSE(visualizer->supportsLiveMode());
    
    // This should do nothing and not crash
    visualizer->displayCurrentState();
}