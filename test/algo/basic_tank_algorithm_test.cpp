#include "gtest/gtest.h"
#include "algo/basic_tank_algorithm.h"
#include "game_board.h"
#include "objects/tank.h"
#include "utils/point.h"
#include "players/battle_info_impl.h"
#include <vector>
#include <string>
#include <gmock/gmock.h>
using ::testing::UnorderedElementsAreArray;

class BasicTankAlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        board = makeBoard({
            "#####",
            "#   #",
            "#   #",
            "#   #",
            "#####"
        });
        tank = std::make_unique<Tank>(1, Point(1, 1), Direction::Up);
        algo = std::make_unique<BasicTankAlgorithm>(1, 0);
        algo->setTank(*tank);
    }
    GameBoard board;
    std::unique_ptr<Tank> tank;
    std::unique_ptr<BasicTankAlgorithm> algo;

    // Helper to call private getLineOfSightDirection
    std::optional<Direction> getLineOfSightDirection(const Point& from, const Point& to) {
        return algo->getLineOfSightDirection(from, to);
    }
    // Helper to call private checkLineOfSightInDirection
    bool checkLineOfSightInDirection(const Point& from, const Point& to, Direction dir) {
        return algo->checkLineOfSightInDirection(from, to, dir);
    }
    static GameBoard makeBoard(const std::vector<std::string>& lines) {
        GameBoard board(lines[0].size(), lines.size());
        std::vector<std::string> errors;
        std::vector<std::pair<int, Point>> tankPositions;
        board.initialize(lines, errors, tankPositions);
        return board;
    }
    static BattleInfoImpl makeBattleInfo(const GameBoard& board, const std::vector<Point>& enemyTanks, const std::vector<Point>& friendlyTanks, const std::vector<Point>& shells) {
        BattleInfoImpl info(board.getWidth(), board.getHeight());
        for (int x = 0; x < board.getWidth(); ++x) {
            for (int y = 0; y < board.getHeight(); ++y) {
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
        "#1   #",
        "#    #",
        "#    #",
        "######"
    });
    std::vector<Point> shells = {Point(5, 5)};
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
    tank->setPosition(Point(2, 2));
    algo->setTank(*tank);
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
    tank->setPosition(Point(2, 2));
    algo->setTank(*tank);
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
    tank->setPosition(Point(1, 1));
    algo->setTank(*tank);
    auto safe = getSafePositions();
    // Only (1,2) is safe
    std::vector<Point> expected = {Point(1,2)};
    EXPECT_THAT(safe, UnorderedElementsAreArray(expected));
}

TEST_F(BasicTankAlgorithmTest, GetSafePositions_SomeTanks) {
    std::vector<Point> enemyTanks = {Point(1,1), Point(3,3)};
    BattleInfoImpl info = makeBattleInfo(board, enemyTanks, {}, {});
    algo->updateBattleInfo(info);
    tank->setPosition(Point(2, 2));
    algo->setTank(*tank);
    auto safe = getSafePositions();
    std::vector<Point> expected = {Point(2,1), Point(3,1), Point(1,2), Point(3,2), Point(1,3), Point(2,3)};
    EXPECT_THAT(safe, UnorderedElementsAreArray(expected));
}

TEST_F(BasicTankAlgorithmTest, GetSafePositions_ShellDanger) {
    std::vector<Point> shells = {Point(2, 4)};
    BattleInfoImpl info = makeBattleInfo(board, {}, {}, shells);
    algo->updateBattleInfo(info);
    tank->setPosition(Point(2, 2));
    algo->setTank(*tank);
    auto safe = getSafePositions();
    std::vector<Point> expected = {Point(1,1), Point(1,2), Point(3,1), Point(3,2)};
    EXPECT_THAT(safe, UnorderedElementsAreArray(expected));
}

