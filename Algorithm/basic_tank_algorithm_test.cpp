#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "basic_tank_algorithm.h"
#include "UserCommon/game_board.h"
#include "UserCommon/objects/tank.h"
#include "utils/point.h"
#include "battle_info_impl.h"
#include <vector>
#include <string>

using namespace UserCommon_318835816_211314471;
using ::testing::UnorderedElementsAreArray;

namespace Algorithm_318835816_211314471 {

class BasicTankAlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Default: tank at (1,1), direction Right
        board = makeBoard({
            "#####",
            "#%  #",
            "#   #",
            "#   #",
            "#####"
        });
        algo = std::make_unique<BasicTankAlgorithm>(1, 0);
    }
    GameBoard board;
    std::unique_ptr<BasicTankAlgorithm> algo;

    // Helper to call private getLineOfSightDirection
    std::optional<Direction> getLineOfSightDirection(const Point& from, const Point& to) {
        return algo->getLineOfSightDirection(from, to);
    }
    // Helper to call private checkLineOfSightInDirection
    bool checkLineOfSightInDirection(const Point& from, const Point& to, Direction dir) {
        return algo->checkLineOfSightInDirection(from, to, dir);
    }
    // Helper to call private isTankAtPosition
    bool isTankAtPosition(const Point& position) {
        return algo->isTankAtPosition(position);
    }
    static GameBoard makeBoard(const std::vector<std::string>& lines) {
        GameBoard board(lines[0].size(), lines.size());
        std::vector<std::pair<int, Point>> tankPositions;
        board.initialize(lines, tankPositions);
        return board;
    }
    static BattleInfoImpl makeBattleInfo(const GameBoard& board, const std::vector<Point>& enemyTanks, const std::vector<Point>& friendlyTanks, const std::vector<Point>& shells) {
        BattleInfoImpl info(board.getWidth(), board.getHeight());
        for (size_t x = 0; x < board.getWidth(); ++x) {
            for (size_t y = 0; y < board.getHeight(); ++y) {
                info.setCellType(x, y, board.getCellType(x, y));
            }
        }
        for (const auto& p : enemyTanks) info.addEnemyTankPosition(p);
        for (const auto& p : friendlyTanks) info.addFriendlyTankPosition(p);
        for (const auto& p : shells) info.addShellPosition(p);
        return info;
    }
    // Helpers to access private members for testing
    const GameBoard& getGameBoard() const { return algo->m_gameBoard; }
    const std::vector<Point>& getEnemyTanks() const { return algo->m_enemyTanks; }
    const std::vector<Point>& getFriendlyTanks() const { return algo->m_friendlyTanks; }
    const std::vector<Point>& getShells() const { return algo->m_shells; }
    bool isInDangerFromShells() { return algo->isInDangerFromShells(); }
    bool isInDangerFromShells(const Point& pos) { return algo->isInDangerFromShells(pos); }
    bool isPositionSafe(const Point& pos) { return algo->isPositionSafe(pos); }
    std::vector<Point> getSafePositions() { return algo->getSafePositions(); }
    // New helpers for private/protected methods
    BasicTankAlgorithm::SafeMoveOption getSafeMoveOption(const Point& p) { return algo->getSafeMoveOption(p); }
    std::vector<BasicTankAlgorithm::SafeMoveOption> getSafeMoveOptions(const std::vector<Point>& positions) { return algo->getSafeMoveOptions(positions); }
    ActionRequest getActionToSafePosition() { return algo->getActionToSafePosition(); }
    // Helper for canShootEnemy
    bool getCanShootEnemy() { return algo->canShootEnemy(); }
    // Helper to set the direction of the algorithm directly
    void setAlgoDirection(Direction dir) {
        algo->m_trackedDirection = dir;
    }
    void setAlgoPosition(const Point& pos) {
        algo->m_trackedPosition = pos;
    }
    // Helper to get private state for testing
    Point getAlgoPosition() const { return algo->m_trackedPosition; }
    Direction getAlgoDirection() const { return algo->m_trackedDirection; }
    int getAlgoShells() const { return algo->m_trackedShells; }
    int getAlgoCooldown() const { return algo->m_trackedCooldown; }
    void setAlgoShells(int shells) { algo->m_trackedShells = shells; }
    void setAlgoCooldown(int cooldown) { algo->m_trackedCooldown = cooldown; }
    void callUpdateState(ActionRequest action) { algo->updateState(action); }
    // Helper to access protected static getRotationToDirection
    static ActionRequest callGetRotationToDirection(Direction current, Direction target) {
        return BasicTankAlgorithm::getRotationToDirection(current, target);
    }
};

TEST_F(BasicTankAlgorithmTest, UpdateBattleInfo_UpdatesGameBoardAndObjects) {
    // Create a board with a wall and a mine
    GameBoard board = makeBoard({
        "#####",
        "#@  #",
        "# # #",
        "#   #",
        "#####"
    });
    std::vector<Point> enemyTanks = {Point(1, 1)};
    std::vector<Point> friendlyTanks = {Point(3, 3)};
    std::vector<Point> shells = {Point(2, 2)};
    BattleInfoImpl info = makeBattleInfo(board, enemyTanks, friendlyTanks, shells);
    algo->updateBattleInfo(info);

    // Check board cell types
    EXPECT_EQ(getGameBoard().getCellType(1, 1), GameBoard::CellType::Mine);
    EXPECT_EQ(getGameBoard().getCellType(2, 2), GameBoard::CellType::Wall);
    // Check enemy tanks
    ASSERT_EQ(getEnemyTanks().size(), 1u);
    EXPECT_EQ(getEnemyTanks()[0], Point(1, 1));
    // Check friendly tanks
    ASSERT_EQ(getFriendlyTanks().size(), 1u);
    EXPECT_EQ(getFriendlyTanks()[0], Point(3, 3));
    // Check shells
    ASSERT_EQ(getShells().size(), 1u);
    EXPECT_EQ(getShells()[0], Point(2, 2));
} 

TEST_F(BasicTankAlgorithmTest, GetLineOfSightDirection_DirectLine) {
    GameBoard board = makeBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    // Horizontal
    EXPECT_EQ(getLineOfSightDirection(Point(1,2), Point(3,2)), Direction::Right);
    // Vertical
    EXPECT_EQ(getLineOfSightDirection(Point(2,1), Point(2,3)), Direction::Down);
    // Diagonal
    EXPECT_EQ(getLineOfSightDirection(Point(1,1), Point(3,3)), Direction::DownRight);
}

TEST_F(BasicTankAlgorithmTest, GetLineOfSightDirection_BlockedLine) {
    GameBoard board = makeBoard({
        "#####",
        "# # #",
        "### #",
        "# # #",
        "#####"
    });
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    // Blocked horizontal
    EXPECT_EQ(getLineOfSightDirection(Point(1,1), Point(3,1)), std::nullopt);
    // Blocked vertical
    EXPECT_EQ(getLineOfSightDirection(Point(1,1), Point(1,3)), std::nullopt);
    // Blocked diagonal
    EXPECT_EQ(getLineOfSightDirection(Point(1,1), Point(3,3)), std::nullopt);
}

TEST_F(BasicTankAlgorithmTest, CheckLineOfSightInDirection_CorrectDirections) {
    GameBoard board = makeBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    // Right
    EXPECT_TRUE(checkLineOfSightInDirection(Point(1,2), Point(3,2), Direction::Right));
    // Down
    EXPECT_TRUE(checkLineOfSightInDirection(Point(2,1), Point(2,3), Direction::Down));
    // DownRight
    EXPECT_TRUE(checkLineOfSightInDirection(Point(1,1), Point(3,3), Direction::DownRight));
    // Wrong direction
    EXPECT_FALSE(checkLineOfSightInDirection(Point(1,1), Point(3,3), Direction::Up));
}

TEST_F(BasicTankAlgorithmTest, CheckLineOfSightInDirection_TankBlocking) {
    GameBoard board = makeBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    // Place enemy tank blocking horizontal path at (2,2)
    std::vector<Point> enemyTanks = {Point(2, 2)};
    BattleInfoImpl info = makeBattleInfo(board, enemyTanks, {}, {});
    algo->updateBattleInfo(info);
    
    // Should not find line of sight through tank
    EXPECT_EQ(getLineOfSightDirection(Point(1,2), Point(3,2)), std::nullopt);
    
    // Tank at (2,2) also blocks diagonal path from (1,1) to (3,3)
    EXPECT_EQ(getLineOfSightDirection(Point(1,1), Point(3,3)), std::nullopt);
    
    // But should find paths that don't go through the tank
    EXPECT_EQ(getLineOfSightDirection(Point(1,1), Point(1,3)), Direction::Down);
}

TEST_F(BasicTankAlgorithmTest, CheckLineOfSightInDirection_FriendlyTankBlocking) {
    GameBoard board = makeBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    // Place friendly tank at (2,2) - blocking path from (1,2) to (3,2)
    std::vector<Point> friendlyTanks = {Point(2, 2)};
    BattleInfoImpl info = makeBattleInfo(board, {}, friendlyTanks, {});
    algo->updateBattleInfo(info);
    
    // Line of sight should be blocked by friendly tank
    EXPECT_FALSE(checkLineOfSightInDirection(Point(1,2), Point(3,2), Direction::Right));
}

TEST_F(BasicTankAlgorithmTest, CheckLineOfSightInDirection_TankNotInPath) {
    GameBoard board = makeBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    // Place enemy tank at (1,1) - not blocking path from (1,2) to (3,2)
    std::vector<Point> enemyTanks = {Point(1, 1)};
    BattleInfoImpl info = makeBattleInfo(board, enemyTanks, {}, {});
    algo->updateBattleInfo(info);
    
    // Line of sight should not be blocked
    EXPECT_TRUE(checkLineOfSightInDirection(Point(1,2), Point(3,2), Direction::Right));
}

TEST_F(BasicTankAlgorithmTest, CheckLineOfSightInDirection_TankAtDestination) {
    GameBoard board = makeBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    // Place enemy tank at destination (3,2)
    std::vector<Point> enemyTanks = {Point(3, 2)};
    BattleInfoImpl info = makeBattleInfo(board, enemyTanks, {}, {});
    algo->updateBattleInfo(info);
    
    // Line of sight should reach the tank at destination
    EXPECT_TRUE(checkLineOfSightInDirection(Point(1,2), Point(3,2), Direction::Right));
}

TEST_F(BasicTankAlgorithmTest, CheckLineOfSightInDirection_TankBehindDestination) {
    GameBoard board = makeBoard({
        "######",
        "#    #",
        "#    #",
        "#    #",
        "######"
    });
    // Place enemy tank at (4,2) - behind destination (3,2)
    std::vector<Point> enemyTanks = {Point(4, 2)};
    BattleInfoImpl info = makeBattleInfo(board, enemyTanks, {}, {});
    algo->updateBattleInfo(info);
    
    // Line of sight should reach destination before being blocked
    EXPECT_TRUE(checkLineOfSightInDirection(Point(1,2), Point(3,2), Direction::Right));
}

TEST_F(BasicTankAlgorithmTest, IsInDangerFromShells_NoShells) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    EXPECT_FALSE(isInDangerFromShells());
}

TEST_F(BasicTankAlgorithmTest, IsInDangerFromShells_ShellWithLineOfSightAndClose) {
    // Tank at (1,1), shell at (1,4), vertical line, within 3 steps (wraps)
    std::vector<Point> shells = {Point(1, 4)};
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, shells);
    algo->updateBattleInfo(info);
    EXPECT_TRUE(isInDangerFromShells());
}

TEST_F(BasicTankAlgorithmTest, IsInDangerFromShells_ShellWithLineOfSightButTooFar) {
    GameBoard gameBoard = makeBoard({
        "######",
        "#%   #",
        "#    #",
        "#    #",
        "######"
    });
    std::vector<Point> shells = {Point(4, 4)};
    BattleInfoImpl info = makeBattleInfo(gameBoard, {}, {}, shells);
    algo->updateBattleInfo(info);
    EXPECT_FALSE(isInDangerFromShells());
}

TEST_F(BasicTankAlgorithmTest, IsInDangerFromShells_ShellBlockedByWall) {
    // Place a wall between shell and tank
    GameBoard walledBoard = makeBoard({
        "#####",
        "#   #",
        "# # #",
        "#   #",
        "#####"
    });
    std::vector<Point> shells = {Point(3, 3)};
    BattleInfoImpl info = makeBattleInfo(walledBoard, {}, {}, shells);
    algo->updateBattleInfo(info);
    EXPECT_FALSE(isInDangerFromShells());
}

TEST_F(BasicTankAlgorithmTest, IsInDangerFromShells_MultipleShells) {
    // One shell is dangerous, one is not
    std::vector<Point> shells = {Point(1, 4), Point(3, 3)};
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, shells);
    algo->updateBattleInfo(info);
    EXPECT_TRUE(isInDangerFromShells());
}

TEST_F(BasicTankAlgorithmTest, IsInDangerFromShells_Point_NoShells) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    EXPECT_FALSE(isInDangerFromShells(Point(1, 1)));
}

TEST_F(BasicTankAlgorithmTest, IsInDangerFromShells_Point_ShellWithLineOfSightAndClose) {
    std::vector<Point> shells = {Point(1, 4)};
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, shells);
    algo->updateBattleInfo(info);
    EXPECT_TRUE(isInDangerFromShells(Point(1, 1)));
}

TEST_F(BasicTankAlgorithmTest, IsInDangerFromShells_Point_ShellWithLineOfSightButTooFar) {
    GameBoard gameBoard = makeBoard({
        "######",
        "#1   #",
        "#    #",
        "#    #",
        "######"
    });
    std::vector<Point> shells = {Point(5, 5)};
    BattleInfoImpl info = makeBattleInfo(gameBoard, {}, {}, shells);
    algo->updateBattleInfo(info);
    EXPECT_FALSE(isInDangerFromShells(Point(1, 1)));
}

TEST_F(BasicTankAlgorithmTest, IsInDangerFromShells_Point_ShellBlockedByWall) {
    GameBoard walledBoard = makeBoard({
        "#####",
        "#   #",
        "# # #",
        "#   #",
        "#####"
    });
    std::vector<Point> shells = {Point(3, 3)};
    BattleInfoImpl info = makeBattleInfo(walledBoard, {}, {}, shells);
    algo->updateBattleInfo(info);
    EXPECT_FALSE(isInDangerFromShells(Point(1, 1)));
}

TEST_F(BasicTankAlgorithmTest, IsInDangerFromShells_Point_MultipleShells) {
    std::vector<Point> shells = {Point(1, 4), Point(3, 3)};
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, shells);
    algo->updateBattleInfo(info);
    EXPECT_TRUE(isInDangerFromShells(Point(1, 1)));
}

TEST_F(BasicTankAlgorithmTest, IsPositionSafe_SafePosition) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    EXPECT_TRUE(isPositionSafe(Point(2, 2)));
}

TEST_F(BasicTankAlgorithmTest, IsPositionSafe_Wall) {
    GameBoard walledBoard = makeBoard({
        "#####",
        "#   #",
        "# # #",
        "#   #",
        "#####"
    });
    BattleInfoImpl info = makeBattleInfo(walledBoard, {}, {}, {});
    algo->updateBattleInfo(info);
    EXPECT_FALSE(isPositionSafe(Point(2, 2)));
}

TEST_F(BasicTankAlgorithmTest, IsPositionSafe_Mine) {
    GameBoard mineBoard = makeBoard({
        "#####",
        "#   #",
        "# @ #",
        "#   #",
        "#####"
    });
    BattleInfoImpl info = makeBattleInfo(mineBoard, {}, {}, {});
    algo->updateBattleInfo(info);
    EXPECT_FALSE(isPositionSafe(Point(2, 2)));
}

TEST_F(BasicTankAlgorithmTest, IsPositionSafe_EnemyTank) {
    std::vector<Point> enemyTanks = {Point(2, 2)};
    BattleInfoImpl info = makeBattleInfo(board, enemyTanks, {}, {});
    algo->updateBattleInfo(info);
    EXPECT_FALSE(isPositionSafe(Point(2, 2)));
}

TEST_F(BasicTankAlgorithmTest, IsPositionSafe_FriendlyTank) {
    std::vector<Point> friendlyTanks = {Point(2, 2)};
    BattleInfoImpl info = makeBattleInfo(board, {}, friendlyTanks, {});
    algo->updateBattleInfo(info);
    EXPECT_FALSE(isPositionSafe(Point(2, 2)));
}

TEST_F(BasicTankAlgorithmTest, IsPositionSafe_ShellDanger) {
    std::vector<Point> shells = {Point(2, 4)};
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, shells);
    algo->updateBattleInfo(info);
    EXPECT_FALSE(isPositionSafe(Point(2, 2)));
}

TEST_F(BasicTankAlgorithmTest, GetSafePositions_AllSafe) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoPosition(Point(2, 2));
    auto safe = getSafePositions();
    std::vector<Point> expected = {
        Point(1,1), Point(2,1), Point(3,1),
        Point(1,2),             Point(3,2),
        Point(1,3), Point(2,3), Point(3,3)
    };
    EXPECT_THAT(safe, UnorderedElementsAreArray(expected));
}

TEST_F(BasicTankAlgorithmTest, GetSafePositions_SomeWalls) {
    GameBoard walledBoard = makeBoard({
        "#####",
        "# # #",
        "## ##",
        "# # #",
        "#####"
    });
    BattleInfoImpl info = makeBattleInfo(walledBoard, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoPosition(Point(2, 2));
    auto safe = getSafePositions();
    // Only non-wall adjacent positions should be returned
    std::vector<Point> expected = {Point(1,1), Point(3,3), Point(1,3), Point(3,1)};
    EXPECT_THAT(safe, UnorderedElementsAreArray(expected));
}

TEST_F(BasicTankAlgorithmTest, GetSafePositions_SomeMines) {
    GameBoard mineBoard = makeBoard({
        "#####",
        "# @ #",
        "# @ #",
        "#   #",
        "#####"
    });
    BattleInfoImpl info = makeBattleInfo(mineBoard, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoPosition(Point(1, 1));
    auto safe = getSafePositions();
    // Only (1,2) is safe
    std::vector<Point> expected = {Point(1,2)};
    EXPECT_THAT(safe, UnorderedElementsAreArray(expected));
}

TEST_F(BasicTankAlgorithmTest, GetSafePositions_SomeTanks) {
    std::vector<Point> enemyTanks = {Point(1,1), Point(3,3)};
    BattleInfoImpl info = makeBattleInfo(board, enemyTanks, {}, {});
    algo->updateBattleInfo(info);
    setAlgoPosition(Point(2, 2));
    auto safe = getSafePositions();
    std::vector<Point> expected = {Point(2,1), Point(3,1), Point(1,2), Point(3,2), Point(1,3), Point(2,3)};
    EXPECT_THAT(safe, UnorderedElementsAreArray(expected));
}

TEST_F(BasicTankAlgorithmTest, GetSafePositions_ShellDanger) {
    std::vector<Point> shells = {Point(2, 4)};
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, shells);
    algo->updateBattleInfo(info);
    setAlgoPosition(Point(2, 2));
    auto safe = getSafePositions();
    std::vector<Point> expected = {Point(1,1), Point(1,2), Point(3,1), Point(3,2)};
    EXPECT_THAT(safe, UnorderedElementsAreArray(expected));
}

TEST_F(BasicTankAlgorithmTest, GetSafeMoveOption_AlreadySafe) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoPosition(Point(2, 2));
    auto opt = getSafeMoveOption(Point(2, 2));
    EXPECT_EQ(opt.action, ActionRequest::DoNothing);
    EXPECT_EQ(opt.cost, 0);
}

TEST_F(BasicTankAlgorithmTest, GetSafeMoveOption_Forward) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Up);
    setAlgoPosition(Point(2, 2));
    auto opt = getSafeMoveOption(Point(2, 1));
    EXPECT_EQ(opt.action, ActionRequest::MoveForward);
    EXPECT_EQ(opt.cost, 1);
}

TEST_F(BasicTankAlgorithmTest, GetSafeMoveOption_RotateLeft) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Right);
    setAlgoPosition(Point(2, 2));
    auto opt = getSafeMoveOption(Point(2, 1));
    EXPECT_EQ(opt.action, ActionRequest::RotateLeft90);
    EXPECT_GT(opt.cost, 1);
}

TEST_F(BasicTankAlgorithmTest, GetSafeMoveOption_RotateRight) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Left);
    setAlgoPosition(Point(2, 2));
    auto opt = getSafeMoveOption(Point(2, 1));
    EXPECT_EQ(opt.action, ActionRequest::RotateRight90);
    EXPECT_GT(opt.cost, 1);
}

TEST_F(BasicTankAlgorithmTest, GetSafeMoveOptions_Multiple) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Up);
    setAlgoPosition(Point(2, 2));
    std::vector<Point> positions = {Point(2, 1), Point(3, 2)};
    auto opts = getSafeMoveOptions(positions);
    ASSERT_EQ(opts.size(), 2u);
    EXPECT_EQ(opts[0].action, ActionRequest::MoveForward);
    EXPECT_EQ(opts[1].action, ActionRequest::RotateRight90);
}

TEST_F(BasicTankAlgorithmTest, GetActionToSafePosition_Forward) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Up);
    setAlgoPosition(Point(2, 2));
    EXPECT_EQ(getSafeMoveOption(Point(2, 1)).action, ActionRequest::MoveForward);
    EXPECT_EQ(getActionToSafePosition(), ActionRequest::MoveForward);
}

TEST_F(BasicTankAlgorithmTest, GetActionToSafePosition_MinimalRotation) {
    GameBoard board = makeBoard({
        "#####",
        "#####",
        "#  ##",
        "### #",
        "#####"
    });
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Up);
    setAlgoPosition(Point(2, 2));
    EXPECT_EQ(getActionToSafePosition(), ActionRequest::RotateLeft90);
}

TEST_F(BasicTankAlgorithmTest, GetActionToSafePosition_NoSafe) {
    GameBoard board = makeBoard({
        "#####",
        "#####",
        "## ##",
        "#####",
        "#####"
    });
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Up);
    setAlgoPosition(Point(2, 2));
    EXPECT_EQ(getActionToSafePosition(), ActionRequest::DoNothing);
}

TEST_F(BasicTankAlgorithmTest, GetActionToSafePosition_ChooseSafeFromShell) {
    GameBoard board = makeBoard({
        "##**#",
        "##  #",
        "##  #",
        "#  ##",
        "#####"
    });
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {Point(2,0), Point(2,1)});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Up);
    setAlgoPosition(Point(2, 3));
    EXPECT_EQ(getActionToSafePosition(), ActionRequest::RotateLeft90);
}

TEST_F(BasicTankAlgorithmTest, CanShootEnemy_EnemyInCurrentDirection) {
    // Tank at (2,2) facing Up, enemy at (2,1)
    BattleInfoImpl info = makeBattleInfo(board, {Point(2, 1)}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Up);
    setAlgoPosition(Point(2, 2));
    EXPECT_TRUE(getCanShootEnemy());
}

TEST_F(BasicTankAlgorithmTest, CanShootEnemy_EnemyNotInCurrentDirection) {
    // Tank at (2,2) facing Up, enemy at (3,2) (to the right)
    BattleInfoImpl info = makeBattleInfo(board, {Point(3, 2)}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Up);
    setAlgoPosition(Point(2, 2));
    EXPECT_FALSE(getCanShootEnemy());
}

TEST_F(BasicTankAlgorithmTest, CanShootEnemy_EnemyBlockedByWall) {
    // Tank at (2,2) facing Up, enemy at (2,1), wall at (2,1)
    GameBoard walledBoard = makeBoard({
        "#####",
        "# # #",
        "#   #",
        "#   #",
        "#####"
    });
    BattleInfoImpl info = makeBattleInfo(walledBoard, {Point(1, 3)}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Up);
    setAlgoPosition(Point(1, 1));
    EXPECT_FALSE(getCanShootEnemy());
}

TEST_F(BasicTankAlgorithmTest, CanShootEnemy_MultipleEnemies_OneInDirection) {
    // Tank at (2,2) facing Up, enemies at (2,1) and (3,2)
    BattleInfoImpl info = makeBattleInfo(board, {Point(2, 1), Point(3, 2)}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Up);
    setAlgoPosition(Point(2, 2));
    EXPECT_TRUE(getCanShootEnemy());
}

TEST_F(BasicTankAlgorithmTest, CanShootEnemy_NoEnemies) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Up);
    setAlgoPosition(Point(2, 2));
    EXPECT_FALSE(getCanShootEnemy());
}

TEST_F(BasicTankAlgorithmTest, UpdateState_MoveForward) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoPosition(Point(1, 1));
    setAlgoDirection(Direction::Right);
    callUpdateState(ActionRequest::MoveForward);
    EXPECT_EQ(getAlgoPosition(), Point(2, 1));
}

TEST_F(BasicTankAlgorithmTest, UpdateState_RotateLeft90) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Up);
    callUpdateState(ActionRequest::RotateLeft90);
    EXPECT_EQ(getAlgoDirection(), Direction::Left);
}

TEST_F(BasicTankAlgorithmTest, UpdateState_RotateRight90) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoDirection(Direction::Up);
    callUpdateState(ActionRequest::RotateRight90);
    EXPECT_EQ(getAlgoDirection(), Direction::Right);
}

TEST_F(BasicTankAlgorithmTest, UpdateState_Shoot) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoShells(3);
    setAlgoCooldown(0);
    callUpdateState(ActionRequest::Shoot);
    EXPECT_EQ(getAlgoShells(), 2);
    EXPECT_EQ(getAlgoCooldown(), Tank::SHOOT_COOLDOWN);
}

TEST_F(BasicTankAlgorithmTest, UpdateState_DoNothing) {
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    setAlgoPosition(Point(1, 1));
    setAlgoDirection(Direction::Up);
    setAlgoShells(2);
    setAlgoCooldown(1);
    callUpdateState(ActionRequest::DoNothing);
    EXPECT_EQ(getAlgoPosition(), Point(1, 1));
    EXPECT_EQ(getAlgoDirection(), Direction::Up);
    EXPECT_EQ(getAlgoShells(), 2);
    EXPECT_EQ(getAlgoCooldown(), 0);
}

TEST_F(BasicTankAlgorithmTest, GetRotationToDirection_NoRotation) {
    EXPECT_EQ(callGetRotationToDirection(Direction::Up, Direction::Up), ActionRequest::DoNothing);
}

TEST_F(BasicTankAlgorithmTest, GetRotationToDirection_45DegreeRight) {
    EXPECT_EQ(callGetRotationToDirection(Direction::Up, Direction::UpRight), ActionRequest::RotateRight45);
}

TEST_F(BasicTankAlgorithmTest, GetRotationToDirection_45DegreeLeft) {
    EXPECT_EQ(callGetRotationToDirection(Direction::Up, Direction::UpLeft), ActionRequest::RotateLeft45);
}

TEST_F(BasicTankAlgorithmTest, GetRotationToDirection_90DegreeRight) {
    EXPECT_EQ(callGetRotationToDirection(Direction::Up, Direction::Right), ActionRequest::RotateRight90);
}

TEST_F(BasicTankAlgorithmTest, GetRotationToDirection_90DegreeLeft) {
    EXPECT_EQ(callGetRotationToDirection(Direction::Up, Direction::Left), ActionRequest::RotateLeft90);
}

TEST_F(BasicTankAlgorithmTest, GetRotationToDirection_FallbackShortest) {
    // 180 degree turn: Up to Down (should pick right or left, both are 4 steps)
    ActionRequest result = callGetRotationToDirection(Direction::Up, Direction::Down);
    EXPECT_TRUE(result == ActionRequest::RotateRight90 || result == ActionRequest::RotateLeft90);
}

TEST_F(BasicTankAlgorithmTest, IsTankAtPosition_EnemyTank) {
    GameBoard board = makeBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    std::vector<Point> enemyTanks = {Point(2, 2), Point(1, 3)};
    BattleInfoImpl info = makeBattleInfo(board, enemyTanks, {}, {});
    algo->updateBattleInfo(info);
    
    EXPECT_TRUE(isTankAtPosition(Point(2, 2)));
    EXPECT_TRUE(isTankAtPosition(Point(1, 3)));
    EXPECT_FALSE(isTankAtPosition(Point(3, 3)));
}

TEST_F(BasicTankAlgorithmTest, IsTankAtPosition_FriendlyTank) {
    GameBoard board = makeBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    std::vector<Point> friendlyTanks = {Point(2, 2), Point(3, 1)};
    BattleInfoImpl info = makeBattleInfo(board, {}, friendlyTanks, {});
    algo->updateBattleInfo(info);
    
    EXPECT_TRUE(isTankAtPosition(Point(2, 2)));
    EXPECT_TRUE(isTankAtPosition(Point(3, 1)));
    EXPECT_FALSE(isTankAtPosition(Point(1, 1)));
}

TEST_F(BasicTankAlgorithmTest, IsTankAtPosition_NoTanks) {
    GameBoard board = makeBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, {});
    algo->updateBattleInfo(info);
    
    EXPECT_FALSE(isTankAtPosition(Point(2, 2)));
    EXPECT_FALSE(isTankAtPosition(Point(1, 1)));
}

TEST_F(BasicTankAlgorithmTest, GetLineOfSightDirection_TankBlocking) {
    GameBoard board = makeBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    // Place enemy tank blocking horizontal path at (2,2)
    std::vector<Point> enemyTanks = {Point(2, 2)};
    BattleInfoImpl info = makeBattleInfo(board, enemyTanks, {}, {});
    algo->updateBattleInfo(info);
    
    // Should not find line of sight through tank
    EXPECT_EQ(getLineOfSightDirection(Point(1,2), Point(3,2)), std::nullopt);
    
    // Tank at (2,2) also blocks diagonal path from (1,1) to (3,3)
    EXPECT_EQ(getLineOfSightDirection(Point(1,1), Point(3,3)), std::nullopt);
    
    // But should find paths that don't go through the tank
    EXPECT_EQ(getLineOfSightDirection(Point(1,1), Point(1,3)), Direction::Down);
}

TEST_F(BasicTankAlgorithmTest, IsInDangerFromShells_Point_ShellBlockedByTank) {
    GameBoard gameBoard = makeBoard({
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####"
    });
    std::vector<Point> shells = {Point(3, 3)};
    std::vector<Point> enemyTanks = {Point(2, 2)}; // Tank blocks shell's path to (1,1)
    BattleInfoImpl info = makeBattleInfo(gameBoard, enemyTanks, {}, shells);
    algo->updateBattleInfo(info);
    EXPECT_FALSE(isInDangerFromShells(Point(1, 1)));
}

} // namespace Algorithm_318835816_211314471

