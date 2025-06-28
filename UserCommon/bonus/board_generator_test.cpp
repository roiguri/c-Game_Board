#include <algorithm>
#include <fstream>
#include <queue>
#include <set>

#include "gtest/gtest.h"

#include "bonus/board_generator.h"

using namespace UserCommon_098765432_123456789;

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
    
    // Helper to extract board dimensions from header
    std::pair<int, int> extractDimensions(const std::vector<std::string>& boardLines) {
        int width = -1, height = -1;
        for (const auto& line : boardLines) {
            if (line.rfind("Rows = ", 0) == 0) {
                height = std::stoi(line.substr(7));
            } else if (line.rfind("Cols = ", 0) == 0) {
                width = std::stoi(line.substr(7));
            }
        }
        return {width, height};
    }
    
    // Helper to get only the board rows (skip 5-line header)
    std::vector<std::string> getBoardRows(const std::vector<std::string>& boardLines) {
        if (boardLines.size() <= 5) return {};
        return std::vector<std::string>(boardLines.begin() + 5, boardLines.end());
    }
    
    // Helper to check if a board has valid structure
    bool isBoardValid(const std::vector<std::string>& boardLines) {
        auto [width, height] = extractDimensions(boardLines);
        auto rows = getBoardRows(boardLines);
        if (width < 0 || height < 0) return false;
        if ((int)rows.size() != height) return false;
        for (const auto& row : rows) {
            if ((int)row.size() != width) return false;
        }
        return true;
    }
    
    // Helper to count occurrences of a character in the board
    int countCharInBoard(const std::vector<std::string>& boardLines, char ch) {
        auto rows = getBoardRows(boardLines);
        int count = 0;
        for (const auto& row : rows) {
            count += std::count(row.begin(), row.end(), ch);
        }
        return count;
    }
    
    // Helper to check if tanks have a valid path between them
    bool hasTankPath(const std::vector<std::string>& boardLines) {
        auto [width, height] = extractDimensions(boardLines);
        auto rows = getBoardRows(boardLines);
        if (width < 0 || height < 0 || (int)rows.size() != height) return false;
        int tank1X = -1, tank1Y = -1;
        int tank2X = -1, tank2Y = -1;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (rows[y][x] == '1') { tank1X = x; tank1Y = y; }
                else if (rows[y][x] == '2') { tank2X = x; tank2Y = y; }
            }
        }
        if (tank1X < 0 || tank2X < 0) return false;
        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
        std::queue<std::pair<int, int>> queue;
        queue.push({tank1X, tank1Y});
        visited[tank1Y][tank1X] = true;
        const int dx[] = {0, 1, 0, -1};
        const int dy[] = {-1, 0, 1, 0};
        while (!queue.empty()) {
            auto [x, y] = queue.front(); queue.pop();
            if (x == tank2X && y == tank2Y) return true;
            for (int i = 0; i < 4; ++i) {
                int nx = x + dx[i]; int ny = y + dy[i];
                if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                if (visited[ny][nx] || rows[ny][nx] == '#') continue;
                visited[ny][nx] = true;
                queue.push({nx, ny});
            }
        }
        return false;
    }
    
    // Helper to check symmetry
    bool checkSymmetry(const std::vector<std::string>& boardLines, const std::string& symmetryType) {
        auto [width, height] = extractDimensions(boardLines);
        auto rows = getBoardRows(boardLines);
        if (width < 0 || height < 0 || (int)rows.size() != height) return false;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                char currentCell = rows[y][x];
                if (currentCell == '1' || currentCell == '2') continue;
                if (symmetryType == "horizontal") {
                    int mirrorX = width - 1 - x;
                    if (rows[y][mirrorX] != currentCell) return false;
                } else if (symmetryType == "vertical") {
                    int mirrorY = height - 1 - y;
                    if (rows[mirrorY][x] != currentCell) return false;
                } else if (symmetryType == "diagonal") {
                    int mirrorX = width - 1 - x;
                    int mirrorY = height - 1 - y;
                    if (rows[mirrorY][mirrorX] != currentCell) return false;
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
    EXPECT_EQ(config.maxSteps, 1000);
    EXPECT_EQ(config.numShells, 10);
    EXPECT_EQ(config.mapName, "Generated Map");
    EXPECT_EQ(config.numTanksPerPlayer, 1);
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
    config.maxSteps = 500;
    config.numShells = 7;
    config.mapName = "My Custom Map";
    config.numTanksPerPlayer = 3;
    
    BoardGenerator generator(config);
    const BoardConfig& retrievedConfig = generator.getConfig();
    
    EXPECT_EQ(retrievedConfig.width, 20);
    EXPECT_EQ(retrievedConfig.height, 15);
    EXPECT_FLOAT_EQ(retrievedConfig.wallDensity, 0.3f);
    EXPECT_FLOAT_EQ(retrievedConfig.mineDensity, 0.1f);
    EXPECT_EQ(retrievedConfig.symmetry, "horizontal");
    EXPECT_EQ(retrievedConfig.seed, 12345);
    EXPECT_EQ(retrievedConfig.maxSteps, 500);
    EXPECT_EQ(retrievedConfig.numShells, 7);
    EXPECT_EQ(retrievedConfig.mapName, "My Custom Map");
    EXPECT_EQ(retrievedConfig.numTanksPerPlayer, 3);
}

// Test loading configuration from file
TEST_F(BoardGeneratorTest, LoadConfig) {
    std::string configContent = 
        "dimensions: 25 20\n"
        "wall_density: 0.4\n"
        "mine_density: 0.08\n"
        "symmetry: vertical\n"
        "seed: 54321\n"
        "max_steps: 777\n"
        "num_shells: 5\n"
        "map_name: Test Map\n"
        "num_tanks_per_player: 2\n";
    
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
    EXPECT_EQ(config.maxSteps, 777);
    EXPECT_EQ(config.numShells, 5);
    EXPECT_EQ(config.mapName, "Test Map");
    EXPECT_EQ(config.numTanksPerPlayer, 2);
    
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
        "seed: abc\n" // Not a number
        "max_steps: notanumber\n" // Invalid
        "num_shells: -5\n" // Invalid (negative)
        "map_name: \n" // Empty
        "num_tanks_per_player: 0\n"; // Invalid (too low)
    
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
    EXPECT_NE(config.seed, -1);
    EXPECT_EQ(config.maxSteps, 1000);
    EXPECT_EQ(config.numShells, 10);
    EXPECT_EQ(config.mapName, "Generated Map");
    EXPECT_EQ(config.numTanksPerPlayer, 1);
    
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

// Test saving to file
TEST_F(BoardGeneratorTest, SaveToFile) {
    BoardConfig config;
    config.seed = 12345;
    config.maxSteps = 222;
    config.numShells = 8;
    config.mapName = "File Output Map";
    config.width = 20;
    config.height = 15;
    config.wallDensity = 0.3f;
    config.mineDensity = 0.1f;
    config.symmetry = "horizontal";
    config.numTanksPerPlayer = 2;

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
    // Check header lines
    ASSERT_GE(lines.size(), 5);
    EXPECT_EQ(lines[0], "File Output Map");
    EXPECT_EQ(lines[1], "MaxSteps = 222");
    EXPECT_EQ(lines[2], "NumShells = 8");
    EXPECT_EQ(lines[3], "Rows = 15");
    EXPECT_EQ(lines[4], "Cols = 20");
    // Check board validity and tanks
    EXPECT_TRUE(isBoardValid(lines));
    EXPECT_GE(countCharInBoard(lines, '1'), 1);
    EXPECT_GE(countCharInBoard(lines, '2'), 1);

    // Cleanup
    file.close();
    std::remove(outputPath.c_str());
}