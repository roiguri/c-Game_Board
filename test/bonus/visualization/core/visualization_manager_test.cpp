#include "gtest/gtest.h"
#include "bonus/visualization/core/visualization_manager.h"
#include "game_board.h"
#include "tank.h"
#include "shell.h"
#include <memory>

// Mock visualizer for testing
class MockVisualizer : public VisualizerBase {
public:
    MockVisualizer(bool supportsLive = false)
        : m_supportsLive(supportsLive),
          m_processedSnapshots(0),
          m_clearCalled(false),
          m_generateOutputCalled(false),
          m_displayCurrentStateCalled(false),
          m_lastOutputPath("") {
    }
    
    void processSnapshot(const GameSnapshot& snapshot) override {
        m_processedSnapshots++;
        m_lastSnapshot = snapshot;
    }
    
    void clear() override {
        m_clearCalled = true;
        m_processedSnapshots = 0;
    }
    
    bool generateOutput(const std::string& outputPath) override {
        m_generateOutputCalled = true;
        m_lastOutputPath = outputPath;
        return true;
    }
    
    bool supportsLiveMode() const override {
        return m_supportsLive;
    }
    
    void displayCurrentState() override {
        m_displayCurrentStateCalled = true;
    }
    
    // Test accessors
    int getProcessedSnapshotCount() const { return m_processedSnapshots; }
    bool wasCleared() const { return m_clearCalled; }
    bool wasGenerateOutputCalled() const { return m_generateOutputCalled; }
    bool wasDisplayCurrentStateCalled() const { return m_displayCurrentStateCalled; }
    std::string getLastOutputPath() const { return m_lastOutputPath; }
    GameSnapshot getLastSnapshot() const { return m_lastSnapshot; }
    
private:
    bool m_supportsLive;
    int m_processedSnapshots;
    bool m_clearCalled;
    bool m_generateOutputCalled;
    bool m_displayCurrentStateCalled;
    std::string m_lastOutputPath;
    GameSnapshot m_lastSnapshot;
};

// Mock visualizer that fails on generateOutput
class FailingMockVisualizer : public MockVisualizer {
public:
    FailingMockVisualizer() : MockVisualizer() {}
    
    bool generateOutput(const std::string& outputPath) override {
        MockVisualizer::generateOutput(outputPath);
        return false;  // Always fail
    }
};

class VisualizationManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test board
        board = GameBoard(5, 5);
        
        std::vector<std::string> boardData = {
            "#####",
            "#1 2#",
            "#   #",
            "# @ #",
            "#####"
        };
        
        std::vector<std::string> errors;
        board.initialize(boardData, errors);

        // Damage one wall to test wall health
        board.damageWall(Point(0, 0));
        
        // Create test tanks
        tanks.push_back(Tank(1, Point(1, 1), Direction::Right));
        tanks.push_back(Tank(2, Point(3, 1), Direction::Left));
        
        // Create test shells
        shells.push_back(Shell(1, Point(2, 2), Direction::Right));
        
        // Create the visualization manager
        manager = std::make_unique<VisualizationManager>();
    }
    
    GameBoard board;
    std::vector<Tank> tanks;
    std::vector<Shell> shells;
    std::unique_ptr<VisualizationManager> manager;
};

TEST_F(VisualizationManagerTest, DefaultConstructor) {
    EXPECT_EQ(manager->getVisualizerCount(), 0);
    EXPECT_FALSE(manager->isLiveVisualizationEnabled());
}

TEST_F(VisualizationManagerTest, AddVisualizer) {
    // Initially, no visualizers
    EXPECT_EQ(manager->getVisualizerCount(), 0);
    
    // Add a visualizer
    manager->addVisualizer(std::make_unique<MockVisualizer>());
    EXPECT_EQ(manager->getVisualizerCount(), 1);
    
    // Add another visualizer
    manager->addVisualizer(std::make_unique<MockVisualizer>());
    EXPECT_EQ(manager->getVisualizerCount(), 2);
    
    // Adding a null visualizer should not change the count
    manager->addVisualizer(nullptr);
    EXPECT_EQ(manager->getVisualizerCount(), 2);
}

TEST_F(VisualizationManagerTest, CaptureGameState) {
    // Add a mock visualizer
    auto mockVisualizer = new MockVisualizer();
    manager->addVisualizer(std::unique_ptr<VisualizerBase>(mockVisualizer));
    
    // Capture a game state
    manager->captureGameState(42, board, tanks, shells, -1, "Test message");
    
    // Verify that the snapshot was processed
    EXPECT_EQ(mockVisualizer->getProcessedSnapshotCount(), 1);
    
    // Verify the snapshot properties
    GameSnapshot lastSnapshot = mockVisualizer->getLastSnapshot();
    EXPECT_EQ(lastSnapshot.getStepNumber(), 42);
    EXPECT_EQ(lastSnapshot.getMessage(), "Test message");
    EXPECT_EQ(lastSnapshot.getTanks().size(), 2);
    EXPECT_EQ(lastSnapshot.getShells().size(), 1);
    EXPECT_EQ(lastSnapshot.getCountdown(), -1);

    // Verify wall health was captured
    const auto& wallHealth = lastSnapshot.getWallHealth();
    EXPECT_FALSE(wallHealth.empty());
    auto it = wallHealth.find(Point(0, 0));
    EXPECT_NE(it, wallHealth.end());
    EXPECT_EQ(it->second, 1);  // Damaged wall
    
    // Capture another game state
    manager->captureGameState(43, board, tanks, shells, 30, "Next step");
    
    // Verify the snapshot count
    EXPECT_EQ(mockVisualizer->getProcessedSnapshotCount(), 2);
    
    // Verify the updated snapshot properties
    lastSnapshot = mockVisualizer->getLastSnapshot();
    EXPECT_EQ(lastSnapshot.getStepNumber(), 43);
    EXPECT_EQ(lastSnapshot.getMessage(), "Next step");
    EXPECT_EQ(lastSnapshot.getCountdown(), 30);
}

TEST_F(VisualizationManagerTest, CaptureGameStateMultipleVisualizers) {
    // Add two mock visualizers
    auto mockVisualizer1 = new MockVisualizer();
    auto mockVisualizer2 = new MockVisualizer();
    
    manager->addVisualizer(std::unique_ptr<VisualizerBase>(mockVisualizer1));
    manager->addVisualizer(std::unique_ptr<VisualizerBase>(mockVisualizer2));
    
    // Capture a game state
    manager->captureGameState(42, board, tanks, shells, 25, "Test message");
    
    // Verify that both received the snapshot
    EXPECT_EQ(mockVisualizer1->getProcessedSnapshotCount(), 1);
    EXPECT_EQ(mockVisualizer2->getProcessedSnapshotCount(), 1);
    
    // Verify they both have the same snapshot
    EXPECT_EQ(mockVisualizer1->getLastSnapshot().getStepNumber(), 
              mockVisualizer2->getLastSnapshot().getStepNumber());
}

TEST_F(VisualizationManagerTest, GenerateOutputs) {
    // Add two mock visualizers
    auto mockVisualizer1 = new MockVisualizer();
    auto mockVisualizer2 = new MockVisualizer();
    
    manager->addVisualizer(std::unique_ptr<VisualizerBase>(mockVisualizer1));
    manager->addVisualizer(std::unique_ptr<VisualizerBase>(mockVisualizer2));
    
    // Generate outputs
    EXPECT_TRUE(manager->generateOutputs("test_output"));
    
    // Verify both visualizers were called
    EXPECT_TRUE(mockVisualizer1->wasGenerateOutputCalled());
    EXPECT_TRUE(mockVisualizer2->wasGenerateOutputCalled());
    
    // Verify the output paths include suffixes for multiple visualizers
    EXPECT_EQ(mockVisualizer1->getLastOutputPath(), "test_output_1");
    EXPECT_EQ(mockVisualizer2->getLastOutputPath(), "test_output_2");
}

TEST_F(VisualizationManagerTest, GenerateOutputsWithFailure) {
    // Add one normal and one failing visualizer
    auto mockVisualizer = new MockVisualizer();
    auto failingVisualizer = new FailingMockVisualizer();
    
    manager->addVisualizer(std::unique_ptr<VisualizerBase>(mockVisualizer));
    manager->addVisualizer(std::unique_ptr<VisualizerBase>(failingVisualizer));
    
    // Generate outputs should fail
    EXPECT_FALSE(manager->generateOutputs("test_output"));
    
    // Both visualizers should still be called
    EXPECT_TRUE(mockVisualizer->wasGenerateOutputCalled());
    EXPECT_TRUE(failingVisualizer->wasGenerateOutputCalled());
}

TEST_F(VisualizationManagerTest, GenerateOutputsSingleVisualizer) {
    // Add a single visualizer
    auto mockVisualizer = new MockVisualizer();
    manager->addVisualizer(std::unique_ptr<VisualizerBase>(mockVisualizer));
    
    // Generate outputs
    EXPECT_TRUE(manager->generateOutputs("test_output"));
    
    // Verify visualizer was called
    EXPECT_TRUE(mockVisualizer->wasGenerateOutputCalled());
    
    // With a single visualizer, no suffix should be added
    EXPECT_EQ(mockVisualizer->getLastOutputPath(), "test_output");
}

TEST_F(VisualizationManagerTest, LiveVisualization) {
    // Add visualizers with different live mode support
    auto nonLiveVisualizer = new MockVisualizer(false);
    auto liveVisualizer = new MockVisualizer(true);
    
    manager->addVisualizer(std::unique_ptr<VisualizerBase>(nonLiveVisualizer));
    manager->addVisualizer(std::unique_ptr<VisualizerBase>(liveVisualizer));
    
    // Initially live visualization is disabled
    EXPECT_FALSE(manager->isLiveVisualizationEnabled());
    
    // Enable live visualization
    manager->setLiveVisualizationEnabled(true);
    EXPECT_TRUE(manager->isLiveVisualizationEnabled());
    
    // Display current state
    manager->displayCurrentState();
    
    // Only the visualizer that supports live mode should be called
    EXPECT_FALSE(nonLiveVisualizer->wasDisplayCurrentStateCalled());
    EXPECT_TRUE(liveVisualizer->wasDisplayCurrentStateCalled());
    
    // Disable live visualization
    manager->setLiveVisualizationEnabled(false);
    EXPECT_FALSE(manager->isLiveVisualizationEnabled());
    
    // Reset flags
    nonLiveVisualizer = new MockVisualizer(false);
    liveVisualizer = new MockVisualizer(true);
    
    // Display current state - should not call any visualizers
    manager->displayCurrentState();
    EXPECT_FALSE(nonLiveVisualizer->wasDisplayCurrentStateCalled());
    EXPECT_FALSE(liveVisualizer->wasDisplayCurrentStateCalled());
}

TEST_F(VisualizationManagerTest, NoLiveVisualizers) {
    // Add visualizers with no live mode support
    manager->addVisualizer(std::make_unique<MockVisualizer>(false));
    manager->addVisualizer(std::make_unique<MockVisualizer>(false));
    
    // Capture stderr to verify warning
    testing::internal::CaptureStderr();
    
    // Enable live visualization
    manager->setLiveVisualizationEnabled(true);
    
    // Check the warning message
    std::string error = testing::internal::GetCapturedStderr();
    EXPECT_NE(error.find("Warning"), std::string::npos);
    EXPECT_NE(error.find("no visualizers support live mode"), std::string::npos);
}