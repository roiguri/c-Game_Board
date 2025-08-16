#include "gtest/gtest.h"
#include "satellite_view_impl.h"
#include "game_board.h"
#include "objects/tank.h"
#include "objects/shell.h"
#include "utils/point.h"
#include <vector>
#include <string>

using namespace UserCommon_098765432_123456789;

TEST(SatelliteViewImplTest, BasicBoardView) {
    // Create a 3x3 board: wall, mine, empty
    std::vector<std::string> boardLines = {
        "#@ ",
        "  1",
        "2 *"
    };
    GameBoard board(3, 3);
    std::vector<std::pair<int, Point>> tankPositions;
    board.initialize(boardLines, tankPositions);

    // Create tanks
    std::vector<Tank> tanks = {
        Tank(1, Point(2, 1), Direction::Right),
        Tank(2, Point(0, 2), Direction::Left)
    };

    // Create shells
    std::vector<Shell> shells = {
        Shell(1, Point(2, 2), Direction::Up)
    };

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
    std::vector<std::pair<int, Point>> tankPositions;
    board.initialize(boardLines, tankPositions);
    std::vector<Tank> tanks;
    std::vector<Shell> shells;
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
    std::vector<std::pair<int, Point>> tankPositions;
    board.initialize(boardLines, tankPositions);
    std::vector<Tank> tanks;
    // Place a shell at (1,0) where there is a mine
    std::vector<Shell> shells = {
        Shell(1, Point(1, 0), Direction::Down)
    };
    Point currentTankPos(2, 2);
    SatelliteViewImpl view(board, tanks, shells, currentTankPos);
    // Should return shell character, not mine
    EXPECT_EQ(view.getObjectAt(1, 0), '*');
}

TEST(SatelliteViewImplTest, IgnoresDestroyedTanksAndShells) {
    std::vector<std::string> boardLines = {
        "   ",
        "   ",
        "   "
    };
    GameBoard board(3, 3);
    std::vector<std::pair<int, Point>> tankPositions;
    board.initialize(boardLines, tankPositions);
    std::vector<Tank> tanks = {
        Tank(1, Point(1, 1), Direction::Right),
        Tank(2, Point(2, 2), Direction::Left)
    };
    std::vector<Shell> shells = {
        Shell(1, Point(0, 0), Direction::Up),
        Shell(2, Point(2, 2), Direction::Down)
    };
    // Destroy tank 2 and shell 2
    tanks[1].destroy();
    shells[1].destroy();

    Point currentTankPos(0, 0);
    SatelliteViewImpl view(board, tanks, shells, currentTankPos);

    // Only tank 1 and shell 1 should be visible
    EXPECT_EQ(view.getObjectAt(1, 1), '1'); // tank 1
    EXPECT_EQ(view.getObjectAt(2, 2), ' '); // destroyed tank 2 and shell 2 ignored
    EXPECT_EQ(view.getObjectAt(0, 0), '%'); // current tank position (shell 1 is at 0,0 but current tank takes precedence)
}

TEST(SatelliteViewImplTest, DestroyedShellOverMineReturnsMine) {
    std::vector<std::string> boardLines = {
        "@  ",
        "   ",
        "   "
    };
    GameBoard board(3, 3);
    std::vector<std::pair<int, Point>> tankPositions;
    board.initialize(boardLines, tankPositions);
    std::vector<Tank> tanks;
    std::vector<Shell> shells = {
        Shell(1, Point(0, 0), Direction::Down)
    };
    shells[0].destroy();
    Point currentTankPos(2, 2);
    SatelliteViewImpl view(board, tanks, shells, currentTankPos);
    // Should return mine character, not shell
    EXPECT_EQ(view.getObjectAt(0, 0), '@');
} 