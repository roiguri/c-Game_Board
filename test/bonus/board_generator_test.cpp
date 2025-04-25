#include "gtest/gtest.h"
#include "bonus/board_generator.h"
#include <fstream>
#include <algorithm>
#include <set>
#include <queue>

class BoardGeneratorTest : public ::testing::Test {
protected:
    // Helper to write a test config file
    std::string writeTestConfig(const std::string& content) {
        std::string filename = "test_config.txt";
        std::ofstream file(filename);
        file << content;
        file.close();
        return filename;
    }
    
    // Helper to check if a board has valid structure
    bool isBoardValid(const std::vector<std::string>& boardLines) {
        if (boardLines.empty()) return false;
        
        // Check dimensions line
        std::istringstream iss(boardLines[0]);
        int width, height;
        if (!(iss >> width >> height)) return false;
        
        // Check actual dimensions
        if (boardLines.size() != static_cast<size_t>(height + 1)) return false;
        
        // Check all rows have the same width
        for (size_t i = 1; i < boardLines.size(); ++i) {
            if (boardLines[i].size() != static_cast<size_t>(width)) return false;
        }
        
        return true;
    }
    
    // Helper to count occurrences of a character in the board
    int countCharInBoard(const std::vector<std::string>& boardLines, char ch) {
        int count = 0;
        
        // Start from index 1 to skip the dimensions line
        for (size_t i = 1; i < boardLines.size(); ++i) {
            count += std::count(boardLines[i].begin(), boardLines[i].end(), ch);
        }
        
        return count;
    }
    
    // Helper to check if tanks have a valid path between them
    bool hasTankPath(const std::vector<std::string>& boardLines) {
        if (boardLines.size() <= 1) return false;
        
        // Parse dimensions
        std::istringstream iss(boardLines[0]);
        int width, height;
        if (!(iss >> width >> height)) return false;
        
        // Find tank positions
        int tank1X = -1, tank1Y = -1;
        int tank2X = -1, tank2Y = -1;
        
        for (int y = 0; y < height; ++y) {
            if (y + 1 >= static_cast<int>(boardLines.size())) continue;
            
            for (int x = 0; x < width; ++x) {
                if (x >= static_cast<int>(boardLines[y + 1].size())) continue;
                
                if (boardLines[y + 1][x] == '1') {
                    tank1X = x;
                    tank1Y = y;
                } else if (boardLines[y + 1][x] == '2') {
                    tank2X = x;
                    tank2Y = y;
                }
            }
        }
        
        if (tank1X < 0 || tank2X < 0) return false;
        
        // BFS to check connectivity
        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
        std::queue<std::pair<int, int>> queue;
        
        queue.push({tank1X, tank1Y});
        visited[tank1Y][tank1X] = true;
        
        const int dx[] = {0, 1, 0, -1};
        const int dy[] = {-1, 0, 1, 0};
        
        while (!queue.empty()) {
            auto [x, y] = queue.front();
            queue.pop();
            
            if (x == tank2X && y == tank2Y) return true;
            
            for (int i = 0; i < 4; ++i) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                
                // Check bounds
                if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                
                // Skip if visited or wall
                if (visited[ny][nx] || boardLines[ny + 1][nx] == '#') continue;
                
                visited[ny][nx] = true;
                queue.push({nx, ny});
            }
        }
        
        return false;
    }
    
    // Helper to check symmetry
    bool checkSymmetry(const std::vector<std::string>& boardLines, const std::string& symmetryType) {
        if (boardLines.size() <= 1) return false;
        
        // Parse dimensions
        std::istringstream iss(boardLines[0]);
        int width, height;
        if (!(iss >> width >> height)) return false;
        
        // Skip the first line which contains dimensions
        for (int y = 0; y < height; ++y) {
            if (y + 1 >= static_cast<int>(boardLines.size())) return false;
            
            for (int x = 0; x < width; ++x) {
                if (x >= static_cast<int>(boardLines[y + 1].size())) return false;
                
                char currentCell = boardLines[y + 1][x];
                
                // Skip tanks for symmetry check
                if (currentCell == '1' || currentCell == '2') continue;
                
                if (symmetryType == "horizontal") {
                    int mirrorX = width - 1 - x;
                    // Check if mirrored cell matches
                    if (boardLines[y + 1][mirrorX] != currentCell) return false;
                }
                else if (symmetryType == "vertical") {
                    int mirrorY = height - 1 - y;
                    if (mirrorY + 1 < static_cast<int>(boardLines.size())) {
                        if (boardLines[mirrorY + 1][x] != currentCell) return false;
                    }
                }
                else if (symmetryType == "diagonal") {
                    int mirrorX = width - 1 - x;
                    int mirrorY = height - 1 - y;
                    if (mirrorY + 1 < static_cast<int>(boardLines.size())) {
                        if (boardLines[mirrorY + 1][mirrorX] != currentCell) return false;
                    }
                }
            }
        }
        
        return true;
    }
};

// Test default constructor
TEST_F(BoardGeneratorTest, DefaultConstructor) {
    BoardGenerator generator;
    const BoardConfig& config = generator.getConfig();
    
    EXPECT_EQ(config.width, 15);
    EXPECT_EQ(config.height, 10);
    EXPECT_FLOAT_EQ(config.wallDensity, 0.25f);
    EXPECT_FLOAT_EQ(config.mineDensity, 0.05f);
    EXPECT_EQ(config.symmetry, "none");
    EXPECT_NE(config.seed, -1); // Should have been replaced with a valid seed
}

// Test constructor with custom config
TEST_F(BoardGeneratorTest, CustomConfigConstructor) {
    BoardConfig config;
    config.width = 20;
    config.height = 15;
    config.wallDensity = 0.3f;
    config.mineDensity = 0.1f;
    config.symmetry = "horizontal";
    config.seed = 12345;
    
    BoardGenerator generator(config);
    const BoardConfig& retrievedConfig = generator.getConfig();
    
    EXPECT_EQ(retrievedConfig.width, 20);
    EXPECT_EQ(retrievedConfig.height, 15);
    EXPECT_FLOAT_EQ(retrievedConfig.wallDensity, 0.3f);
    EXPECT_FLOAT_EQ(retrievedConfig.mineDensity, 0.1f);
    EXPECT_EQ(retrievedConfig.symmetry, "horizontal");
    EXPECT_EQ(retrievedConfig.seed, 12345);
}

// Test loading configuration from file
TEST_F(BoardGeneratorTest, LoadConfig) {
    std::string configContent = 
        "dimensions: 25 20\n"
        "wall_density: 0.4\n"
        "mine_density: 0.08\n"
        "symmetry: vertical\n"
        "seed: 54321\n";
    
    std::string filename = writeTestConfig(configContent);
    
    BoardGenerator generator;
    EXPECT_TRUE(generator.loadConfig(filename));
    
    const BoardConfig& config = generator.getConfig();
    EXPECT_EQ(config.width, 25);
    EXPECT_EQ(config.height, 20);
    EXPECT_FLOAT_EQ(config.wallDensity, 0.4f);
    EXPECT_FLOAT_EQ(config.mineDensity, 0.08f);
    EXPECT_EQ(config.symmetry, "vertical");
    EXPECT_EQ(config.seed, 54321);
    
    // Cleanup
    std::remove(filename.c_str());
}

// Test loading invalid configuration file
TEST_F(BoardGeneratorTest, LoadInvalidConfig) {
    std::string configContent = 
        "dimensions: 3 3\n" // Too small
        "wall_density: 1.5\n" // Out of range
        "mine_density: -0.1\n" // Out of range
        "symmetry: invalid\n" // Invalid value
        "seed: abc\n"; // Not a number
    
    std::string filename = writeTestConfig(configContent);
    
    BoardGenerator generator;
    EXPECT_TRUE(generator.loadConfig(filename)); // Parsing shouldn't fail completely
    
    const BoardConfig& config = generator.getConfig();
    // Should retain default values for invalid entries
    EXPECT_EQ(config.width, 15);
    EXPECT_EQ(config.height, 10);
    EXPECT_FLOAT_EQ(config.wallDensity, 0.25f);
    EXPECT_FLOAT_EQ(config.mineDensity, 0.05f);
    EXPECT_EQ(config.symmetry, "none");
    
    // Cleanup
    std::remove(filename.c_str());
}

// Test basic board generation
TEST_F(BoardGeneratorTest, GenerateBasicBoard) {
    BoardGenerator generator;
    EXPECT_TRUE(generator.generateBoard());
    
    auto boardLines = generator.getBoardLines();
    EXPECT_TRUE(isBoardValid(boardLines));
    
    // Check for presence of required elements
    EXPECT_EQ(countCharInBoard(boardLines, '1'), 1);
    EXPECT_EQ(countCharInBoard(boardLines, '2'), 1);
    EXPECT_GT(countCharInBoard(boardLines, '#'), 4); // At least the border walls
    EXPECT_GT(countCharInBoard(boardLines, '@'), 0); // At least one mine
}

// Test board generation with fixed seed
TEST_F(BoardGeneratorTest, GenerateBoardWithSeed) {
    BoardConfig config;
    config.seed = 12345;
    
    BoardGenerator generator1(config);
    EXPECT_TRUE(generator1.generateBoard());
    auto boardLines1 = generator1.getBoardLines();
    
    BoardGenerator generator2(config);
    EXPECT_TRUE(generator2.generateBoard());
    auto boardLines2 = generator2.getBoardLines();
    
    // Both boards should be identical with the same seed
    EXPECT_EQ(boardLines1, boardLines2);
}

// Test board density parameters
TEST_F(BoardGeneratorTest, WallAndMineDensity) {
  BoardConfig config;
  config.width = 20;
  config.height = 15;
  config.wallDensity = 0.4f;
  config.mineDensity = 0.1f;
  config.seed = 12345;
  
  BoardGenerator generator(config);
  EXPECT_TRUE(generator.generateBoard());
  
  auto boardLines = generator.getBoardLines();
  EXPECT_TRUE(isBoardValid(boardLines));
  
  int wallCount = countCharInBoard(boardLines, '#');
  int mineCount = countCharInBoard(boardLines, '@');
  
  // Calculate the total board area
  int boardArea = config.width * config.height;
  
  // Area available for walls (excluding tanks)
  int availableArea = boardArea - 2; // Just subtracting the 2 tank cells
  int expectedWallsApprox = static_cast<int>(availableArea * config.wallDensity);
  
  // Allow for some variance due to connectivity checks
  EXPECT_GE(wallCount, expectedWallsApprox * 0.3);
  EXPECT_LE(wallCount, expectedWallsApprox * 1.2);
  
  // Area available for mines (excluding tanks and walls)
  int availableForMines = boardArea - 2;
  int expectedMinesApprox = static_cast<int>(availableForMines * config.mineDensity);
  
  EXPECT_GE(mineCount, expectedMinesApprox * 0.5);
  EXPECT_LE(mineCount, expectedMinesApprox * 1.5);
}

// Test horizontal symmetry
TEST_F(BoardGeneratorTest, HorizontalSymmetry) {
    BoardConfig config;
    config.symmetry = "horizontal";
    config.seed = 12345;
    
    BoardGenerator generator(config);
    EXPECT_TRUE(generator.generateBoard());
    
    auto boardLines = generator.getBoardLines();
    EXPECT_TRUE(isBoardValid(boardLines));
    EXPECT_TRUE(checkSymmetry(boardLines, "horizontal"));
}

// Test vertical symmetry
TEST_F(BoardGeneratorTest, VerticalSymmetry) {
    BoardConfig config;
    config.symmetry = "vertical";
    config.seed = 12345;
    
    BoardGenerator generator(config);
    EXPECT_TRUE(generator.generateBoard());
    
    auto boardLines = generator.getBoardLines();
    EXPECT_TRUE(isBoardValid(boardLines));
    EXPECT_TRUE(checkSymmetry(boardLines, "vertical"));
}

// Test diagonal symmetry
TEST_F(BoardGeneratorTest, DiagonalSymmetry) {
    BoardConfig config;
    config.symmetry = "diagonal";
    config.seed = 12345;
    
    BoardGenerator generator(config);
    EXPECT_TRUE(generator.generateBoard());
    
    auto boardLines = generator.getBoardLines();
    EXPECT_TRUE(isBoardValid(boardLines));
    EXPECT_TRUE(checkSymmetry(boardLines, "diagonal"));
}

// Test connectivity validation
TEST_F(BoardGeneratorTest, TankConnectivity) {
    BoardGenerator generator;
    EXPECT_TRUE(generator.generateBoard());
    
    auto boardLines = generator.getBoardLines();
    EXPECT_TRUE(hasTankPath(boardLines));
}

// Test saving to file
TEST_F(BoardGeneratorTest, SaveToFile) {
    BoardConfig config;
    config.seed = 12345;
    
    BoardGenerator generator(config);
    EXPECT_TRUE(generator.generateBoard());
    
    std::string outputPath = "test_board_output.txt";
    EXPECT_TRUE(generator.saveToFile(outputPath));
    
    // Verify file exists and has correct content
    std::ifstream file(outputPath);
    EXPECT_TRUE(file.good());
    
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    std::string expected = "20 15\n";
    EXPECT_TRUE(isBoardValid(lines));
    EXPECT_EQ(countCharInBoard(lines, '1'), 1);
    EXPECT_EQ(countCharInBoard(lines, '2'), 1);
    
    // Cleanup
    file.close();
    std::remove(outputPath.c_str());
}

// Test that tanks are not trapped
TEST_F(BoardGeneratorTest, TanksNotTrapped) {
    for (int i = 0; i < 5; ++i) { // Try multiple times with different seeds
        BoardConfig config;
        config.seed = i;
        config.wallDensity = 0.7f; // High wall density to stress test
        
        BoardGenerator generator(config);
        EXPECT_TRUE(generator.generateBoard());
        
        auto boardLines = generator.getBoardLines();
        EXPECT_TRUE(hasTankPath(boardLines));
    }
}