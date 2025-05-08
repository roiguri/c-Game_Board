#include "gtest/gtest.h"
#include "satellite_view_impl.h"
#include "game_board.h"
#include "objects/tank.h"
#include "objects/shell.h"
#include "utils/point.h"
#include <memory>
#include <vector>
#include <string>

TEST(SatelliteViewImplTest, BasicBoardView) {
    // Create a 3x3 board: wall, mine, empty
    std::vector<std::string> boardLines = {
        "#@ ",
        "  1",
        "2 *"
    };
    GameBoard board(3, 3);
    std::vector<std::string> errors;
    std::vector<std::pair<int, Point>> tankPositions;
    board.initialize(boardLines, errors, tankPositions);

    // Create tanks
    auto tank1 = std::make_shared<Tank>(1, Point(2, 1), Direction::Right);
    auto tank2 = std::make_shared<Tank>(2, Point(0, 2), Direction::Left);
    std::vector<std::shared_ptr<const Tank>> tanks = {tank1, tank2};

    // Create shell
    auto shell = std::make_shared<Shell>(1, Point(2, 2), Direction::Up);
    std::vector<std::shared_ptr<const Shell>> shells = {shell};

    // Current tank position (should be marked as '%')
    Point currentTankPos(2, 1);

    SatelliteViewImpl view(board, tanks, shells, currentTankPos);

    // Check wall
    EXPECT_EQ(view.getObjectAt(0, 0), '#');
    // Check mine
    EXPECT_EQ(view.getObjectAt(1, 0), '@');
    // Check empty
    EXPECT_EQ(view.getObjectAt(2, 0), ' ');
    // Check tank 1 (should be '%')
    EXPECT_EQ(view.getObjectAt(2, 1), '%');
    // Check tank 2
    EXPECT_EQ(view.getObjectAt(0, 2), '2');
    // Check shell
    EXPECT_EQ(view.getObjectAt(2, 2), '*');
}

TEST(SatelliteViewImplTest, OutOfRangeCoordinates) {
    std::vector<std::string> boardLines = {
        "#@ ",
        "   ",
        "   "
    };
    GameBoard board(3, 3);
    std::vector<std::string> errors;
    std::vector<std::pair<int, Point>> tankPositions;
    board.initialize(boardLines, errors, tankPositions);
    std::vector<std::shared_ptr<const Tank>> tanks;
    std::vector<std::shared_ptr<const Shell>> shells;
    Point currentTankPos(1, 1);
    SatelliteViewImpl view(board, tanks, shells, currentTankPos);
    // Out of range (negative and too large)
    EXPECT_EQ(view.getObjectAt(-1, 0), '&');
    EXPECT_EQ(view.getObjectAt(0, -1), '&');
    EXPECT_EQ(view.getObjectAt(3, 0), '&');
    EXPECT_EQ(view.getObjectAt(0, 3), '&');
}

TEST(SatelliteViewImplTest, ShellOverMineReturnsShell) {
    std::vector<std::string> boardLines = {
        "#@ ",
        "   ",
        "   "
    };
    GameBoard board(3, 3);
    std::vector<std::string> errors;
    std::vector<std::pair<int, Point>> tankPositions;
    board.initialize(boardLines, errors, tankPositions);
    std::vector<std::shared_ptr<const Tank>> tanks;
    // Place a shell at (1,0) where there is a mine
    auto shell = std::make_shared<Shell>(1, Point(1, 0), Direction::Down);
    std::vector<std::shared_ptr<const Shell>> shells = {shell};
    Point currentTankPos(2, 2);
    SatelliteViewImpl view(board, tanks, shells, currentTankPos);
    // Should return shell character, not mine
    EXPECT_EQ(view.getObjectAt(1, 0), '*');
} 