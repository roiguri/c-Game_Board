#include "gtest/gtest.h"
#include "offensive_tank_algorithm.h"
#include "offensive_battle_info.h"
#include "utils/point.h"
#include "utils/direction.h"

using namespace UserCommon_318835816_211314471;

namespace Algorithm_318835816_211314471 {
#include <memory>
#include <vector>

class OffensiveTankAlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        algo = std::make_unique<TankAlgorithm_318835816_211314471_A>(1, 0);
        setTankState(Point(2,2), Direction::Up);
    }
    std::unique_ptr<TankAlgorithm_318835816_211314471_A> algo;
    // Helper to set tank state
    void setTankState(const Point& pos, Direction dir) {
        setTrackedPosition(pos);
        setTrackedDirection(dir);
        setGameBoard(GameBoard(5,5));
    }
    void setTrackedPosition(const Point& pos) { algo->m_trackedPosition = pos; }
    void setTrackedDirection(Direction dir) { algo->m_trackedDirection = dir; }
    void setGameBoard(const GameBoard& board) { algo->m_gameBoard = board; }
    void setShells(const std::vector<Point>& shells) { algo->m_shells = shells; }
    void setEnemyTanks(const std::vector<Point>& enemies) { algo->m_enemyTanks = enemies; }
    void setCurrentPath(const std::vector<Point>& path) { algo->m_currentPath = path; }
    void setTurnsSinceLastUpdate(int turns) { algo->m_turnsSinceLastUpdate = turns; }
    std::optional<Point> getTargetPosition() const { return algo->m_targetPosition; }
    std::vector<Point> getCurrentPath() const { return algo->m_currentPath; }
    // Helper to create a board from string lines
    GameBoard makeBoard(const std::vector<std::string>& lines) {
        GameBoard board(lines[0].size(), lines.size());
        std::vector<std::pair<int, Point>> tankPositions;
        board.initialize(lines, tankPositions);
        return board;
    }
    // Helper to create and inject battle info with a custom board and target
    void injectBattleInfoWithBoard(const std::vector<std::string>& boardLines, const std::optional<Point>& target, const std::vector<Point>& enemyTanks = {}, const std::vector<Point>& shells = {}) {
        GameBoard board = makeBoard(boardLines);
        OffensiveBattleInfo info(board.getWidth(), board.getHeight());
        for (size_t x = 0; x < board.getWidth(); ++x) {
            for (size_t y = 0; y < board.getHeight(); ++y) {
                info.setCellType(x, y, board.getCellType(x, y));
            }
        }
        for (const auto& p : enemyTanks) info.addEnemyTankPosition(p);
        for (const auto& p : shells) info.addShellPosition(p);
        if (target) info.setTargetTankPosition(*target);
        algo->updateBattleInfo(info);
        setGameBoard(board);
    }
    auto callTurnToShootAction() { return algo->turnToShootAction(); }
    auto callFindPathBFS(const Point& from, const Point& to) { return algo->findPathBFS(from, to); }
    auto callFollowCurrentPath() { return algo->followCurrentPath(); }
    ActionRequest callGetRotationToDirection(Direction current, Direction target) {
        return BasicTankAlgorithm::getRotationToDirection(current, target);
    }
};

TEST_F(OffensiveTankAlgorithmTest, UpdateBattleInfo_SetsAndClearsTarget) {
    injectBattleInfoWithBoard({
        "     ",
        "     ",
        "     ",
        "     ",
        "     "
    }, Point(3,3));
    EXPECT_TRUE(getTargetPosition().has_value());
    EXPECT_EQ(getTargetPosition().value(), Point(3,3));
    injectBattleInfoWithBoard({
        "     ",
        "     ",
        "     ",
        "     ",
        "     "
    }, std::nullopt);
    EXPECT_FALSE(getTargetPosition().has_value());
}

TEST_F(OffensiveTankAlgorithmTest, TurnToShootAction_RotatesCorrectly) {
    injectBattleInfoWithBoard({
        "     ",
        "     ",
        "     ",
        "     ",
        "     "
    }, Point(2,0));
    setTankState(Point(2,2), Direction::Left);
    auto action = callTurnToShootAction();
    ASSERT_TRUE(action.has_value());
    EXPECT_EQ(action.value(), callGetRotationToDirection(Direction::Left, Direction::Up));
}

TEST_F(OffensiveTankAlgorithmTest, FindPathBFS_FindsShortestPath) {
    injectBattleInfoWithBoard({
        "     ",
        "     ",
        "  1 2",
        "     ",
        "     "
    }, Point(4,2));
    setTankState(Point(2,2), Direction::Up);
    auto path = callFindPathBFS(Point(2,2), Point(4,2));
    ASSERT_FALSE(path.empty());
    EXPECT_EQ(path.front(), Point(3,1));
    EXPECT_EQ(path.back(), Point(4,2));
}

TEST_F(OffensiveTankAlgorithmTest, FollowCurrentPath_MoveForward) {
    injectBattleInfoWithBoard({
        "     ",
        "     ",
        "     ",
        "     ",
        "     "
    }, Point(2,0));
    setTankState(Point(2,2), Direction::Up);
    setCurrentPath({Point(2,1), Point(2,0)});
    auto action = callFollowCurrentPath();
    ASSERT_TRUE(action.has_value());
    EXPECT_EQ(action.value(), ActionRequest::MoveForward);
}

TEST_F(OffensiveTankAlgorithmTest, GetAction_Priority1_UpdateBattleInfo) {
    injectBattleInfoWithBoard({
        "     ",
        "     ",
        "     ",
        "     ",
        "     "
    }, Point(2,0));
    setTurnsSinceLastUpdate(4);
    auto action = algo->getAction();
    EXPECT_EQ(action, ActionRequest::GetBattleInfo);
}

TEST_F(OffensiveTankAlgorithmTest, GetAction_Priority2_AvoidDanger) {
    injectBattleInfoWithBoard({
        "     ",
        "     ",
        "     ",
        "     ",
        "     "
    }, Point(2,0), {}, {Point(2,4)});
    setTankState(Point(2,2), Direction::Up);
    auto action = algo->getAction();
    EXPECT_NE(action, ActionRequest::DoNothing);
    EXPECT_NE(action, ActionRequest::Shoot);
}

TEST_F(OffensiveTankAlgorithmTest, GetAction_Priority3_ShootIfPossible) {
    injectBattleInfoWithBoard({
        "     ",
        "     ",
        "     ",
        "     ",
        "     "
    }, Point(2,0), {Point(2,0)});
    setTankState(Point(2,2), Direction::Up);
    auto action = algo->getAction();
    EXPECT_EQ(action, ActionRequest::Shoot);
}

TEST_F(OffensiveTankAlgorithmTest, GetAction_Priority4_TurnToShoot) {
    injectBattleInfoWithBoard({
        "     ",
        "     ",
        "     ",
        "     ",
        "     "
    }, Point(0,2), {Point(0,2)});
    setTankState(Point(2,2), Direction::Up);
    auto action = algo->getAction();
    EXPECT_TRUE(action == ActionRequest::RotateLeft90 || action == ActionRequest::RotateRight90);
}

TEST_F(OffensiveTankAlgorithmTest, GetAction_Priority5_ChaseWithBFS) {
    injectBattleInfoWithBoard({
        "     ",
        "     ",
        "     ",
        "     ",
        "     "
    }, Point(4,2), {Point(4,2)});
    setTankState(Point(2,2), Direction::Up);
    auto action = algo->getAction();
    EXPECT_TRUE(action == ActionRequest::MoveForward || action == ActionRequest::RotateRight90 || action == ActionRequest::RotateLeft90);
}

} // namespace Algorithm_318835816_211314471 